#include "kits/common/log/CRossLogger.h"
#include "kits/communication/udp/UdpClient.h"
#include "UdpCenter.h"
#include "kits/common/system_config/SystemConfig.h"
#include <memory>

namespace _Modules
{
    using namespace _Kits;
    UdpCenter::UdpCenter(QObject *parent) : ModuleBase(parent)
    {
    }

    UdpCenter::~UdpCenter() noexcept
    {
        m_storeClients.clear();
    }

    bool UdpCenter::init(const YAML::Node &config)
    {
        auto config_path = config["config_path"].as<std::string>();
        // 加载配置文件
        auto &sysConfig = _Kits::SystemConfig::instance();
        auto loadFilePath = sysConfig.configVersionPath() + config_path;
        m_config = _Kits::SystemConfig::instance().loadYamlFile(loadFilePath);
        return true;
    }
    bool UdpCenter::start()
    {
        for (const auto &item : m_config)
        {
            std::string client_title = item["client_title"].as<std::string>();
            auto client = std::make_shared<UdpClient>(this);
            if (client->start(item))
            {
                m_storeClients[client_title] = client;
            }
        }
        emit dispatchClient(m_storeClients);
        return true;
    }

    bool UdpCenter::stop()
    {
        return true;
    }
} // namespace _Modules