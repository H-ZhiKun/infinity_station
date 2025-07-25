#pragma once

#include "kits/rfid_recv_kits/RfidBase/RfidBase.h"
#include "kits/communication/tcp_client/TcpClient.h"

namespace _Kits
{

    class RfidTcp : public RfidBase
    {
        Q_OBJECT

      public:
        RfidTcp();

        virtual ~RfidTcp() override;

        virtual bool Init(const YAML::Node &) override;
        virtual bool Start() override;
        virtual bool Stop() override;

      private:
        std::unique_ptr<TcpClient> m_pTcpClient;
        QString mqstr_host;
        quint16 mqui_port;

        int marktoint(const uint8_t *mark);

      protected slots:
        virtual void OnrecvData(const QByteArray &data) override;
    };

} // namespace _Kits