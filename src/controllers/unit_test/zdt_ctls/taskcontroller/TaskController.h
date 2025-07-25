#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include <vector>
#include <json/json.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include "tis_global/Struct.h"
namespace _Controllers
{
    class TaskController final : public ControllerBase<TaskController>
    {
      public:
        TaskController() = default;
        ~TaskController() = default;
        void notifyTaskToModules(TIS_Info::TaskInfo);
        void notifyTimeToModules(const QVariant &);
        void sendCSVToQml(QVariant &);
        void sendLineNameToQml(const QVariant &);
        void sendLineDataToQml(const Json::Value); // 线路数据发送
        void sendTaskNameToQml(const QString &);
        void recvStationNameByQml(const QString);     // QML发送站点名称，获取站点数据请求
        void sendStationDataToQml(const Json::Value); // 站点数据发送

        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::TaskService::notifyTaskData, TaskController::notifyTaskToModules);
        ASYNC_TASK_ADD(TIS_Info::TaskService::notifyUniqueTime, TaskController::notifyTimeToModules);

        ASYNC_TASK_ADD(TIS_Info::TaskService::sendCSVData, TaskController::sendCSVToQml);
        SYNC_TASK_ADD(TIS_Info::TaskService::sendLineData, TaskController::sendLineDataToQml);
        ASYNC_TASK_ADD(TIS_Info::TaskService::sendLineName, TaskController::sendLineNameToQml);
        ASYNC_TASK_ADD(TIS_Info::TaskService::sendTaskNameToQml, TaskController::sendTaskNameToQml);

        TASK_LIST_END
    };
} // namespace _Controllers
