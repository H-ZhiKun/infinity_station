// BaslerManager.h
#pragma once
#include "kits/common/module_base/ModuleBase.h"
#include "kits/camera/basler/base/BaslerBase.h"

#include "pylon/PylonIncludes.h"
#include "pylon/InstantCamera.h"

#include <memory>
#include <yaml-cpp/yaml.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include "kits/common/utils/Utils.h"

class BaslerManager : public _Kits::ModuleBase
{
    Q_OBJECT
    DECLARE_MODULE(BaslerManager)
  public:
    explicit BaslerManager();
    ~BaslerManager();

    bool init(const YAML::Node &config);
    bool start();
    bool stop();

    // 设备管理
    bool removeCamera(const std::string &serialNumber);
    bool searchCamera(const std::string &serialNumber = "");

  signals:
    void sendImage(std::shared_ptr<TIS_Info::ImageBuffer>);
    void initTriggerCtrls(std::vector<std::string>);
    void initPatrolCtrls(std::vector<std::string>);
    void grabImage(int timeout = 1000);
    void stopRecordTriggerByOneCam();
    /// @brief 由前端手动触发的录像
    /// @param  rootPath 根目录
    /// @param  录像名称
    /// @note   主要需要将录像名称和录像路径一起传递，在controller处拼接名字
    void startRecordTriggerByOneCam(QString, QString);

  private slots:
    /// @brief
    /// @param name 相机名
    void onStartRecordTriggerByOneCam(QString);

  private:
    void initSystem();
    void initCameraList();

    void processCameraInitialization();
    void connectCameraSignals();

    std::unordered_map<std::string, std::unique_ptr<_Kits::BaslerBase>> _mmap_sn_camera; // sn_camera
    std::unordered_map<std::string, std::string> _mmap_CameraSN_Name;                    // 用于存储相机序列号和名字
    std::unordered_map<std::string, std::string> _mmap_CameraSN_Type;                    // 用于存储相机序列号和类型
    std::vector<std::string> _m_cameraNames_trigger;
    std::vector<std::string> _m_cameraNames_patrol;

    Pylon::DeviceInfoList_t _m_deviceLists;

    std::thread _m_initThread;
    std::thread _m_startThread;
    std::condition_variable _m_cond_init;
    std::mutex _m_mutex_start;
    std::mutex _m_mutex_init;
    std::atomic_bool _mb_initialized = false;
    std::atomic_bool _mb_stopped = false;
};