#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include "tis_global/EnumClass.h"
#include "tis_global/Function.h"
namespace _Controllers
{
    class TestDatabase : public ControllerBase<TestDatabase>
    {
      public:
        void testCURD(TIS_Info::QmlCommunication::ForQmlSignals, const QVariant &);
        void insert();
        void select();
        void testSqlite();
        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::HardwareManager::resourceInfo, TestDatabase::testCURD);
        TASK_LIST_END
    };
} // namespace _Controllers