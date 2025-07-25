#include "HikManager.h"
#include "kits/camera/hik/factory/HikFactory.h"
#include "kits/camera/hik/camera_normal/HikCamera.h"
#include "kits/common/system_config/SystemConfig.h"
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;
using namespace _Modules;

HikManager::HikManager(QObject *parent) : m_locationCtrl_(std::make_unique<LocationCtrl>()), _mqstr_SavePath("NoTask")
{
}

HikManager::~HikManager() noexcept
{
}

bool HikManager::init(const YAML::Node &config)
{
#if defined(USE_KIT_CAMERA_HIK_NORMAL)
    HikFactory::RegisterCameraCreateFactory<HikCamera>(std::string(HikCamera::CameraType));
#endif

#if defined(USE_KIT_CAMERA_HIK_TEMPERATURE)
    HikFactory::RegisterCameraCreateFactory<HikCamera>("TEMPERATURE");
#endif

    // 加载配置文件
    auto &sysConfig = _Kits::SystemConfig::instance();
    auto loadFilePath = sysConfig.configVersionPath() + config["hik_path"].as<std::string>("camera_config/hik/camera_info.yaml");
    YAML::Node Hikconfig = _Kits::SystemConfig::instance().loadYamlFile(loadFilePath);

    TIS_Info::HikLogInfo cameraLogInfo;

    for (auto cam_config : Hikconfig["hik_info"])
    {
        cameraLogInfo.mstr_ipAddr = cam_config["ipaddr"].as<std::string>();
        cameraLogInfo.mus_port = cam_config["port"].as<unsigned short>();
        cameraLogInfo.mstr_userName = cam_config["username"].as<std::string>();
        cameraLogInfo.mstr_password = cam_config["password"].as<std::string>();
        cameraLogInfo.mi_channel = cam_config["channel"].as<int>();
        cameraLogInfo.mstr_deviceName = cam_config["camera_name"].as<std::string>();
        cameraLogInfo.mstr_type = cam_config["type"].as<std::string>();

        auto camera = HikFactory::CreateCameraByType(cameraLogInfo.mstr_type);
        if (camera)
        {
            m_cameraMap_[cameraLogInfo.mstr_deviceName] = std::move(camera);

            if (cameraLogInfo.mstr_type == HikCamera::CameraType)
            {
                mvec_CameraName_video_.push_back(cameraLogInfo.mstr_deviceName);
            }

            mvec_cameraInfo_.push_back(std::move(cameraLogInfo));
        }
        else
        {
            LogError("Camera type not supported:{}", cameraLogInfo.mstr_type);
            continue;
        }
    }

    if (config["nvr_info"])
    {
        TIS_Info::HikLogInfo nvrLogInfo;
        nvrLogInfo.mstr_ipAddr = {config["nvr_info"]["ipaddr"].as<std::string>()};
        nvrLogInfo.mus_port = {config["nvr_info"]["port"].as<unsigned short>()};
        nvrLogInfo.mstr_userName = {config["nvr_info"]["username"].as<std::string>()};
        nvrLogInfo.mstr_password = {config["nvr_info"]["password"].as<std::string>()};

        emit initHikNvrCtrl(nvrLogInfo);
    }

    return true;
}

bool HikManager::start()
{
    emit initVideoCtrl(mvec_CameraName_video_);

    for (auto &camerainfo : mvec_cameraInfo_)
    {
        if (camerainfo.mstr_type == HikCamera::CameraType)
        {
            connect(m_cameraMap_[camerainfo.mstr_deviceName].get(), &HikCamera::sendImage, this, &HikManager::sendImage);
            connect(this, &HikManager::sendSubTitle, m_cameraMap_[camerainfo.mstr_deviceName].get(), &HikBase::onWriteSubTitle);
        }

        m_cameraMap_[camerainfo.mstr_deviceName]->start(camerainfo);
    }

    connect(m_locationCtrl_.get(), &LocationCtrl::updateLocationSignal, this, &HikManager::onStartRecord);

    return true;
}

bool HikManager::stop()
{
    for (auto &camera : m_cameraMap_)
    {
        if (camera.second)
        {
            camera.second->closeCamera();
        }
    }

    return true;
}

void HikManager::onStartRecord(QString filename)
{
    emit startRecord(_mqstr_SavePath, filename);
}

void HikManager::onLocation(const QVariant &var)
{
    QVariantMap map = var.toMap();
    auto it = map.find("站区名");

    if (it != map.end())
    {
        if (nullptr != m_locationCtrl_)
        {
            m_locationCtrl_->updateLocation(it.value().toString());
        }
    }

    std::vector<QString> subtitles;
    if (map.contains("站区名"))
    {
        subtitles.push_back("站区名:" + map["站区名"].toString());
    }

    if (map.contains("锚段名"))
    {
        subtitles.push_back("锚段名:" + map["锚段名"].toString());
    }

    if (map.contains("杆号名"))
    {
        subtitles.push_back("杆号名:" + map["杆号名"].toString());
    }

    if (map.contains("公里标"))
    {
        subtitles.push_back("公里标:" + QString::number(map["公里标"].toDouble(), 'f', 3));
    }

    if (map.contains("速度"))
    {
        subtitles.push_back("速度:" + QString::number(map["速度"].toDouble(), 'f', 3));
    }

    emit sendSubTitle(subtitles);
}

void HikManager::notifyTask(TIS_Info::TaskInfo taskInfo)
{
    if (taskInfo.nTaskstate == 1)
    {
        emit stopRecord();
        return;
    }

    _mqstr_SavePath = taskInfo.strTaskSavePath;

    m_locationCtrl_->updateLocation("");
}