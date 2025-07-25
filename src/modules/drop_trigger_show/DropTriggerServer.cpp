#include "DropTriggerServer.h"

using namespace _Modules;
using namespace _Kits;
using namespace TIS_Info;

DropTriggerServer::DropTriggerServer(QObject *parent) : ModuleBase(parent)
{
}

DropTriggerServer::~DropTriggerServer() noexcept
{
}

bool DropTriggerServer::init(const YAML::Node &config)
{
    auto config_path = config["config_path"].as<std::string>();

    YAML::Node config_node = YAML::LoadFile(std::filesystem::current_path().string() + config_path);
    CameraInfo camInfo;

    std::string modelFile = std::filesystem::current_path().string() + config["modelFile"].as<std::string>();
    std::string labelMapFile = std::filesystem::current_path().string() + config["labelMapFile"].as<std::string>();
    std::string hogParamsFile = std::filesystem::current_path().string() + config["hogParamsFile"].as<std::string>();

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

    emit initVideosave();
    emit initSVM(modelFile, labelMapFile, hogParamsFile);
    return true;
}
bool DropTriggerServer::start()
{
    for (const auto &camera : m_yaml_config["camera_info"])
    {
        CameraInfo camInfo;
        ReadCameraConfig(camera, camInfo);

        connect(m_mapCamera[camInfo.mstr_CameraName].get(), &CameraBase::ImageOutPut, this, &DropTriggerServer::onGetImage);
        m_mapCamera[camInfo.mstr_CameraName]->openCamera(camera);
        m_mapCamera[camInfo.mstr_CameraName]->startGrab();
    }

    return true;
}
bool DropTriggerServer::stop()
{
    m_mapCamera.clear();
    return true;
}

void DropTriggerServer::onGetImage(const QImage image, const std::string &camera_name) // 需要改为值传递，因为QImage是shared_ptr机制
{
    static int count = 0;

    ImageInfo imgInfo;
    imgInfo.image = image;
    imgInfo.name = camera_name;

    emit imgout(imgInfo);

    if (++count % 15 == 0)
    {
        emit imgoutqml(imgInfo);
    }
}

void DropTriggerServer::onRecvRecordByQml(QVariant record)
{
    TIS_Info::RecordInfo recordInfo;
    bool bisrecord = record == 0 ? true : false;

    recordInfo.mb_is_record = bisrecord;
    recordInfo.mstr_record_path = mqstr_record_path;
    recordInfo.mstr_camera_name = mqstr_camera_name;

    emit isRecord(std::move(recordInfo));
}

void DropTriggerServer::onTrigger()
{
    for (auto &camera : m_mapCamera)
    {
        camera.second->SetTrigger("On", "Line3", "RisingEdge");
    }
}

void DropTriggerServer::ReadCameraConfig(const YAML::Node &config, CameraInfo &camInfo)
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
    mqstr_camera_name = config["camera_name"].as<std::string>().c_str();
    camInfo.mus_IsShowImg = static_cast<ReturnCode>(config["isshow_img"].as<int>());
    camInfo.mi_IsShowData = static_cast<ReturnCode>(config["isshow_data"].as<int>());
    camInfo.mstr_SaveImageType = (config["save_img_type"].as<std::string>());
    camInfo.mi_SaveImageMod = static_cast<PicSaveType>(config["save_img_mod"].as<int>());
    camInfo.mui8_Channel = config["channel"].as<unsigned char>();
    camInfo.mstr_Img_SavePath = config["save_img_path"].as<std::string>();
    mqstr_record_path = config["save_video_path"].as<std::string>().c_str();
}
