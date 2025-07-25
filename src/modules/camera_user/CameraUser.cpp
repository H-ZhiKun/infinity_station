// #include "./kits/required/log/CRossLogger.h"
#include "CameraUser.h"
#include "kits/camera_service/CameraBase.h"
#include "kits/camera_service/CameraFactory.h"
#include "kits/camera_service/ImgChannel.h"
#include <QImage>
#include <filesystem>
#include <memory>
#include <mutex>
// #include <objidlbase.h>
#include "kits/common/log/CRossLogger.h"
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qlogging.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <string>
#include <vector>

using namespace _Kits;
using namespace _Modules;

CameraUser::CameraUser(QObject *parent) : ModuleBase(parent)
{
    // pantographDetectionModel input = {0};    //接口有问题
    // PantographDetection_Init(input);   //接口有问题
}

CameraUser::~CameraUser()
{
    // stop();
    // #if __has_include(<windows.h>)
    //     CoUninitialize();
    // #elif __has_include(<unistd.h>)

    // #endif
}
bool CameraUser::init(const YAML::Node &config)
{
    if (config["config_path"].as<std::string>().empty())
    {
        LogError("No configuration path found.");
        return false;
    }
    std::string config_path = config["config_path"].as<std::string>();
    try
    {
        m_details = YAML::LoadFile(std::filesystem::current_path().string() + config_path);
    }
    catch (const YAML::BadFile &e)
    {
        LogError("load file path error {}", config_path);
        return false;
    }
    return true;
}
bool CameraUser::start()
{
    m_isclose = false;
    qRegisterMetaType<QImage>("QImage");
    m_allCameraCtrl.resize(5);

    connect(this, &CameraUser::saveImage, this, &CameraUser::OnSaveImage);

    ImgChannel::RegisterImgChannel(1, QImage::Format_Grayscale8); // 灰度
    ImgChannel::RegisterImgChannel(3, QImage::Format_RGB16);      // RGB

    if (nullptr == m_locationCtrl)
    {
        m_locationCtrl = std::make_unique<LocationCtrl>();
    }
    m_locationCtrl->initLocationCtrl();

    if (nullptr == m_detectCtrl)
    {
        m_detectCtrl = std::make_unique<DetectCtrl>();
    }
    m_detectCtrl->init();

    if (m_details["camera_info"] && m_details["camera_info"].IsSequence())
    {
        createCamera(m_details);
    }

    return true;
}

void CameraUser::createCamera(const YAML::Node &config_node)
{
    m_allCameraCtrl.clear();
    CameraInfo camInfo;
    for (const auto &camera : config_node["camera_info"])
    {
        ReadCameraConfig(camera, camInfo);
        auto camera_ctr = std::make_shared<CameraCtrl>();
        if (nullptr == camera_ctr)
        {
            continue;
        }
        camera_ctr->createObjectByType(camInfo.mstr_Camera_Type);

        if (0 == camera_ctr->start(camera))
        {
            // m_allCameraCtrl.emplace_back(std::move(camera_ctr));
            connect(this, &CameraUser::subtitleSend, camera_ctr.get(), &CameraCtrl::onSubtitleChanged);

            connect(this, &CameraUser::startRecord, camera_ctr.get(), &CameraCtrl::onStartRecord);

            connect(this, &CameraUser::stopRecord, camera_ctr.get(), &CameraCtrl::onStopRecord);

            connect(camera_ctr.get(), &CameraCtrl::popImageSignals, this, &CameraUser::imgout);

            connect(camera_ctr.get(), &CameraCtrl::sendPathToArcuv, [this](const QVariant &path) { emit this->sendPathToArcuv(path); });

            connect(m_locationCtrl.get(), &LocationCtrl::updateLocationSignal, camera_ctr.get(), &CameraCtrl::onStartRecord);

            connect(this, &CameraUser::CallFromArc, camera_ctr.get(), &CameraCtrl::onCacheStartRecord);

            connect(this, &CameraUser::sendTaskPathToCtl, camera_ctr.get(), &CameraCtrl::onVideoSavePathChanged);

            connect(camera_ctr.get(), &CameraCtrl::flagRecord, this, &CameraUser::flagRecord);
            // emit startRecord("test_me");
            // camera_ctr->onStartRecord("test_me");
            m_allCameraCtrl.emplace_back(std::move(camera_ctr));
        }
    }
}
bool CameraUser::stop()
{
    emit stopRecord();
    m_allCameraCtrl.clear();
    m_isclose = true;

    return true;
}

void CameraUser::PantographDetect(QImage &image, const std::string &camera_name)
{
    image.width();

    // m_detect_input.m_img_buffer = image.bits();
    // m_detect_input.m_img_width = image.width();
    // m_detect_input.m_img_height = image.height();
    // if (m_TotalInfo.m_CameraInfo.find(camera_name) !=
    //     m_TotalInfo.m_CameraInfo.end())
    // {
    //     m_detect_input.m_img_channel =
    //         m_TotalInfo.m_CameraInfo[camera_name].mui8_Channel;
    // }
    // else
    // {
    //     // log
    //     return ReturnCode_Fail;
    // }

    // 以下是检测
    // if(PantographDetection_Process(m_detect_input, &m_detect_results) == 0){
    //     if(m_detect_results.m_result_num > 0)
    //     {
    //         for(int i = 0; i < m_detect_results.m_result_num; i++)
    //         {
    //             qDebug() << "检测到电视弓缺陷: " <<
    //             m_detect_results.m_results[i].label_ID;
    //         }
    //         return ReturnCode_OVER;
    //     }
    // }

    return;
}

void CameraUser::SubtitleChange(const QVariant &var)
{
    if (m_allCameraCtrl.empty())
    {
        return;
    }

    QVariantMap map = var.toMap();
    std::vector<std::string> vec_subtitle;

    for (auto it = map.begin(); it != map.end(); ++it)
    {
        QString key = it.key();
        QVariant value = it.value();

        if (value.type() == QVariant::Double)
        {
            // Format the floating point number to 2 decimal places
            QString formattedValue = QString::number(value.toDouble(), 'f', 2);
            vec_subtitle.push_back(key.toStdString() + ":" + formattedValue.toStdString());
        }
        else
        {
            vec_subtitle.push_back(key.toStdString() + ":" + value.toString().toStdString());
        }
    }

    if (m_isclose)
    {
        return;
    }

    for (auto camera : m_allCameraCtrl)
    {
        camera->onSubtitleChanged(vec_subtitle);
    }

    auto it = map.find("stationName"); // 主要是定位如何定义map中的定位key是多少，不应该写死

    if (it != map.end())
    {
        if (nullptr != m_locationCtrl)
        {
            m_locationCtrl->updateLocation(it.value().toString().toStdString());
        }
    }
}

void CameraUser::CameraStartRecord(const QVariant &filename)
{
    emit startRecord(filename.toString().toStdString());
}

void CameraUser::CameraStopRecord()
{
    emit stopRecord();
}
void CameraUser::OnSaveImage(QImage image, const CameraInfo &camera_info, const QDateTime &dateTime)
{

    QString timestamp = dateTime.toString("yyyyMMdd_hhmmss");
    QString save_Path =
        QString("%1/%2").arg(QString::fromStdString(camera_info.mstr_Img_SavePath)).arg(camera_info.mstr_CameraName.c_str());
    QString save_Name = QString("%1/%2.%3").arg(save_Path).arg(timestamp).arg((camera_info.mstr_SaveImageType.c_str()));

    QDir dir(save_Path);
    if (!dir.exists())
    {
        dir.mkpath(save_Path);
    }

    if (!image.save(save_Name))
    {
        qDebug() << "Save image failed:" << save_Name;
    }
}
void CameraUser::ReadCameraConfig(const YAML::Node &config, CameraInfo &camInfo)
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
}

void CameraUser::onCallFromArc(const QVariant &value)
{
    QVariantMap map = value.toMap();

    map["location"] = m_locationCtrl->getLocation().c_str();

    emit CallFromArc(value);
}

void CameraUser::notifyTask(TIS_Info::TaskInfo taskInfo)
{
    if (taskInfo.nTaskstate == 1)
    {
        emit stopRecord();
        return;
    }

    emit sendTaskPathToCtl(taskInfo.strTaskSavePath.toStdString());
    m_locationCtrl->updateLocation("");
}