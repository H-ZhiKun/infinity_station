#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include <qvariant.h>
namespace _Controllers
{
    class RadarPageController : public ControllerBase<RadarPageController>
    {
      public:
        void onTaskIdChoosed(const QVariant &);

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::offlineRadarTaskidGet, RadarPageController::onTaskIdChoosed);
        TASK_LIST_END
    };
} // namespace _Controllers