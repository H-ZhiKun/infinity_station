#include "TaskController.h"
#include "kits/common/log/CRossLogger.h"
#include "service/AppFramework.h"
#include "tis_global/EnumClass.h"
#include "tis_global/Field.h"
#include "tis_global/Function.h"
#include <qglobal.h>
#include <qlogging.h>

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;
void TaskController::notifyTaskToModules(TIS_Info::TaskInfo data)
{
    App().notify(_Service::NotifyType::Task, data);
}
void TaskController::notifyTimeToModules(const QVariant &data)
{
    //  _Service::App().notify(_Service::NotifyType::UniqueTime, data);
}

void TaskController::sendCSVToQml(QVariant &data)
{
    // 获取线路数据
    QVariantMap CSVdata = data.toMap();

    // 通过QmlPrivateEngine发送数据
    App().invokeModuleAsync(
        TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::linedata_recv, QVariant::fromValue(CSVdata));
}

void TaskController::sendLineNameToQml(const QVariant &lineNames)
{
    // 通过QmlPrivateEngine发送数据
    App().invokeModuleAsync(TIS_Info::QmlPrivateEngine::callFromCpp,
                            TIS_Info::QmlCommunication::ForQmlSignals::lineNameList_recv,
                            QVariant::fromValue(lineNames));
}

void TaskController::sendLineDataToQml(const Json::Value lineData)
{

    // 将 Json::Value 转换为字符串，然后解析为 QJsonDocument
    auto temp = lineData.toStyledString();
    QString jsonString = QString::fromStdString(lineData.toStyledString());
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 通过QmlPrivateEngine发送数据
    App().invokeModuleAsync(
        TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::linedata_recv, doc.toVariant());
}

void _Controllers::TaskController::recvStationNameByQml(const QString stationName)
{
}

void _Controllers::TaskController::sendStationDataToQml(const Json::Value stationData)
{
    // 将 Json::Value 转换为字符串，然后解析为 QJsonDocument
    auto temp = stationData.toStyledString();
    QString jsonString = QString::fromStdString(stationData.toStyledString());
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 通过QmlPrivateEngine发送数据
    App().invokeModuleAsync(
        TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::linedata_recv, doc.toVariant());
}

void TaskController::sendTaskNameToQml(const QString &taskname)
{

    // 通过QmlPrivateEngine发送数据
    App().invokeModuleAsync(
        TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::taskname_recv, QVariant::fromValue(taskname));
}
