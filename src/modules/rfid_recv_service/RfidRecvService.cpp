#include "RfidRecvService.h"
#include "kits/common/system_config/SystemConfig.h"
using namespace _Kits;
_Modules::RfidRecvService::RfidRecvService()
{
}

_Modules::RfidRecvService::~RfidRecvService() noexcept
{
}

bool _Modules::RfidRecvService::init(const YAML::Node &config)
{
    if (config.IsDefined() == false)
    {
        qWarning() << "Invalid configuration node.";
        return false;
    }

    if (config["class_type"])
    {
        auto className = config["class_type"].as<std::string>();
    }

    auto config_path = config["config_path"].as<std::string>();
    if (config_path.empty())
    {
        qWarning() << "No configuration path found.";
        return false;
    }
    // 加载配置文件
    auto &sysConfig = _Kits::SystemConfig::instance();
    auto loadFilePath = sysConfig.configVersionPath() + config_path;
    YAML::Node config_node = _Kits::SystemConfig::instance().loadYamlFile(loadFilePath);

    if (config_node["rfidconfig"])
    {
        for (const auto &rfid_config : config_node["rfidconfig"])
        {
            try
            {
                // 检查配置项是否存在
                if (!rfid_config["tcpip"] || !rfid_config["tcpport"] || !rfid_config["rfid_recv_type"])
                {
                    qWarning() << "Invalid RFID configuration - missing required fields";
                    continue;
                }

                // 获取配置值
                std::string recv_type = rfid_config["rfid_recv_type"].as<std::string>();
                mb_isSave_natural = rfid_config["saveNaturalData"].as<bool>();
#if defined(USE_KIT_RFID_VFR61M)
                if (recv_type == "rfid_vfr61m")
                {
                    m_pRfidBase = std::make_unique<RfidVFR61M>();

                    if (m_pRfidBase && m_pRfidBase->Init(rfid_config))
                    {
                        // 先建立信号槽连接再发射信号
                        connect(m_pRfidBase.get(), &RfidBase::sendOutData, this, &RfidRecvService::sendRfidData);
                        connect(m_pRfidBase.get(), &RfidBase::sendRfidBackCheckData, this, &RfidRecvService::sendCheckBackData);
                        connect(this, &RfidRecvService::isSaveNaturalData, m_pRfidBase.get(), &RfidBase::onisSaveNaturalData);
                        connect(m_pRfidBase.get(), &RfidBase::sendRfidNaturalData, this, &RfidRecvService::sendNaturalData);

                        if (m_pRfidBase->Start())
                        {
                            emit isSaveNaturalData(mb_isSave_natural);
                            return true;
                        }
                        qWarning() << "Failed to start RFID device";
                    }
                    else
                    {
                        qWarning() << "Failed to initialize RFID device";
                    }
                }
#elif defined(USE_KIT_RFID_ANOTHER_TCP)
                m_pRfidBase = std::make_unique<RfidTcp>();

#endif
            }
            catch (const YAML::Exception &e)
            {
                qWarning() << "YAML parsing error:" << e.what();
                continue;
            }
            catch (const std::exception &e)
            {
                qWarning() << "Exception during RFID configuration:" << e.what();
                continue;
            }
        }
    }
    else
    {
        qWarning() << "No 'rfidconfig' section found in configuration file";
    }
    return false;
}

bool _Modules::RfidRecvService::start()
{
    emit isSaveNaturalData(mb_isSave_natural);
    return true;
}

bool _Modules::RfidRecvService::stop()
{
    if (m_pRfidBase->Stop())
    {
        return true;
    }
    return false;
}
