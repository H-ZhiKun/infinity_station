#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include "kits/common/controller_base/ControllerBase.h"
#include "tis_global/EnumClass.h"

namespace _Controllers
{
    class TestTaskController : public ControllerBase<TestTaskController>
    {
      public:
        void testDeviceInfo(TIS_Info::QmlCommunication::ForQmlSignals, const QVariant &);
        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::HardwareManager::resourceInfo, TestTaskController::testDeviceInfo);
        TASK_LIST_END
      private:
        int m_index = 0;
    };

    class TestQmlController : public ControllerBase<TestQmlController>
    {
      public:
        void testPageChanged(const QVariant &);
        void testMVBTest(const QVariant &);
        static void testTaskTest(const QVariant &);
        static void testTaskControllerTest(const QVariant &);

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::PageChange, TestQmlController::testPageChanged);
        TASK_LIST_END
    };
} // namespace _Controllers
