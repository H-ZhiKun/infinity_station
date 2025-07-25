#pragma once
#include "kits/common/controller_base/ControllerBase.h"
namespace _Controllers
{
    class LocationController : public ControllerBase<LocationController>
    {
      public:
        void sendPositToXJ(const QVariant &); // 巡检
        void sendPositTypeToPage(int);
        void sendPositToPage(const QVariant &);
        void doDisplay(const QVariant &);
        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::LocationService::sendPositionData, LocationController::sendPositToPage);
        ASYNC_TASK_ADD(TIS_Info::LocationService::sendPositionData, LocationController::sendPositToXJ);
        ASYNC_TASK_ADD(TIS_Info::LocationService::sendPositionData, LocationController::doDisplay);
        ASYNC_TASK_ADD(TIS_Info::LocationService::sendPositionType, LocationController::sendPositTypeToPage);
        TASK_LIST_END
    };
} // namespace _Controllers
