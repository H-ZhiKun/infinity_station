#pragma once

#include "kits/common/factory/ModuleRegister.h"
#include "kits/common/module_base/ModuleBase.h"
#include "kits/thrid_devices/rfid_recv_kits/RfidBase/RfidBase.h"

#if defined(USE_KIT_RFID_VFR61M)
#include "kits/thrid_devices/rfid_recv_kits/RfidVFR61M/RfidVFR61M.h"
#endif

#if defined(USE_KIT_RFID_ANOTHER_TCP)
#include "kits/thrid_devices/rfid_recv_kits/RfidTcpAnother/RfidTcpAnother.h"
#endif

#include <memory>

namespace _Modules
{
    class RfidRecvService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(RfidRecvService)

      public:
        RfidRecvService();
        virtual ~RfidRecvService() noexcept override; // 显式声明noexcept
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      private:
        // 处理RFID数据
        void processRfidData(const QString &data);
        std::unique_ptr<_Kits::RfidBase> m_pRfidBase;
        bool mb_isSave_natural = false; // 是否保存原始数据

      private slots:

      signals:
        void sendRfidData(const QString &data);         // 发送RFID数据到其他模块
        void sendCheckBackData(const QByteArray &data); // 发送回查数据到其他模块
        void isSaveNaturalData(bool);                   // 是否保存原始数据信号
        void sendNaturalData(const std::vector<uint8_t>, const QString);
    };
} // namespace _Modules