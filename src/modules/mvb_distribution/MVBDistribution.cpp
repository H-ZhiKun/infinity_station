#include "MVBDistribution.h"
#include "kits/thrid_devices/duagon/duagon_card/DuagonCardMVB.h"
#include "kits/common/log/CRossLogger.h"
#include "tis_global/Struct.h"
#include <chrono>
#include <filesystem>
#include <memory>
#include <yaml-cpp/node/parse.h>

namespace _Modules
{
    using namespace _Kits;
    MVBDistribution::MVBDistribution(QObject *parent) : _Kits::ModuleBase(parent)
    {
    }
    MVBDistribution::~MVBDistribution() noexcept
    {
    }
    bool MVBDistribution::init(const YAML::Node &config)
    {
        if (!config["config_path"])
        {
            return false;
        }
        std::string fullPath = std::filesystem::current_path().string() + config["config_path"].as<std::string>();
        try
        {
            m_details = YAML::LoadFile(fullPath);
        }
        catch (const YAML::BadFile &e)
        {
            LogError("load file path error {}", fullPath);
            return false;
        }
        return true;
    }
    bool MVBDistribution::start()
    {
        if (m_details["mvb_type"].as<std::string>() == "duagon")
        {
            m_ptrDuagonCard = std::make_unique<DuagonCardMVB>();
            std::vector<TIS_Info::DuagonCardData> userDatas;
            for (const auto item : m_details["read_config"])
            {
                TIS_Info::DuagonCardData data;
                data.m_bRead = true;
                data.m_portId = item["port"].as<int>();
                data.m_portSize = item["size"].as<int>();
                data.m_interval = std::chrono::milliseconds(item["interval"].as<int>());
                data.m_portData.resize(data.m_portSize);
                userDatas.push_back(std::move(data));
            }
            for (const auto item : m_details["write_config"])
            {
                TIS_Info::DuagonCardData data;
                data.m_bRead = false;
                data.m_portId = item["port"].as<int>();
                data.m_portSize = item["size"].as<int>();
                data.m_interval = std::chrono::milliseconds(item["interval"].as<int>());
                data.m_portData.resize(data.m_portSize);
                emit setConfigHeart(data); // 初始化controller中的端口id
                userDatas.push_back(std::move(data));
            }
            if (!m_ptrDuagonCard->init(userDatas))
            {
// 板卡初始化失败应该终止进程
#ifdef NDEBUG
                throw std::runtime_error("duagon mvb init error, restart process.");
#endif
            }
            m_ptrDuagonCard->setCallBack(
                [this](const TIS_Info::DuagonCardData &dgData) { emit duagonData(dgData.m_portId, dgData.m_portData); });
        }

        return true;
    }
    void MVBDistribution::dataToDuagonCard(const TIS_Info::DuagonCardData &data)
    {
        if (m_ptrDuagonCard)
        {
            m_ptrDuagonCard->writeData(data);
        }
    }
    bool MVBDistribution::stop()
    {
        return true;
    }
} // namespace _Modules