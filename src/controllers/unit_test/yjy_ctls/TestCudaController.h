#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include "tis_global/Function.h"
#include <qvariant.h>
namespace _Controllers
{
    class TestCudaController : public ControllerBase<TestCudaController>
    {

    public:
    explicit TestCudaController();
    ~TestCudaController() noexcept;
     
    void doTest(const QVariant &);
    TASK_LIST_BEGIN
    // ASYNC_TASK_ADD(TIS_Info::DeviceManager::notifyDiskInfo, DeviceInfoController::doDiskInfo);
    TASK_LIST_END
      private:
        int m_index = 0;
    };
} // namespace _Controllers