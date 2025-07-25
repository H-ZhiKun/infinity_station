#pragma once
#include "ConcurrentSniffer.h"
#include "kits/common/log/CRossLogger.h"
#include <QtConcurrent/QtConcurrent>
#include <atomic>
#include <functional>
#include <mutex>
#include <qfuture.h>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_set>

namespace _Kits
{
    class ConcurrentPool
    {
      public:
        static void stop()
        {
            ref().m_bRun = false;
        }
        /**
         * @brief 立即异步执行一个普通可调用对象（如 Lambda 或自由函数）
         * @tparam Ret 期望的返回值类型（默认为 void）
         * @tparam Func 可调用对象类型
         * @tparam Args 传递给 task 的参数类型
         * @param task 可调用对象
         * @param args 可调用对象的参数
         * @return QFuture<Ret> 可用于后续结果等待
         */
        template <typename Func, typename... Args, std::enable_if_t<std::is_invocable_v<Func, Args...>, int> = 0>
        static auto runTask(Func &&task, Args &&...args);

        /**
         * @brief 立即异步执行一个成员函数调用
         * @tparam Ret 期望的返回值类型（默认为 void）
         * @tparam Object 对象实例类型
         * @tparam Func 成员函数指针类型
         * @tparam Args 成员函数参数类型
         * @param obj 成员函数所属对象实例
         * @param task 成员函数指针
         * @param args 成员函数参数
         * @return QFuture<Ret> 可用于后续结果等待
         */
        template <typename Object,
                  typename Func,
                  typename... Args,
                  std::enable_if_t<std::is_member_function_pointer_v<std::decay_t<Func>>, int> = 0>
        static auto runTask(Object &&obj, Func &&task, Args &&...args);

        /**
         * @brief 延时执行一个普通可调用对象
         * @param milliseconds 延时毫秒数（0 则立即执行）
         * @param function 外部封包
         */
        static bool runAfter(uint64_t milliseconds, std::function<void()> &&function);
        /**
         * @brief 周期性执行一个成员函数任务
         * @param timerName 定时任务名称（用于管理和取消）
         * @param milliseconds 周期毫秒数
         * @param function 外部封包
         */
        static bool runEvery(const std::string &timerName, uint64_t milliseconds, std::function<void()> &&function);
        static bool timerCancel(const std::string &timerName);

        ~ConcurrentPool()
        {
            {
                std::lock_guard<std::mutex> lock(mtxTimer_);
                cvTimer_.notify_one();
            }
            if (thTimer_.joinable())
                thTimer_.join();
        }

      private:
        ConcurrentPool()
        {
            thTimer_ = std::thread([this] { timerThreadFunc(); });
        };
        [[nodiscard]] static ConcurrentPool &ref()
        {
            static ConcurrentPool ins;
            return ins;
        }
        struct TimerTask
        {
            std::string taskName;
            std::chrono::steady_clock::time_point nextTrigger;
            std::chrono::milliseconds interval;
            bool repeat;
            std::function<void()> task;
            bool operator<(const TimerTask &other) const
            {
                if (nextTrigger == other.nextTrigger)
                    return taskName < other.taskName;
                return nextTrigger < other.nextTrigger;
            }
        };
        bool addTimerTask(const std::string &timerName, uint64_t milliseconds, bool repeat, std::function<void()> &&func);
        void timerThreadFunc();
        std::mutex mtxTimer_;
        std::atomic_bool m_bRun = true;
        std::condition_variable cvTimer_;
        std::multiset<TimerTask> tasks_;
        std::mutex mtxCancel_;
        std::unordered_set<std::string> cancels_;
        std::thread thTimer_;
        std::atomic_uint64_t serializeId_ = 0;
    };

    inline bool ConcurrentPool::timerCancel(const std::string &timerName)
    {
        std::lock_guard<std::mutex> lock(ref().mtxCancel_);
        ref().cancels_.emplace(timerName);
        return true;
    }

    template <typename Func, typename... Args, std::enable_if_t<std::is_invocable_v<Func, Args...>, int>>
    inline auto ConcurrentPool::runTask(Func &&task, Args &&...args)
    {
        using Ret = std::invoke_result_t<Func, Args...>;
        if (!ref().m_bRun)
            return QFuture<Ret>{};
        auto future = QtConcurrent::run(std::forward<Func>(task), std::forward<Args>(args)...);
        ConcurrentSniffer<Ret>::acquire(future, typeid(std::decay_t<decltype(task)>).name());
        return future;
    }

    template <typename Object,
              typename Func,
              typename... Args,
              std::enable_if_t<std::is_member_function_pointer_v<std::decay_t<Func>>, int>>
    inline auto ConcurrentPool::runTask(Object &&obj, Func &&task, Args &&...args)
    {
        if (!ref().m_bRun)
            return false;
        using Ret = std::invoke_result_t<Func, Object, Args...>;

        auto future = QtConcurrent::run(std::forward<Object>(obj), std::forward<Func>(task), std::forward<Args>(args)...);
        ConcurrentSniffer<Ret>::acquire(future, typeid(std::decay_t<decltype(task)>).name());
        return future;
    }

    inline bool ConcurrentPool::runAfter(uint64_t milliseconds, std::function<void()> &&function)
    {
        if (!ref().m_bRun)
            return false;
        if (milliseconds == 0)
        {
            runTask(std::move(function));
            return true;
        }

        return ref().addTimerTask("", milliseconds, false, std::forward<std::function<void()>>(function));
    }

    inline bool ConcurrentPool::runEvery(const std::string &timerName, uint64_t milliseconds, std::function<void()> &&function)
    {
        if (!ref().m_bRun)
            return false;
        if (timerName.empty())
        {
            LogError("timer name can not be empty.");
            return false;
        }
        if (milliseconds == 0)
        {
            LogError("milliseconds can not be 0.");
            return false;
        }

        return ref().addTimerTask(timerName, milliseconds, true, std::forward<std::function<void()>>(function));
    }

    inline bool ConcurrentPool::addTimerTask(const std::string &timerName, uint64_t milliseconds, bool repeat, std::function<void()> &&func)
    {
        std::lock_guard<std::mutex> lock(mtxTimer_);
        std::string taskName = timerName;
        if (!timerName.empty())
        {
            for (auto it = tasks_.begin(); it != tasks_.end(); ++it)
            {
                if (it->taskName == timerName)
                {
                    LogError("timerName '{}' already existed when adding timerTask.", timerName);
                    return false;
                }
            }
        }
        else
        {
            taskName = std::to_string(++serializeId_);
        }
        TimerTask timerTask;
        timerTask.taskName = taskName;
        timerTask.nextTrigger = std::chrono::steady_clock::now() + std::chrono::milliseconds(milliseconds);
        timerTask.interval = std::chrono::milliseconds(milliseconds);
        timerTask.repeat = repeat;
        timerTask.task = std::move(func);
        tasks_.emplace(timerTask);
        cvTimer_.notify_one();
        return true;
    }

    inline void ConcurrentPool::timerThreadFunc()
    {
        std::unique_lock<std::mutex> lock(mtxTimer_);
        while (!m_bRun)
        {
            if (tasks_.empty())
            {
                cvTimer_.wait(lock, [this] { return m_bRun || !tasks_.empty(); });
            }
            else
            {
                auto nextTriggerTime = tasks_.begin()->nextTrigger;
                cvTimer_.wait_until(lock, nextTriggerTime, [this, nextTriggerTime] {
                    return m_bRun || tasks_.empty() || tasks_.begin()->nextTrigger < nextTriggerTime;
                });
            }

            if (m_bRun)
                break;

            // 注意这里可能 wait 后 tasks_ 变化了
            size_t maxTasksPerLoop = 100;
            size_t processedTasks = 0;

            while (!tasks_.empty() && processedTasks <= maxTasksPerLoop)
            {
                auto timeNow = std::chrono::steady_clock::now();
                if (tasks_.begin()->nextTrigger > timeNow)
                    break;
                TimerTask task = *tasks_.begin();
                tasks_.erase(tasks_.begin());
                lock.unlock();

                bool canceled = false;
                if (!task.taskName.empty())
                {
                    std::lock_guard<std::mutex> lockCancel(mtxCancel_);
                    auto it = cancels_.find(task.taskName);
                    if (it != cancels_.end())
                    {
                        cancels_.erase(it);
                        canceled = true;
                    }
                }

                if (!canceled)
                {
                    runTask(task.task);
                }

                lock.lock();

                if (!canceled && task.repeat && !m_bRun)
                {
                    task.nextTrigger = timeNow + task.interval;
                    tasks_.emplace(std::move(task));
                }
            }
            ++processedTasks;
        }
    }
} // namespace _Kits
