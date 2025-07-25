#pragma once
#include <QObject>
#include <QString>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace _Kits
{

    class LocationCtrl : public QObject
    {
        Q_OBJECT
      private:
        /* data */
      public:
        LocationCtrl(/* args */);
        ~LocationCtrl();
        void updateLocation(const QString &newlocation);
        QString getLocation()
        {
            return m_str_now_location;
        }
      signals:
        void updateLocationSignal(QString filename);

      private:
        void recordByLocation();

      private:
        std::thread m_threadfor_record;
        QString m_str_new_location;
        QString m_str_now_location;
        std::mutex m_mutexfor_location;
        std::condition_variable m_waitCondition;
        std::atomic_bool m_startFlag = false;
    };

} // namespace _Kits