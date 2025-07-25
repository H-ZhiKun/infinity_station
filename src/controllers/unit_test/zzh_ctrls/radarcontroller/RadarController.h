#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include "tis_global/Function.h"
namespace _Controllers
{
    class RadarController : public ControllerBase<RadarController>
    {
      public:
        void sendState(const QVariant &);
        void sendData(std::vector<std::vector<TIS_Info::DeviceSingleData>>);
        void sendOverInfo(QString);
        void sendOverConfig(const QVariant &);
        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::RadarService::StateSendQml, RadarController::sendState);
        ASYNC_TASK_ADD(TIS_Info::RadarService::DataSendQml, RadarController::sendData);
        ASYNC_TASK_ADD(TIS_Info::RadarService::OverinfoSendQml, RadarController::sendOverInfo);
        ASYNC_TASK_ADD(TIS_Info::RadarService::OverconfigSendQml, RadarController::sendOverConfig);
        TASK_LIST_END

      private:
        // QVariantMap CreateMapToQml(QVariant &data);
    };
} // namespace _Controllers