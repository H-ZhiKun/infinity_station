#pragma once
#include <QObject>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class LocationCtrl:public QObject
{
    Q_OBJECT
private:
    /* data */
public:
    LocationCtrl(/* args */);
    ~LocationCtrl();
    void initLocationCtrl();
    void updateLocation(const std::string& newlocation);
    std::string getLocation(){return m_str_now_location;}
signals:
    void updateLocationSignal(const std::string& filename);
private:
    void recordByLocation();
private:
    std::thread m_threadfor_record;
    std::string m_str_new_location;
    std::string m_str_now_location;
    std::mutex m_mutexfor_location;
    std::condition_variable m_waitCondition;
    std::atomic_bool m_startFlag = false;
};

