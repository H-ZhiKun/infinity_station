#pragma once

#include "kits/communication/udp/UdpClient.h"
#include "kits/common/module_base/ModuleBase.h"
#include <QObject>
#include "tis_global/Struct.h"
#include <vector>

namespace _Modules
{
    class Dispatch6017 : public _Kits::ModuleBase
    {
      public:
        Q_OBJECT
        DECLARE_MODULE(Dispatch6017)

      public:
        explicit Dispatch6017();
        virtual ~Dispatch6017() noexcept;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      public slots:
        void getElectricDataFromCtrl(const double);
        void getPressData0FromCtrl(const double);
        void getPressData1FromCtrl(const double);
        void getPressData2FromCtrl(const double);
        void getPressData3FromCtrl(const double);
        void getSpotData0xFromCtrl(const double);
        void getSpotData0yFromCtrl(const double);

      signals:
        void sendConfigToCtrl(const std::vector<TIS_Info::_6017Info>);
        void sendElectriDataOut(const double);
        void sendPressData0Out(const double);
        void sendPressData1Out(const double);
        void sendPressData2Out(const double);
        void sendPressData3Out(const double);
        void sendSpotData0xOut(const double);
        void sendSpotData0yOut(const double);

        void sendNaturalDataOut(const QByteArray data, const QHostAddress senderAddress, const quint16 senderPort);

      private:
        std::vector<TIS_Info::_6017Info> _mvec_channelInfo;
    };
} // namespace _Modules