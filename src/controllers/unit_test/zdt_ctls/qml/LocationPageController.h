#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include <qvariant.h>
#include <string>
namespace _Controllers
{
    class LocationPageController : public ControllerBase<LocationPageController>
    {
      public:
        void onULPoleCalibrationSended(const QVariant &data); // 上下杆号校准
        void onPoleCalibrationSended(const QVariant &);       // 站区杆号校准

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::ULPoleCalibration, LocationPageController::onULPoleCalibrationSended);
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::PoleCalibration, LocationPageController::onPoleCalibrationSended);

        TASK_LIST_END

      private:
    };
} // namespace _Controllers