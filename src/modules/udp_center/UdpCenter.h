#pragma once
#include "kits/common/module_base/ModuleBase.h"
#include "kits/communication/udp/UdpClient.h"
namespace _Modules
{

    class UdpCenter : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(UdpCenter)

      public:
        explicit UdpCenter(QObject *parent = nullptr);
        virtual ~UdpCenter() noexcept;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void dispatchClient(const std::unordered_map<std::string, std::shared_ptr<_Kits::UdpClient>> &clients); // 派发客户端

      private:
        std::unordered_map<std::string, std::shared_ptr<_Kits::UdpClient>> m_storeClients;
        YAML::Node m_config;
    };

} // namespace _Modules
