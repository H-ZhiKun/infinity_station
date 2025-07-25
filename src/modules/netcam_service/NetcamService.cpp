#include "NetcamService.h"

using namespace _Modules;
using namespace _Kits;
using namespace TIS_Info;

NetcamService::NetcamService(QObject *parent) : ModuleBase(parent)
{
}

NetcamService::~NetcamService() noexcept
{
}

bool NetcamService::init(const YAML::Node &config)
{
    auto config_path = config["config_path"].as<std::string>();

    YAML::Node config_node = YAML::LoadFile(std::filesystem::current_path().string() + config_path);
    CameraInfo camInfo;

    if (config_node["camera_info"] && config_node["camera_info"].IsSequence())
    {
        m_yaml_config = config_node;

        for (const auto &camera : config_node["camera_info"])
        {
            ReadCameraConfig(camera, camInfo);
            auto camera_by_type = std::make_unique<CameraBasler>();

            if (nullptr == camera_by_type)
            {
                continue;
            }

            m_mapCamera[camInfo.mstr_CameraName] = std::move(camera_by_type);
        }
    }
    return true;
}
bool NetcamService::start()
{
    QStringList snList;
    for (const auto &camera : m_yaml_config["camera_info"])
    {
        CameraInfo camInfo;
        ReadCameraConfig(camera, camInfo);

        connect(m_mapCamera[camInfo.mstr_CameraName].get(), &CameraBase::ImageOutPut, this, &NetcamService::onGetImage);
        if (m_mapCamera[camInfo.mstr_CameraName]->openCamera(camera))
        {
            QString key = QString().fromStdString(camInfo.mstr_CameraName);
            m_mapInitinfo[key] = true;
            m_mapInitinfo[QString().fromStdString(serialNumber)] = key;
            snList << QString().fromStdString(serialNumber);
            m_mapCamera[camInfo.mstr_CameraName]->startGrab();
        }
        else
        {
            QString key = QString().fromStdString(camInfo.mstr_CameraName);
            m_mapInitinfo[key] = false;
            QString sn = key + "-badsn";
            m_mapInitinfo[sn] = key;
            snList << sn;
        }
    }
    m_mapInitinfo["snlist"] = snList;
    emit initinfo(m_mapInitinfo);
    return true;
}
bool NetcamService::stop()
{
    m_mapCamera.clear();
    return true;
}

void NetcamService::onGetImage(const QImage image, const std::string &camera_name) // 需要改为值传递，因为QImage是shared_ptr机制
{
    ImageInfo imgInfo;
    imgInfo.image = image;
    imgInfo.name = camera_name;
    emit imgout(imgInfo);
}

void NetcamService::onRecvRecordByQml(QVariant record)
{
    TIS_Info::RecordInfo recordInfo;

    QString qstr_record = record.toString();

    bool bisrecord = record.toString() == "start" ? true : false;

    recordInfo.mb_is_record = bisrecord;
    recordInfo.mstr_record_path = mqstr_record_path;

    emit isRecord(std::move(recordInfo));
}

void NetcamService::onTrigger()
{
    for (auto &camera : m_mapCamera)
    {
        camera.second->SetTrigger("On", "Line3", "RisingEdge");
    }
}

void NetcamService::ReadCameraConfig(const YAML::Node &config, CameraInfo &camInfo)
{
    camInfo.mstr_Camera_Type = config["camera_type"].as<std::string>();
    camInfo.mstr_Camera_ID = config["camera_ip"].as<std::string>();
    camInfo.mus_CameraLeft = config["camera_left"].as<unsigned short>();
    camInfo.mus_CameraRight = config["camera_right"].as<unsigned short>();
    camInfo.mus_CameraTop = config["camera_top"].as<unsigned short>();
    camInfo.mus_CameraBottom = config["camera_bottom"].as<unsigned short>();
    camInfo.mus_CameraGain = config["camera_gain"].as<unsigned short>();
    camInfo.mus_CameraExposure = config["camera_exposure"].as<unsigned short>();
    camInfo.mus_CameraFps = config["camera_fps"].as<unsigned short>();
    camInfo.mi_CameraPixelFormat = static_cast<PixelFormat>(config["camera_pixelformat"].as<int>());
    camInfo.mus_CameraPort = config["camera_port"].as<unsigned short>();
    camInfo.mstr_CameraPassword = config["camera_pwd"].as<std::string>();
    camInfo.mstr_CameraUserName = config["camera_username"].as<std::string>();
    camInfo.mstr_CameraName = config["camera_name"].as<std::string>();
    camInfo.mus_IsShowImg = static_cast<ReturnCode>(config["isshow_img"].as<int>());
    camInfo.mi_IsShowData = static_cast<ReturnCode>(config["isshow_data"].as<int>());
    camInfo.mstr_SaveImageType = (config["save_img_type"].as<std::string>());
    camInfo.mi_SaveImageMod = static_cast<PicSaveType>(config["save_img_mod"].as<int>());
    camInfo.mui8_Channel = config["channel"].as<unsigned char>();
    camInfo.mstr_Img_SavePath = config["save_img_path"].as<std::string>();
    mqstr_record_path = config["save_video_path"].as<std::string>().c_str();
    serialNumber = config["serial_number"].as<std::string>();
}