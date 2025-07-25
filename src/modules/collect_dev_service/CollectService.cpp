#include "CollectService.h"
#include "kits/common/log/CRossLogger.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <functional>
#include <json/value.h>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qobject.h>
#include <qvariant.h>

// using namespace _Modules;
using namespace _Kits;
namespace _Modules
{

    CollectService::CollectService(QObject *parent)
    {
    }
    CollectService::~CollectService() noexcept
    {
        release();
    }

    bool CollectService::initYAML(const YAML::Node &config)
    {
        auto str_config_path = config["config_path"].as<std::string>();

        if (str_config_path.empty())
        {
            return false;
        }

        m_config = YAML::LoadFile(std::filesystem::current_path().string() + str_config_path);
        return true;
    }
    bool CollectService::initDevice()
    {
        if (m_config.Type() == YAML::NodeType::Undefined)
        {
            return false;
        }

        auto temperature = m_config["temperature_info"]["type"].as<std::string>();
        return true;
    }
    bool CollectService::init(const YAML::Node &config)
    {
        bool ret = true;
        ret &= initYAML(config);
        ret &= initDevice();

        // 开始运行
        if (ret)
        {
            m_runThreadFlag.store(true);
            m_runThread = std::thread(&CollectService::run, this);
            return true;
        }
        return false;
    }
    bool CollectService::start()
    {
        return true;
    }
    bool CollectService::stop()
    {
        release();
        return true;
    }

    void CollectService::release()
    {
        m_runThreadFlag.store(false);
        if (m_runThread.joinable())
        {
            m_runThread.join();
        }
    }
    void CollectService::run()
    {
        while (m_runThreadFlag)
        {
            // todo
            std::this_thread::sleep_for(std::chrono::milliseconds(m_sendRate));
        }
    }

} // namespace _Modules
