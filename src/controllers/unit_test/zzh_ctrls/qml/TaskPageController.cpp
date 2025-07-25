#include "TaskPageController.h"
#include "kits/common/log/CRossLogger.h"
#include "service/AppFramework.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;
#include <qvariant.h>

namespace _Controllers
{
    void TaskPageController::onTaskSended(const QVariant &task_data)
    {
        if (task_data.isNull())
        {
            LogError("Invalid task_data!");
            return;
        }
        App().invokeModuleAsync(TIS_Info::TaskService::recvMBLKData, task_data);
    }

    void TaskPageController::onLineNameSended(const QVariant &line_name)
    {
        if (line_name.isNull())
        {
            LogError("Invalid line_name!");
            return;
        }
        App().invokeModuleAsync(TIS_Info::TaskService::OnrecvTaskName, Q_ARG(QString, line_name.toString()));
    }

    void TaskPageController::onLineBtnChooseSended(const QVariant &Choose_sig)
    {

        App().invokeModuleAsync(TIS_Info::TaskService::onRecvTaskChooseSig, Choose_sig);
    }
} // namespace _Controllers
// namespace _Controllers