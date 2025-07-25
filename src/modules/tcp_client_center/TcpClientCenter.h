#pragma once
#include "kits/common/module_base/ModuleBase.h"
#include "kits/communication/tcp_client/TcpClient.h"
#include <memory>
#include <unordered_map>

namespace _Modules
{

    class TcpClientCenter : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(TcpClientCenter)
      public:
        explicit TcpClientCenter(QObject *parent = nullptr);
        virtual ~TcpClientCenter() noexcept;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void dispatchClient(const std::unordered_map<std::string, std::shared_ptr<_Kits::TcpClient>> &clients); // 派发客户端

      private:
        std::unordered_map<std::string, std::shared_ptr<_Kits::TcpClient>> m_storeClients;
        YAML::Node m_config; // 配置保存
    };

} // namespace _Modules
