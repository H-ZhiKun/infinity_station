#include "RfidController.h"
#include "kits/common/log/CRossLogger.h"
#include "service/AppFramework.h"
#include "tis_global/EnumClass.h"
#include "tis_global/Field.h"
#include <qvariant.h>

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;
void RfidController::sendRfid(const QString &data)
{

    QVariantMap RfidData;
    RfidData["rfid"] = data; // 传递原始数据
    _Kits::LogInfo("rfid:{}", data.toStdString());

    // 发送rfid数据到QML
    App().invokeModuleAsync(
        TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::rfid_recv, QVariant::fromValue(RfidData));
}

void RfidController::sendRfidtoLocation(const QString &data)
{
    QVariantMap mapData;
    mapData["rfidId"] = data;

    QVariantMap mapData2;
    mapData2["data"] = mapData;
    //_Kits::LogInfo("[rfid] Sending RFID to LocationService: {}", data.toStdString());

    // bool success = App().invokeModuleAsync(TIS_Info::LocationService::recvLocationData, QVariant::fromValue(mapData2));
    bool success = App().invokeModuleAsync(TIS_Info::LocationService::recvRfidData, data);
    if (!success)
    {
        _Kits::LogError("[rfid] Failed to invoke recvLocationData in LocationService");
    }
}