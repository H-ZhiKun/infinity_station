#pragma once
#include "kits/common/controller_base/ControllerBase.h"
namespace _Controllers
{
    class RfidController : public ControllerBase<RfidController>
    {
      public:
        void sendRfid(const QString &);
        void sendRfidtoLocation(const QString &);
        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::RfidRecvService::sendRfidData, RfidController::sendRfid);
        ASYNC_TASK_ADD(TIS_Info::RfidRecvService::sendRfidData, RfidController::sendRfidtoLocation);
        TASK_LIST_END
    };
} // namespace _Controllers