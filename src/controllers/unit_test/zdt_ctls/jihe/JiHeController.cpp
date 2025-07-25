#include "JiHeController.h"
#include "kits/common/log/CRossLogger.h"
#include "service/AppFramework.h"
#include "tis_global/EnumClass.h"
#include "tis_global/Field.h"
#include <qvariant.h>

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;
void JiHeController::sendJiHeInfoToQml(const QVariant &data)
{
    App().invokeModuleAsync(TIS_Info::QmlPrivateEngine::callFromCpp, TIS_Info::QmlCommunication::ForQmlSignals::jihedata_recv, data);
}

void JiHeController::sendJiHeTOJiHeTriggerService(const QVariant &data)
{
    App().invokeModuleAsync(TIS_Info::JiHeTriggerService::onJiHeDataReceived, data);
}

void JiHeController::doDisplay(const QVariant &data)
{
    QVariantMap JiHeData = data.toMap();
    double zig0 = JiHeData.value("zig0", -1.0f).toDouble();
    double hei0 = JiHeData.value("hei0", -1.0f).toDouble();
    _Kits::LogInfo("[jihe] zig0:{},hei0:{}", zig0, hei0);
}
