#include "HardwareManager.h"
#include "tis_global/EnumClass.h"
#include <chrono>
#include <memory>
#include <qdebug.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <thread>

#ifdef _WIN32
#include "kits/common/hardware_resources/windows/HardwareWindows.h"
#endif

namespace _Modules
{
    using namespace _Kits;

    // 2 完成模块类型
    // 注册宏实现，如果将来要制作动态库时避免发生错误。
    HardwareManager::HardwareManager(QObject *parent) : ModuleBase(parent)
    {
    }

    HardwareManager::~HardwareManager() noexcept
    {
        m_bHolder = false;
        if (m_thCollect.joinable())
            m_thCollect.join();
    }

    bool HardwareManager::init(const YAML::Node &config)
    {
        Q_UNUSED(config)

#ifdef _WIN32
        m_ptrHardware = std::make_unique<HardwareWindows>();
#endif
        return true;
    }

    bool HardwareManager::start()
    {
        m_thCollect = std::thread(&HardwareManager::collectFunction, this);
        return true;
    }
    bool HardwareManager::stop()
    {

        return true;
    }
    void HardwareManager::collectFunction()
    {
        using namespace std::chrono;
        using namespace std::chrono_literals;

        constexpr auto hw_interval = 500ms;
        constexpr auto ld_interval = 30min;

        auto nextHW = steady_clock::now() + hw_interval;
        auto nextLD = steady_clock::now() + ld_interval;

        while (m_bHolder)
        {
            const auto now = steady_clock::now();

            if (now >= nextHW)
            {
                nextHW = now + hw_interval;
                emit resourceInfo(TIS_Info::QmlCommunication::ForQmlSignals::hardware_resource, m_ptrHardware->resourcesInfo());
            }

            if (now >= nextLD)
            {
                nextLD = now + ld_interval;
                emit logicalDriveInfo(TIS_Info::QmlCommunication::ForQmlSignals::logical_drive, m_ptrHardware->logicalDriveInfo());
            }

            const auto sleepUntil = std::min(nextHW, nextLD);
            if (const auto sleepDuration = sleepUntil - now; sleepDuration > 0ms)
            {
                std::this_thread::sleep_for(sleepDuration);
            }
        }
    }
} // namespace _Modules
