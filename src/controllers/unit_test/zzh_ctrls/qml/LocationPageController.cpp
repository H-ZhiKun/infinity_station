#include "LocationPageController.h"
#include "kits/common/log/CRossLogger.h"
#include "service/AppFramework.h"

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;
#include <qvariant.h>

namespace _Controllers
{
    // 上下杆号校准
    void LocationPageController::onULPoleCalibrationSended(const QVariant &data)
    {
        // 发送组合后的数据到其他模块
        App().invokeModuleAsync(TIS_Info::LocationService::recvHandData, data);
    }
    // 站区杆号校准
    void LocationPageController::onPoleCalibrationSended(const QVariant &data)
    {
        // 发送组合后的数据到其他模块
        App().invokeModuleAsync(TIS_Info::LocationService::recvHandData, data);
    }

} // namespace _Controllers
// namespace _Controllers