#pragma once

#include "kits/common/controller_base/ControllerBase.h"
#include "tis_global/Function.h"
#include "tis_global/Struct.h"
namespace _Controllers
{

    class SpeedController : public ControllerBase<SpeedController>
    {
      public:
        void sendSpeedToModule(const TIS_Info::SpeedData &);

        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::ConnectivityService::speedOutput, SpeedController::sendSpeedToModule);
        TASK_LIST_END
      private:
    };

} // namespace _Controllers