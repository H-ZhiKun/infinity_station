#pragma once
#include "kits/common/hardware_resources/base/HardwareBase.h"
#include "kits/common/module_base/ModuleBase.h"
#include "tis_global/EnumClass.h"
#include "tis_global/Function.h"
#include <atomic>
#include <thread>

namespace _Modules
{

    class HardwareManager : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(HardwareManager)
        TIS_CONNECT(TIS_Info::HardwareManager::resourceInfo, TIS_Info::QmlPrivateEngine::callFromCpp)

      public:
        explicit HardwareManager(QObject *parent = nullptr);
        virtual ~HardwareManager() noexcept;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void resourceInfo(TIS_Info::QmlCommunication::ForQmlSignals ForQmlSignals, const QVariant &);     // 信号用于传输设备状态信息
        void logicalDriveInfo(TIS_Info::QmlCommunication::ForQmlSignals ForQmlSignals, const QVariant &); // 信号用于传输硬盘占用情况

      private:
        void collectFunction();
        std::unique_ptr<_Kits::HardwareBase> m_ptrHardware{nullptr};
        std::thread m_thCollect; // 信息采集线程
        std::atomic_bool m_bHolder = true;
    };

} // namespace _Modules
