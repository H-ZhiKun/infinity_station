#include "LiveNvrService.h"

namespace _Modules
{
    using namespace _Kits;
LiveNvrService::LiveNvrService()
{
}

LiveNvrService::~LiveNvrService() noexcept
{
}

bool LiveNvrService::init(const YAML::Node &config)
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

    YAML::Node config_node;
    try
    {
        config_node = YAML::LoadFile(std::filesystem::current_path().string() + config_path);
    }
    catch (const YAML::Exception &e)
    {
        qWarning() << "Failed to load configuration file:" << e.what();
        return false;
    }

    if (config_node["live_nvr_config"])
    {
        for (const auto &LiveNvr_config : config_node["live_nvr_config"])
        {
            try
            {
                // 检查配置项是否存在
                if (!LiveNvr_config["live_nvr_ip"] || !LiveNvr_config["live_nvr_port"] || !LiveNvr_config["live_nvr_user"] ||
                    !LiveNvr_config["live_nvr_password"])
                {
                    qWarning() << "Invalid RFID configuration - missing required fields";
                    continue;
                }

                // 获取配置值
                TIS_Info::LogInfo LiveNvr_info;
                LiveNvr_info.mstr_ipAddr = QString::fromStdString(LiveNvr_config["live_nvr_ip"].as<std::string>());
                LiveNvr_info.mus_port = LiveNvr_config["live_nvr_port"].as<unsigned short>();
                LiveNvr_info.mstr_userName = QString::fromStdString(LiveNvr_config["live_nvr_user"].as<std::string>());
                LiveNvr_info.mstr_password = QString::fromStdString(LiveNvr_config["live_nvr_password"].as<std::string>());

                m_pLiveNvr = std::make_unique<LiveNvr>();

                if (m_pLiveNvr && m_pLiveNvr->init(LiveNvr_info))
                {
                    connect(m_pLiveNvr.get(), &LiveNvr::channelInfoReceived, this, &LiveNvrService::sendChanel);//发送通道号
                    connect(m_pLiveNvr.get(), &LiveNvr::sendDownLoadVecInfo, this, &LiveNvrService::sendNvrDataInfoToController);
                    //connect(m_pLiveNvr.get(), &LiveNvr::fileDownloadFinished, this, &LiveNvrService::fileDownloadFinished);
                    qWarning() << "Failed to start live_nvr device";
                }
                else
                {
                    qWarning() << "Failed to initialize live_nvr device";
                }
            }
            catch (const YAML::Exception &e)
            {
                qWarning() << "YAML parsing error:" << e.what();
                continue;
            }
            catch (const std::exception &e)
            {
                qWarning() << "Exception during live_nvr configuration:" << e.what();
                continue;
            }
        }
    }
    else
    {
        qWarning() << "No 'live_nvr_config' section found in configuration file";
    }
    return false;
}

bool LiveNvrService::start()
{

    return true;
}

bool LiveNvrService::stop()
{
    return false;
}


 void LiveNvrService::onEndRecord(const QVariant & data)
 {
    emit sendEndRecordState(data);
 }
}

