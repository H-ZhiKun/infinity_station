#pragma once

#include "kits/communication/udp/UdpClient.h"
#include "kits/common/module_base/ModuleBase.h"
#include <QObject>
#include "tis_global/Struct.h"
#include <vector>

namespace _Modules
{
    class DwcDataDispatch : public _Kits::ModuleBase
    {
      public:
        Q_OBJECT
        DECLARE_MODULE(DwcDataDispatch)

      public:
        explicit DwcDataDispatch();
        virtual ~DwcDataDispatch() noexcept;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      public slots:
        void getElectricDataFromCtrl(const float);
        void getVoltageDataFromCtrl(const float);
        void getAccData0xFromCtrl(const float);
        void getPressData0FromCtrl(const float);

      signals:
        void startCtrl();
        void sendElectriDataOut(const float); // 电流
        void sendVoltageDataOut(const float); // 电压
        void sendPressData0Out(const float);
        void sendAccData0xOut(const float); // 振动 x
    };
} // namespace _Modules