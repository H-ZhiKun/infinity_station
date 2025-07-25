#pragma once
#include "RemoteBase.h"
#include "tis_global/Struct.h"
#include <queue>
#include <thread>
#include <mutex>
#include <vector>
namespace _Kits
{
    class RemotePublisher : public RemoteBase
    {
      public:
        RemotePublisher() = default;
        ~RemotePublisher() = default;

        bool initialize(const std::string &protocol, const std::string &ip, const std::string &port, int timeout, int hwm);
        void stop();
        bool publish(const TIS_Info::RemoteMessage &msg);
        bool publishAsync(const TIS_Info::RemoteMessage &msg);

      private:
        void processMessage();
        void batchSend(std::vector<zmq::multipart_t> &);
        std::thread m_thWork;
        std::atomic_bool m_bRunning{false};
        std::queue<zmq::multipart_t> m_messages;
        std::condition_variable m_cvMsg;
        std::mutex m_mtxMsg;
    };
} // namespace _Kits