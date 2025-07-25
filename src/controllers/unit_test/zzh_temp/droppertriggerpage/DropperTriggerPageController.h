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

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::simulateTrigger, DropperTriggerPageController::onSimulateTriggerSended);

        TASK_LIST_END

      private:
    };
} // namespace _Controllers