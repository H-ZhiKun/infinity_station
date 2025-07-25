#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include <qvariant.h>
namespace _Controllers
{
    class MainPageController : public ControllerBase<MainPageController>
    {
      public:
        void onPageChanged(const QVariant &);
        void onTaskSended(const QVariant &);

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::PageChange, MainPageController::onPageChanged);
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::LocationRecv, MainPageController::onPageChanged);
        TASK_LIST_END
    };
} // namespace _Controllers
