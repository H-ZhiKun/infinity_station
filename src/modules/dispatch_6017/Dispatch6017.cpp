#include "Dispatch6017.h"
#include "kits/common/log/CRossLogger.h"
#include "kits/common/system_config/SystemConfig.h"

using namespace _Modules;

_Modules::Dispatch6017::Dispatch6017() : ModuleBase()
{
}

_Modules::Dispatch6017::~Dispatch6017() noexcept
{
}

bool _Modules::Dispatch6017::init(const YAML::Node &config)
{
    std::vector<TIS_Info::_6017Info> vec_6017_info;

    try
    {

        auto config_path = config["config_path"].as<std::string>();
        auto &ins = _Kits::SystemConfig::instance();
        YAML::Node config_node = YAML::LoadFile(ins.configVersionPath() + config_path);

        for (const auto node : config_node["6017_info"])
        {
            TIS_Info::_6017Info info;

            info.mstr_index = QString::fromStdString(node["index"].as<std::string>());
            info.mstr_ip = QString::fromStdString(node["ip"].as<std::string>());
            info.mi_port = node["port"].as<int>();
            info.mui_interval = node["send_interval"].as<uint16_t>();
            info.mb_is_save_natural_data = node["save_natural_data"].as<bool>(false);

            for (const auto node_info : node["channel_link_info"])
            {
                auto channel = node_info["channel"].as<int>();
                info.mmap_Channel_info[QString::fromStdString(node_info["link_info"].as<std::string>())] = channel;
            }

            vec_6017_info.push_back(std::move(info));
        }
    }
    catch (const YAML::Exception &e)
    {
        _Kits::LogError("Dispatch6017::init error {}", e.what());

        return false;
    }

    _mvec_channelInfo = std::move(vec_6017_info);
    return true;
}

bool _Modules::Dispatch6017::start()
{
    emit sendConfigToCtrl(_mvec_channelInfo);
    return true;
}

bool _Modules::Dispatch6017::stop()
{
    return true;
}

void _Modules::Dispatch6017::getElectricDataFromCtrl(const double electric_data)
{
    emit sendElectriDataOut(electric_data);
}

void _Modules::Dispatch6017::getPressData0FromCtrl(const double press_data)
{
    emit sendPressData0Out(press_data);
}

void _Modules::Dispatch6017::getPressData1FromCtrl(const double press_data)
{
    emit sendPressData1Out(press_data);
}

void _Modules::Dispatch6017::getPressData2FromCtrl(const double press_data)
{
    emit sendPressData2Out(press_data);
}

void _Modules::Dispatch6017::getPressData3FromCtrl(const double press_data)
{
    emit sendPressData3Out(press_data);
}

void _Modules::Dispatch6017::getSpotData0xFromCtrl(const double press_data)
{
    emit sendSpotData0xOut(press_data);
}

void _Modules::Dispatch6017::getSpotData0yFromCtrl(const double press_data)
{
    emit sendSpotData0yOut(press_data);
}
