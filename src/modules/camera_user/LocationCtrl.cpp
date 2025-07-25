#include "LocationCtrl.h"
#include <QDateTime>

LocationCtrl::LocationCtrl(/* args */)
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

void LocationCtrl::initLocationCtrl()
{
    m_startFlag.store(true);
    m_threadfor_record = std::thread(&LocationCtrl::recordByLocation, this);
}

void LocationCtrl::updateLocation(const std::string& newlocation)
{
    std::lock_guard locker(m_mutexfor_location);
    m_str_new_location = newlocation;
    m_waitCondition.notify_one();
}
void LocationCtrl::recordByLocation()
{
    std::unique_lock locker(m_mutexfor_location);
    while (m_startFlag.load())
    {
        m_waitCondition.wait(locker);

        if (!m_startFlag.load())
        {
            break;
        }

        if (m_str_new_location != m_str_now_location)
        {
            QDateTime dateTime = QDateTime::currentDateTime();
            
            auto fliename = m_str_now_location + "-" + m_str_new_location + dateTime.toString("yyyy-MM-dd-hh-mm-ss").toStdString();
            emit updateLocationSignal(fliename);

            m_str_now_location = m_str_new_location;
        }
    }
}