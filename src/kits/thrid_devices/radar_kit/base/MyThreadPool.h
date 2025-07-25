#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <QDebug>

class ThreadPool
{
  public:
    // 构造函数：初始化线程池
    explicit ThreadPool(size_t numThreads) : stop(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers.emplace_back([this, numThreads]() {
                while (!this->stop)
                { // 由stop完成退出
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this]() { return this->stop || !this->tasks.empty(); });

                        if (this->stop && this->tasks.empty())
                        {
                            return;
                        }
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                        while (tasks.size() > numThreads * 8)
                        {
                            tasks.pop();
                            qDebug() << "task pop:" << tasks.size();
                        }
                    }
                    task();
                }
            });
        }
    }

    // 添加任务到线程池
    template <class F>
    void enqueue(F &&f)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop)
            {
                throw std::runtime_error("ThreadPool is stopped");
            }
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    // 添加任务到线程池（带可变参数版本）
    template <class F, class... Args>
    void enqueue(F &&f, Args &&...args)
    {
        // 使用完美转发绑定函数和参数
        auto bound_task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop)
            {
                return; // 如果线程池已停止，则不添加新任务
            }

            // 将绑定后的任务包装为 void() 类型
            tasks.emplace([bound_task]() mutable { bound_task(); });
        }
        condition.notify_one();
    }

    // 停止线程池
    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

  private:
    std::vector<std::thread> workers;        // 工作线程集合
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex queueMutex;                   // 互斥锁
    std::condition_variable condition;       // 条件变量
    std::atomic<bool> stop;                  // 停止标志
};