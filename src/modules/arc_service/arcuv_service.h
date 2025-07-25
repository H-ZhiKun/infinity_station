#pragma once

#include "kits/common/module_base/ModuleBase.h"
#include "kits/thrid_devices/arc_kits/dwcarcuv/dwcarcuv.h"
#include <QDateTime>
#include <memory>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace _Modules
{

    class ArcUVService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(ArcUVService)

      public:
        ArcUVService();
        ~ArcUVService()=default;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void voltageToController(const QVariant &);
        void sendArcStatisticsToController(TIS_Info::arcData);
        void sendArcVideoPath(const TIS_Info::arcData &);
        void sendDWCVoltage(float);

      public slots:
        // 处理燃弧数据的槽函数
        void OnVoltageReceived(float voltage);
        void OnVideoPathReceived(const TIS_Info::arcData &);

      private:
        YAML::Node m_details;
        std::unique_ptr<_Kits::DWCArcUV> dwcarcuv; // 用于存储 DWCArcUV 实例
        
    };

} // namespace _Modules