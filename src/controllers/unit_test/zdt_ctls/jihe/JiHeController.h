#pragma once
#include "kits/common/controller_base/ControllerBase.h"
namespace _Controllers
{
    class JiHeController : public ControllerBase<JiHeController>
    {
      public:
        void sendJiHeInfoToQml(const QVariant &);
        void sendJiHeTOJiHeTriggerService(const QVariant &);
        void doDisplay(const QVariant &);
        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::JiHeService::sendJiHeData, JiHeController::sendJiHeInfoToQml);
        ASYNC_TASK_ADD(TIS_Info::JiHeService::sendJiHeData, JiHeController::sendJiHeTOJiHeTriggerService);
        ASYNC_TASK_ADD(TIS_Info::JiHeService::sendJiHeData, JiHeController::doDisplay);
        TASK_LIST_END
    };
} // namespace _Controllers
