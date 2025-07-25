#include "DropperTriggerPageController.h"
#include "kits/common/log/CRossLogger.h"
#include "service/AppFramework.h"

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;
#include <qvariant.h>

namespace _Controllers
{
    void DropperTriggerPageController::onSimulateTriggerSended(const QVariant &data)
    {
        // 触发吊弦触发service中的模拟触发
        App().invokeModuleAsync(TIS_Info::TaskService::recvMBLKData);
    }

} // namespace _Controllers
// namespace _Controllers