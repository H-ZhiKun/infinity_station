#pragma once
#include "kits/common/factory/ControllerRegister.h"
#include "kits/common/object_pool/ObjectPool.h"
#include "kits/common/log/CRossLogger.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFutureWatcher>
#include <cstddef>
#include <mutex>
#include <string>
#include "kits/common/event_thread/EventThread.h"
#include "tis_global/Field.h"

namespace _Kits
{

    template <typename T = void>
    class ConcurrentSniffer
    {
        struct FutureWatcherInfo : public QFutureWatcher<T>
        {
            std::string info_;
            QElapsedTimer startTime_;
            uint64_t id_ = 0;
            void setInfo(const std::string &info, uint64_t id)
            {
                info_ = info;
                id_ = id;
                startTime_.start();
            }
            qint64 elapsedMs() const
            {
                return startTime_.elapsed();
            }
        };

      public:
        static void acquire(const QFuture<T> &future, const std::string &info)
        {
            auto obj = pool_->getObject([](auto *) {},
                                        [](FutureWatcherInfo *ptr) {
                                            if (ptr)
                                            {
                                                ptr->disconnect();
                                            }
                                        });

            if (!obj)
            {
                LogError("[ConcurrentSniffer] Failed to acquire FutureWatcherInfo from pool.");
                return;
            }
            auto &evtThread = _Kits::EventThread::instance();
            if (obj->thread() != evtThread.thread(TIS_Info::EventThread::eventloop_thread_pool))
            {
                evtThread.moveTreeToThread(TIS_Info::EventThread::eventloop_thread_pool, obj.get());
            }

            uint64_t id = nextId_.fetch_add(1, std::memory_order_relaxed);
            obj->setInfo(info, id);
            aliveTask(id, obj);
            QObject::connect(obj.get(), &QFutureWatcher<T>::finished, [id, wacther = obj.get(), info]() {
                removeTask(id);
                const auto &fut = wacther->future();
                if (fut.isCanceled())
                {
                    LogError("[ConcurrentSniffer] Task canceled, Info: {}", info);
                }
                else if (fut.isFinished())
                {
                    // 正常结束不记录
                    // LogInfo("[ConcurrentSniffer] Task finished, Info: {}", info);
                }
                else
                {
                    LogError("[ConcurrentSniffer] Task unknown state, Info: {}", info);
                }
            });
            obj->setFuture(future);
        }
        static void logTasks()
        {
            std::string output;

            constexpr size_t taskTopCount = 10;
            size_t taskCount = 0;
            for (const auto &[_, task] : aliveTasks_)
            {
                const auto &fut = task->future();
                std::string stateStr = getFutureState(fut);
                if (stateStr == "Running")
                {
                    stateStr = "Running";
                }

                output += fmt::format("\n[Task {}]\n"
                                      "  - Elapsed: {} ms\n"
                                      "  - State  : {}\n"
                                      "  - Info   :\n"
                                      "    {}\n",
                                      task->id_,
                                      task->elapsedMs(),
                                      stateStr,
                                      task->info_);
                taskCount++;
                if (taskCount >= taskTopCount)
                {
                    break;
                }
            }
            output += fmt::format("\n[ConcurrentSniffer] ===== Alive Tasks count {} =====", aliveTasks_.size());
            aliveTasks_.clear();

            LogError("\n[ConcurrentSniffer] ===== Alive Top 10 Tasks Start ====={}"
                     "\n[ConcurrentSniffer] ===== Alive Top 10 Tasks End =====\n",
                     output);
        }

      private:
        static void aliveTask(uint64_t id, std::shared_ptr<FutureWatcherInfo> task)
        {
            std::lock_guard<std::mutex> lock(mtxAlive_);
            aliveTasks_.emplace(id, task);

            if (aliveTasks_.size() >= maxAliveCount_)
            {
                logTasks();
            }
        }

        static void removeTask(uint64_t id)
        {
            std::lock_guard<std::mutex> lock(mtxAlive_);
            auto it = aliveTasks_.find(id);
            if (it != aliveTasks_.end())
            {
                aliveTasks_.erase(it);
                // LogInfo("[ConcurrentSniffer] Alive erase id = {}", id);
                // LogInfo("[ConcurrentSniffer] Alive erase Tasks size = {}", aliveTasks_.size());
            }
            else
            {
                LogError("[ConcurrentSniffer] Alive erase id = {} not found!", id);
            }
        }

        static std::string getFutureState(const QFuture<T> &future)
        {
            if (!future.isValid())
                return "Invalid";
            else if (future.isCanceled())
                return "Canceled";
            else if (future.isFinished())
                return "Finished";
            else if (future.isSuspended())
                return "Suspended";
            else if (future.isSuspending())
                return "Suspending";
            else if (future.isRunning())
                return "Running";
            else if (future.isStarted())
                return "Started";

            return "Unknown";
        }

      private:
        static inline std::shared_ptr<ObjectPool<FutureWatcherInfo>> pool_ = std::make_shared<ObjectPool<FutureWatcherInfo>>();
        static inline std::mutex mtxAlive_;
        static inline std::recursive_mutex rsmtxAlive_;
        static inline std::unordered_map<uint64_t, std::shared_ptr<FutureWatcherInfo>> aliveTasks_;
        static inline std::atomic<uint64_t> nextId_ = 1;
        static inline constexpr size_t maxAliveCount_ = 1024;
    };

} // namespace _Kits
