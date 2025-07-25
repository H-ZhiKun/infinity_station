#include "LocationController.h"
#include "kits/common/log/CRossLogger.h"
#include "service/AppFramework.h"
#include "tis_global/EnumClass.h"
#include <qvariant.h>
#include "tis_global/Field.h"

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;

void LocationController::sendPositToPage(const QVariant &data)
{
    App().invokeModuleAsync(TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::egvlocation_page, data);
}

void LocationController::sendPositToXJ(const QVariant &data)
{
    App().invokeModuleAsync(TIS_Info::CameraUser::SubtitleChange, data);
}

void LocationController::doDisplay(const QVariant &data)
{
    QVariantMap PositionData = data.toMap();
    QString stationName = PositionData.value("stationName", QString()).toString();
    QString poleName = PositionData.value("poleName", QString()).toString();
    double dis = PositionData.value("kiloMeter", -1.0f).toDouble();
    int location_type = data.toMap().value("positionType", 0).toInt();
    _Kits::LogInfo("stationName:{},poleName:{},dis:{},locationType:{}",
                   stationName.toLocal8Bit().toStdString(),
                   poleName.toStdString(),
                   dis,
                   location_type);
}

void LocationController::sendPositTypeToPage(int positionType)
{
    // QVariantMap data;
    // data["positionType"] = positionType;
    // App().invokeModuleAsync(TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::jihedata_recv, data);
}