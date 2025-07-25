#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include <qvariant.h>
#include <string>
namespace _Controllers
{
    class EgvInspectionImagePageController : public ControllerBase<EgvInspectionImagePageController>
    {
      public:
        void onCameraStatusGet(const QVariant &);

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::camera_button_clicked, EgvInspectionImagePageController::onCameraStatusGet);

        TASK_LIST_END

      private:
    };
} // namespace _Controllers