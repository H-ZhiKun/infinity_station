#include "LocationCtrl.h"
#include <QDateTime>

using namespace _Kits;
LocationCtrl::LocationCtrl(/* args */) : m_startFlag(true), m_threadfor_record(std::thread(&LocationCtrl::recordByLocation, this))
{
}

LocationCtrl::~LocationCtrl()
{
    m_startFlag.store(false);
    m_waitCondition.notify_all();

    if (m_threadfor_record.joinable())
    {
        m_threadfor_record.join();
    }
}

void LocationCtrl::updateLocation(const QString &newlocation)
{
    {
        std::lock_guard locker(m_mutexfor_location);
        m_str_new_location = newlocation;
    }
    if (newlocation.isEmpty())
    {
        m_str_now_location.clear();
        return;
    }

    m_waitCondition.notify_one();
}

void LocationCtrl::recordByLocation()
{
    std::unique_lock<std::mutex> locker(m_mutexfor_location);
    while (m_startFlag.load())
    {
        // 安全等待：满足位置变化或线程退出时才唤醒
        m_waitCondition.wait(locker, [this] { return !m_startFlag.load() || (m_str_new_location != m_str_now_location); });

        if (!m_startFlag.load())
            break;

        // 复制新位置值（锁内）
        const QString newLocation = m_str_new_location;

        // 释放锁执行耗时操作
        locker.unlock();

        // 生成文件名（无锁操作）
        const QString filename = m_str_now_location + "-" + newLocation + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");

        // 发出信号（QT信号线程安全）
        emit updateLocationSignal(filename);

        // 重新加锁更新状态
        locker.lock();
        m_str_now_location = newLocation;
    }
}