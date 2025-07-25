#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include <qvariant.h>
#include <string>
namespace _Controllers
{
    class DropperTriggerPageController : public ControllerBase<DropperTriggerPageController>
    {
      public:
        void onSimulateTriggerSended(const QVariant &);
        void onDropperVideoRecordSended(const QVariant &);

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::SimulateTrigger, DropperTriggerPageController::onSimulateTriggerSended);
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::DropperVideoRecord, DropperTriggerPageController::onDropperVideoRecordSended);

        TASK_LIST_END

      private:
    };
} // namespace _Controllers