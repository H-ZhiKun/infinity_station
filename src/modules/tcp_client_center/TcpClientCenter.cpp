#include "TcpClientCenter.h"
#include "kits/common/log/CRossLogger.h"
#include "kits/communication/tcp_client/TcpClient.h"
#include <qtmetamacros.h>

namespace _Modules
{
    using namespace _Kits;

    TcpClientCenter::TcpClientCenter(QObject *parent) : ModuleBase(parent)
    {
    }

    TcpClientCenter::~TcpClientCenter() noexcept
    {
    }

    bool TcpClientCenter::init(const YAML::Node &config)
    {
        if (!config["config_path"])
        {
            return false;
        }
        std::string fullPath = std::filesystem::current_path().string() + config["config_path"].as<std::string>();
        try
        {
            m_config = YAML::LoadFile(fullPath);
        }
        catch (const YAML::BadFile &e)
        {
            LogError("load file path error {}", fullPath);
            return false;
        }
        for (const auto &item : m_config)
        {
            std::string title = item["client_title"].as<std::string>().c_str();
            QString ip = item["ip"].as<std::string>().c_str();
            uint16_t port = item["port"].as<uint16_t>();
            uint64_t reconnect_interval = item["reconnect_interval"].as<uint16_t>();
            auto client = std::make_shared<_Kits::TcpClient>(ip, port, reconnect_interval, this);
            client->start();
            m_storeClients[title] = client;
        }
        return true;
    }

    bool TcpClientCenter::start()
    {
        emit dispatchClient(m_storeClients);
        return true;
    }
    bool TcpClientCenter::stop()
    {
        return true;
    }

} // namespace _Modules
