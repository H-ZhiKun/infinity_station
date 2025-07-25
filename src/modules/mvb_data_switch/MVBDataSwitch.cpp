#include "MVBDataSwitch.h"
#include "kits/common/log/CRossLogger.h"

namespace _Modules
{
    using namespace _Kits;

    MVBDataSwitch::MVBDataSwitch(QObject *parent) : _Kits::ModuleBase(parent)
    {
    }
    MVBDataSwitch::~MVBDataSwitch() noexcept
    {
    }

    bool MVBDataSwitch::init(const YAML::Node &config)
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

    bool MVBDataSwitch::start()
    {
        TIS_Info::MvbAmifSwitch amifSwitch;
        TIS_Info::MvbAsatSwitch asatSwitch;
        for (const auto item : m_details["amif_send_config"])
        {
            amifSwitch.temperature = static_cast<bool>(item["temperature"].as<int>());
            amifSwitch.catOff = static_cast<bool>(item["cat_off"].as<int>());
            amifSwitch.catHei = static_cast<bool>(item["cat_hei"].as<int>());
            amifSwitch.arc = static_cast<bool>(item["arc"].as<int>());
            amifSwitch.panStructure = static_cast<bool>(item["pan_structure"].as<int>());
            amifSwitch.bowSystem = static_cast<bool>(item["bow_system"].as<int>());
            amifSwitch.abrval = static_cast<bool>(item["abrval"].as<int>());
        }
        for (const auto item : m_details["asat_send_config"])
        {
            asatSwitch.geoparMode = static_cast<bool>(item["geopar_mode"].as<int>());
            asatSwitch.temperatureMode = static_cast<bool>(item["temperature_mode"].as<int>());
            asatSwitch.arcMode = static_cast<bool>(item["arc_mode"].as<int>());
            asatSwitch.pantoMode = static_cast<bool>(item["panto_mode"].as<int>());
            asatSwitch.sysCommunicationMode = static_cast<bool>(item["system_communication_mode"].as<int>());
            asatSwitch.abrvalMode = static_cast<bool>(item["abrval_mode"].as<int>());
        }

        emit switchConfigData(amifSwitch, asatSwitch);
        return true;
    }

    bool MVBDataSwitch::stop()
    {
        return true;
    }
} // namespace _Modules