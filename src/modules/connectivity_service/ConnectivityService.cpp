#include "modules/connectivity_service/ConnectivityService.h"
#include "kits/common/log/CRossLogger.h"
#include "kits/common/system_config/SystemConfig.h"

#if defined(USE_KIT_CONNECTIVITY_6051NET)
#include "kits/thrid_devices/connectivity_kits/6051net/ConnectivityManager6051.h"
#endif

#if defined(USE_KIT_CONNECTIVITY_7404NET)
#include "kits/thrid_devices/connectivity_kits/7404net/ConnectivityManager7404.h"
#endif

using namespace _Kits;
_Modules::ConnectivityService::ConnectivityService()
{
}

_Modules::ConnectivityService::~ConnectivityService()
{
}

bool _Modules::ConnectivityService::init(const YAML::Node &config)
{
    auto className = config["class_type"].as<std::string>();

    if (className.empty())
    {
        return false;
    }

    // 加载配置文件
    auto &sysConfig = _Kits::SystemConfig::instance();
    auto loadFilePath = sysConfig.configVersionPath() + config["config_path"].as<std::string>();
    YAML::Node con_config = _Kits::SystemConfig::instance().loadYamlFile(loadFilePath);

#if defined(USE_KIT_CONNECTIVITY_6051NET)
    mvec_Connect.push_back(std::make_unique<_Kits::ConnectivityManager6051>());
    if (!con_config["connect_info"]["6051net"].IsDefined())
    {
        LogInfo("6051net config is not defined.");
    }
    else
    {
        if (!mvec_Connect.back()->init(con_config["connect_info"]["6051net"]))
        {
            return false;
        }
        // mvec_Connect.back()->start();
    }

#endif

#if defined(USE_KIT_CONNECTIVITY_7404NET)
    mvec_Connect.push_back(std::make_unique<_Kits::ConnectivityManager7404>());
    if (!con_config["connect_info"]["7404net"].IsDefined())
    {
        LogInfo("7404net config is not defined.");
    }
    else
    {
        if (!mvec_Connect.back()->init(con_config["connect_info"]["7404net"]))
        {
            return false;
        }
        // mvec_Connect.back()->start();
    }
#endif

    _mb_isSave_natural = con_config["saveNaturalData"].as<bool>(false);
    _mstr_root_file_path = QString::fromStdString(con_config["saveNaturalDataPath"].as<std::string>(""));

    return true;
}
bool _Modules::ConnectivityService::start()
{
    for (auto &connect : mvec_Connect)
    {
        QObject::connect(connect.get(), &ConnectivityManagerBase::sendOutSpeedDataTotal, this, &ConnectivityService::onRecvSpeedData);
        QObject::connect(this, &ConnectivityService::JiHeTrigger, connect.get(), &ConnectivityManagerBase::onRecvYYJiheTrigger);
        QObject::connect(this, &ConnectivityService::isSaveNaturalData, connect.get(), &ConnectivityManagerBase::onisSaveNaturalData);
        QObject::connect(connect.get(), &ConnectivityManagerBase::sendNaturalData, this, &ConnectivityService::sendNaturalData);
        emit initSaveNaturalDataCtrl(connect->getName());
        connect->start();
    }

    emit isSaveNaturalData(_mb_isSave_natural);
    return true;
}
bool _Modules::ConnectivityService::stop()
{
    mvec_Connect.clear();

    return false;
}

void _Modules::ConnectivityService::onRecvSpeedData(TIS_Info::SpeedData speedData)
{
    emit speedOutput(std::move(speedData));
}