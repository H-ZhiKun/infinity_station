#include "RemoteAccess.h"
#include "kits/common/system_config/SystemConfig.h"
#include <memory>
#include "kits/common/invoke/InvokeBase.h"
#include "tis_global/Struct.h"

namespace _Modules
{
    using namespace _Kits;
    RemoteAccess::RemoteAccess(QObject *parent) : ModuleBase(parent)
    {
    }

    RemoteAccess::~RemoteAccess() noexcept
    {
        if (m_publisher)
            m_publisher->stop();
        if (m_subscriber)
            m_subscriber->stop();
    }

    bool RemoteAccess::init(const YAML::Node &config)
    {
        auto config_path = config["config_path"].as<std::string>();
        // 加载配置文件
        auto &sysConfig = _Kits::SystemConfig::instance();
        auto loadFilePath = sysConfig.configVersionPath() + config_path;
        m_config = _Kits::SystemConfig::instance().loadYamlFile(loadFilePath);
        return true;
    }
    bool RemoteAccess::start()
    {
        if (m_config["publisher"])
        {
            const auto &detail = m_config["publisher"];
            m_publisher = std::make_unique<_Kits::RemotePublisher>();
            std::string protocol = detail["protocol"].as<std::string>();
            std::string ip = detail["ip"].as<std::string>();
            std::string port = detail["port"].as<std::string>();
            int timeout = detail["send_timeout"].as<int>();
            int hwm = detail["send_hwm"].as<int>();
            m_publisher->initialize(protocol, ip, port, timeout, hwm);
        }
        if (m_config["subscriber"])
        {
            const auto &detail = m_config["subscriber"];
            m_subscriber = std::make_unique<_Kits::RemoteSubscriber>();
            std::string protocol = detail["protocol"].as<std::string>();
            std::string ip = detail["ip"].as<std::string>();
            std::string port = detail["port"].as<std::string>();
            int timeout = detail["recv_timeout"].as<int>();
            int hwm = detail["recv_hwm"].as<int>();
            m_subscriber->initialize(protocol, ip, port, timeout, hwm);
        }

        for (const auto &[key, invokeable] : m_mapTopicMethod)
        {
            m_subscriber->registerCallback(key, [callback = invokeable](const TIS_Info::RemoteMessage &message) {
                callback->getPrivateSlot().invoke(callback.get(), message);
            });
        }
        return true;
    }

    bool RemoteAccess::stop()
    {
        m_mapTopicMethod.clear();
        return true;
    }

    bool RemoteAccess::customization(const std::unordered_map<_Kits::RegisterKey, std::list<std::any>> &lvTasks)
    {
        for (auto &[key, tasks] : lvTasks)
        {
            std::string topic = std::get<0>(key);
            for (auto &task : tasks)
            {
                m_mapTopicMethod[topic] = std::any_cast<std::shared_ptr<_Kits::InvokeBase>>(task);
            }
        }
        return true;
    }

    void RemoteAccess::publish(const TIS_Info::RemoteMessage &message)
    {
        if (m_publisher)
        {
            m_publisher->publishAsync(message);
        }
    }
} // namespace _Modules