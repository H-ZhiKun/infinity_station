
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace _Kits
{
template <typename T>
class BlockingQueue {
public:
    explicit BlockingQueue(size_t max_size) : m_max_size(max_size) {}

    void pushItem(const T& item) 
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cond_producer.wait(lock, [this] { return m_queue.size() < m_max_size; });

        m_queue.push(item);
        m_max_size++;
        m_cond_consumer.notify_one();
    }

    void pushItemNoBlocking(const T& item) 
    {
       std::unique_lock<std::mutex> lock(m_mtx);
       if(m_queue.size() >= m_max_size)
       {
            m_queue.pop();
       }
        m_queue.push(item);
        m_cond_consumer.notify_one();
    }

    T popItem() 
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cond_consumer.wait(lock, [this] { return !m_queue.empty(); });

        T item = m_queue.front();
        m_queue.pop();
        m_cond_producer.notify_one();
        return item;
    }

private:
    std::queue<T> m_queue;
    size_t m_max_size = 0;
    std::mutex m_mtx;
    std::condition_variable m_cond_producer;
    std::condition_variable m_cond_consumer;
};

}