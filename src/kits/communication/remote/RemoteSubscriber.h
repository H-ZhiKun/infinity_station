#pragma once
#include "RemoteBase.h"
#include "tis_global/Struct.h"
#include <thread>
#include <mutex>
namespace _Kits
{
    using RemoteMessageCallback = std::function<void(const TIS_Info::RemoteMessage &)>;
    class RemoteSubscriber : public RemoteBase
    {
      public:
        RemoteSubscriber() = default;
        ~RemoteSubscriber() = default;

        bool initialize(const std::string &protocol,
                        const std::string &ip,
                        const std::string &port,
                        int timeout,
                        int hwm,
                        const std::vector<std::string> &topics = {});
        void stop();
        void registerCallback(const std::string &topic, RemoteMessageCallback callback);
        void unregisterCallback(const std::string &topic);

      private:
        void messageLoop();

        std::thread m_thWorker;
        std::atomic_bool m_bRunning{false};
        std::unordered_map<std::string, RemoteMessageCallback> m_callbacks;
        std::mutex m_mtxCallback;
        std::vector<std::string> m_topics;
    };
} // namespace _Kits