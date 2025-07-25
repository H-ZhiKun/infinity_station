#pragma once

#include "arcbase.h"
#include <QObject>
#include "kits/communication/udp/UdpClient.h"
#include <string>
#include <vector>

namespace _Kits
{
    class Arcbee : public QObject
    {
      public:
        Arcbee(std::string title);
        ~Arcbee();

      public slots:
        bool onInitArc(const std::unordered_map<std::string, std::shared_ptr<_Kits::UdpClient>> &map_udpClient);
        void onReadSockDatagrams(std::shared_ptr<std::vector<QByteArray>> data);

      private:
        void processVoltageData();

      private:
        std::string mstr_title = "";

        std::thread mthread_arcProcess;
        std::mutex m_mtx_arcProcess;
        std::condition_variable m_cond_arcProcess;
        std::vector<QByteArray> m_list_arcProcess_data;
        std::atomic_bool mb_arcProcess_running = false;
    };

} // namespace _Kits