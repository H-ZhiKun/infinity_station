#pragma once
#include "kits/common/module_base/ModuleBase.h"
#include "kits/communication/tcp_server/TcpPrivateClient.h"
#include "kits/communication/tcp_server/TcpServer.h"
#include <memory>

namespace _Modules
{

    class TcpServerCenter : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(TcpServerCenter)
      public:
        explicit TcpServerCenter(QObject *parent = nullptr);
        virtual ~TcpServerCenter() noexcept;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void newClient(std::shared_ptr<_Kits::TcpPrivateClient> client); // controller中同步绑定，获取目标业务客户端
        void clientDisconnect(qintptr socketId);                         // controller中同步绑定，删除目标业务客户端

      private:
        std::unique_ptr<_Kits::TcpServer> m_server = nullptr;
        YAML::Node m_config; // 配置保存
    };

} // namespace _Modules
