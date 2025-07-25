#pragma once
#include <QThread>
#include <cstdint>
#include <memory>
#include <mutex>
#include <qcoreapplication.h>
#include <shared_mutex>
#include <unordered_map>
#include <QEventLoop>
#include "kits/common/log/CRossLogger.h"

namespace _Kits
{
    class EventThread
    {
      public:
        ~EventThread() noexcept = default;
        EventThread(const EventThread &) = delete;
        EventThread &operator=(const EventThread &) = delete;

        static EventThread &instance()
        {
            static EventThread ins;
            return ins;
        }

        void stop();
        QThread *thread(const std::string &entName);
        void moveTreeToThread(const std::string &entName, QObject *obj);

      private:
        EventThread() = default;

        std::unordered_map<std::string, std::unique_ptr<QThread>> m_mapEvents;
        mutable std::shared_mutex m_mtxEnt;
        uint16_t m_entCount = 0;
    };

    inline void EventThread::stop()
    {
        std::unique_lock lock(m_mtxEnt);
        for (auto &[key, thread] : m_mapEvents)
        {
            thread->quit();
            thread->wait();
        }
        m_mapEvents.clear();
    }

    inline QThread *EventThread::thread(const std::string &entName)
    {
        {
            std::shared_lock lock(m_mtxEnt);
            auto it = m_mapEvents.find(entName);
            if (it != m_mapEvents.end())
            {
                return it->second.get();
            }
        }

        {
            std::unique_lock lock(m_mtxEnt);
            auto uniqueThread = std::make_unique<QThread>();
            uniqueThread->start();
            _Kits::LogInfo("[event thread]: new event thread loop {}, {}.", entName, ++m_entCount);
            auto [it, inserted] = m_mapEvents.emplace(entName, std::move(uniqueThread));
            return it->second.get();
        }
    }

    inline void EventThread::moveTreeToThread(const std::string &entName, QObject *obj)
    {
        if (!obj)
        {
            return;
        }

        // 获取目标线程（主线程或工作线程）
        QThread *targetThread = (entName == "eventloop_main") ? QCoreApplication::instance()->thread() : thread(entName);

        if (!targetThread)
        {
            return;
        }

        // 如果已经在目标线程则跳过
        if (obj->thread() == targetThread)
        {
            return;
        }

        // 在对象当前所属线程中执行移动
        if (QThread::currentThread() != obj->thread())
        {
            QMetaObject::invokeMethod(
                obj,
                [obj, targetThread]() { obj->moveToThread(targetThread); },
                Qt::BlockingQueuedConnection); // 阻塞等待移动完成
        }
        else
        {
            obj->moveToThread(targetThread);
        }

        // 递归处理子对象（现在它们会自动继承新线程）
        const auto children = obj->children();
        for (QObject *child : children)
        {
            if (child)
            { // 额外判空保护
                moveTreeToThread(entName, child);
            }
        }
    }
} // namespace _Kits