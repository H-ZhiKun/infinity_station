#include "arcuv_service.h"
#include <filesystem>
#include "kits/common/system_config/SystemConfig.h"

namespace _Modules
{

    using namespace _Kits;

    ArcUVService::ArcUVService()
    {
        dwcarcuv = std::make_unique<DWCArcUV>();
    }

    bool ArcUVService::init(const YAML::Node &config)
    {
        if (config["config_path"].as<std::string>().empty())
        {
            LogError("No configuration path found.");
            return false;
        }
        std::string config_path = config["config_path"].as<std::string>();
        try
        {
            m_details = _Kits::SystemConfig::instance().loadYamlFile(_Kits::SystemConfig::instance().configVersionPath() + config_path);
        }
        catch (const YAML::BadFile &e)
        {
            LogError("load file path error {}", config_path);
            return false;
        }
        return true;
    }
    bool ArcUVService::start()
    {
        // 遍历配置的设备
        if (m_details["devices"])
        {
            for (const auto &device : m_details["devices"])
            {

                if (device["threshold"])
                {
                    dwcarcuv->setArcThreshold(device["threshold"].as<double>());
                }
                if (device["devIndex"])
                {
                    dwcarcuv->setDeviceIndex(device["devIndex"].as<std::string>());
                }
                if(device["cameraname"])
                {
                    dwcarcuv->setCameraName(device["cameraname"].as<std::string>());
                }
                connect(this, &ArcUVService::sendArcVideoPath, dwcarcuv.get(), &DWCArcUV::OnVideoPathRecv);
                auto ret = connect(this, &ArcUVService::sendDWCVoltage, dwcarcuv.get(), &DWCArcUV::OnDWCVoltageReceived);
                connect(dwcarcuv.get(), &DWCArcUV::sendArcStatistics, this, &ArcUVService::sendArcStatisticsToController);
            }
        }
        return true;
    }
    bool ArcUVService::stop()
    {
        return true;
    }

    void ArcUVService::OnVoltageReceived(float voltage)
    {
        // 创建要发送到控制器的数据
        QVariantMap data;
        data["voltage"] = voltage;
        data["timestamp"] = QDateTime::currentDateTime();
        emit voltageToController(data); // 用于发送电压值到前端显示
        emit sendDWCVoltage(voltage);   // 用于发送到kit进行处理
    }

    void ArcUVService::OnVideoPathReceived(const TIS_Info::arcData &data)
    {
        emit sendArcVideoPath(data);
    }

} // namespace _Modules