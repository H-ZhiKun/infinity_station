#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include <qvariant.h>
#include "kits/common/read_csv/ReadCSVFile.h"
#include <string>
namespace _Controllers
{
    class TaskPageController : public ControllerBase<TaskPageController>
    {
      public:
        void onTaskSended(const QVariant &task_data);
        void onLineNameSended(const QVariant &);
        void onLineBtnChooseSended(const QVariant &);

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::TaskSend, TaskPageController::onTaskSended);
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::LineNameSend, TaskPageController::onLineNameSended);

        QML_ADD(TIS_Info::QmlCommunication::QmlActions::LineBtnChoose, TaskPageController::onLineBtnChooseSended);
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::LineBtnChoose, TaskPageController::onLineBtnChooseSended);

        TASK_LIST_END

      private:
    };
} // namespace _Controllers
