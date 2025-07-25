#pragma once
#include "CameraBaumer.h"
#include "bgapi2_genicam.hpp"
#include "kits/camera_service/CameraBase.h"

// #include "kits/camera_service/CameraBumer.h"
#include "kits/camera_service/encodevideo.h"
#include "kits/camera_service/encodevideoffmpeg.h"
#include "kits/camera_service/muxingvideo.h"
#include <QObject>
#include <yaml-cpp/yaml.h>

namespace _Kits
{
class CameraBumerManager : public QObject
{
    Q_OBJECT
  public:
    explicit CameraBumerManager();
    virtual ~CameraBumerManager();
    virtual void init(const YAML::Node &config);

    void cleanCameraCount();
    void resetCameraCount(const std::string &key, const uint32_t count);
    void removeCamera(const std::string &snNumber);
    uint32_t getCameraCount(const std::string);

    std::vector<std::string> cameraState();
    void searchGEVCamera();   // 搜索网口相机
    void searchUSBCamera();   // 搜索USB相机
    void captureImageStart(); // 图像采集

  protected:
    void initTimer(); // 初始化定时器
    void initializeBGAPI();
    void deinitializeBGAPI();
    void stop();
    bool addCamera(const std::string &snNumber, BGAPI2::Device *dev);
    bool forceIP(const std::string &snNumber, BGAPI2::NodeMap *nodeMap);

  private:
    std::atomic_bool bHold_{true};
    std::mutex mtxCamera_;
    std::string interfaceIp_;
    std::string interfaceMask_;
    std::string interfaceMAC_;
    uint32_t u32InterfaceIp_ = 0;
    uint32_t u32InterfaceMask_ = 0;
    YAML::Node nodeParams_;
    std::unordered_map<std::string, std::unique_ptr<CameraBaumer>>
        mapCameras_; // 存储每个baumerCamera 用SNnum对应
    std::mutex mtxCount_;
    std::unordered_map<std::string, uint32_t> mapCameraCounts_;
    std::unordered_map<std::string, CameraInfo>
        mapCameraInfo_; // 保存每个baumerCamera参数，以相机SNNUM对应
    BGAPI2::System *pSystem_ = nullptr;
    QTimer *timerSearch_;
};
} // namespace _Kits