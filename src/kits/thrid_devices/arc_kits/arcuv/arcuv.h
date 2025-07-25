#pragma once

#include <string>
#include <memory>
#include <QObject>
#include "kits/communication/udp/UdpClient.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <QDateTime>
#include <vector>
#include "kits/common/database/orm/TableStructs.h"

namespace _Kits
{
    class ArcUV : public QObject
    {
        Q_OBJECT
      public:
        ArcUV(std::string title);
        virtual ~ArcUV();

        bool start();

      private:
        void processVoltageData();

      private slots:

      public slots:
        bool onInitArc(const std::unordered_map<std::string, std::shared_ptr<_Kits::UdpClient>> &map_udpClient);
        void onReadSockDatagrams(cstd::shared_ptr<std::vector<QByteArray>> data);

      signals:
        void voltageReceived(double);

      private:
        std::string mstr_ip = "";
        uint16_t mus_port = 0;
        std::string mstr_title = "";

        std::thread mthread_arcProcess;
        std::mutex m_mtx_arcProcess;
        std::condition_variable m_cond_arcProcess;
        std::vector<QByteArray> m_list_arcProcess_data;
        std::atomic_bool mb_arcProcess_running = false;
    };
} // namespace _Kits