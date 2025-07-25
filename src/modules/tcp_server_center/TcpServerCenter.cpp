#include "TcpServerCenter.h"
#include "kits/common/log/CRossLogger.h"
#include <memory>
#include <qobject.h>

namespace _Modules
{
    using namespace _Kits;

    TcpServerCenter::TcpServerCenter(QObject *parent) : ModuleBase(parent)
    {
    }

    TcpServerCenter::~TcpServerCenter() noexcept
    {
    }

    bool TcpServerCenter::init(const YAML::Node &config)
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
        return true;
    }

    bool TcpServerCenter::start()
    {
        m_server = std::make_unique<_Kits::TcpServer>(this);
        if (m_server->start(m_config))
        {
            QObject::connect(m_server.get(), &_Kits::TcpServer::newConnection, this, &TcpServerCenter::newClient);
            QObject::connect(m_server.get(), &_Kits::TcpServer::clientDisconnected, this, &TcpServerCenter::clientDisconnect);
        }
        return true;
    }
    bool TcpServerCenter::stop()
    {
        return true;
    }

} // namespace _Modules
