#include "RadarController.h"
#include "service/AppFramework.h"
#include "tis_global/Field.h"
#include <qimage.h>
#include <qvariant.h>
#include <sys/stat.h>

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;
void RadarController::sendState(const QVariant &data)
{

    App().invokeModuleAsync(
        TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::radar_device_state, QVariant::fromValue(data));
}

void RadarController::sendData(std::vector<std::vector<TIS_Info::DeviceSingleData>> data)
{
    QVariantList outerList;
    for (const auto &inner : data)
    {
        QVariantList innerList;
        for (const auto &s : inner)
        {
            QVariantMap map;
            map["x"] = s.mf_x;
            map["y"] = s.mf_y;
            // ...其它字段...
            innerList << map;
        }
        outerList << innerList;
    }

    App().invokeModuleAsync(
        TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::radar_page, QVariant::fromValue(outerList));
}

void RadarController::sendOverInfo(QString data)
{
    App().invokeModuleAsync(
        TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::radar_overrun_info, QVariant::fromValue(data));
}

void RadarController::sendOverConfig(const QVariant &data)
{
    App().invokeModuleAsync(TIS_Info::QmlPrivateEngine::callFromCpp,
                            TIS_Info::QmlCommunication::ForQmlSignals::radar_overrun_config,
                            QVariant::fromValue(data));
}
