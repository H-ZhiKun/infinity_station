#include "BaslerManager.h"
#include "kits/camera/basler/factory/BaslerFactory.h"
#include <QDebug>
#include <chrono>
#include "kits/common/log/CRossLogger.h"
#include "kits/common/system_config/SystemConfig.h"
#include "kits/camera/basler/trigger/BaslerTrigger.h"
#include "kits/camera/basler/patrol_inspection/BaslerPatrolIns.h"

#include <chrono>

using namespace _Kits;
using namespace TIS_Info;
using namespace Pylon;

BaslerManager::BaslerManager()
{
}

BaslerManager::~BaslerManager()
{
}

bool BaslerManager::init(const YAML::Node &config)
{
    try
    {
        // 注册相机工厂

        BaslerFactory::RegisterCameraCreateFactory<BaslerTrigger>(std::string(BaslerTrigger::CameraType));
        BaslerFactory::RegisterCameraCreateFactory<BaslerPatrolIns>(std::string(BaslerPatrolIns::CameraType));

        // 读取配置
        auto &sysConfig = _Kits::SystemConfig::instance();
        auto baslerFilePath = sysConfig.configVersionPath() + config["basler_path"].as<std::string>("config/camera/basler.yaml");
        auto baslerConfig = _Kits::SystemConfig::instance().loadYamlFile(baslerFilePath);

        // 存储相机信息
        for (const auto &camConfig : baslerConfig["basler_info"])
        {
            _mmap_CameraSN_Name[camConfig["sn"].as<std::string>()] = camConfig["name"].as<std::string>();
            _mmap_CameraSN_Type[camConfig["sn"].as<std::string>()] = camConfig["type"].as<std::string>();

            if (camConfig["type"].as<std::string>() == _Kits::BaslerTrigger::CameraType)
            {
                _m_cameraNames_trigger.push_back(camConfig["name"].as<std::string>());
            }
            else if (camConfig["type"].as<std::string>() == _Kits::BaslerPatrolIns::CameraType)
            {
                _m_cameraNames_patrol.push_back(camConfig["name"].as<std::string>());
            }
        }

        // 启动初始化线程
        _m_initThread = std::thread(&BaslerManager::processCameraInitialization, this);
        return true;
    }
    catch (const std::exception &e)
    {
        qCritical() << "BaslerManager init failed:" << e.what();
        return false;
    }
}

bool BaslerManager::start()
{
    emit initTriggerCtrls(_m_cameraNames_trigger);
    emit initPatrolCtrls(_m_cameraNames_patrol);
    _m_startThread = std::thread([this]() {
        while (!_mb_stopped.load())
        {
            std::unique_lock<std::mutex> lock(_m_mutex_start);
            _m_cond_init.wait(lock, [this]() { return _mb_stopped.load() || _mb_initialized.load(); });

            if (_mb_stopped.load())
            {
                return;
            }

            connectCameraSignals();
        }
    });

    return true;
}

bool BaslerManager::stop()
{
    std::lock_guard<std::mutex> lock(_m_mutex_init);
    _mb_stopped.store(true);

    _m_cond_init.notify_all();

    if (_m_initThread.joinable())
    {
        _m_initThread.join();
    }

    // 关闭所有相机
    for (auto &[sn, camera] : _mmap_sn_camera)
    {
        camera->closeCamera();
    }
    _mmap_sn_camera.clear();

    PylonTerminate();
    return true;
}

void BaslerManager::processCameraInitialization()
{
    try
    {
        PylonInitialize();

        {
            std::lock_guard<std::mutex> lock(_m_mutex_init);
            initCameraList();
        }

        if (!_mmap_sn_camera.empty())
        {
            _mb_initialized.store(true);
            _m_cond_init.notify_one();
        }
    }
    catch (const GenericException &e)
    {
        qCritical() << "Pylon initialization error:" << e.GetDescription();
    }
}

void BaslerManager::connectCameraSignals()
{
    for (auto &[sn, camera] : _mmap_sn_camera)
    {
        if (camera->start())
        {
            connect(camera.get(), &_Kits::BaslerBase::sendImage, this, &BaslerManager::sendImage);
        }
    }
}

void BaslerManager::initCameraList()
{
    try
    {
        // 获取传输层工厂单例并创建设备枚举器
        CTlFactory::GetInstance().EnumerateDevices(_m_deviceLists);

        if (_m_deviceLists.size() < 0)
        {
            qDebug() << "No devices found";
            return;
        }

        for (auto &&device : _m_deviceLists)
        {
            const std::string sn = device.GetSerialNumber().c_str();

            if (_mmap_CameraSN_Type.find(sn) == _mmap_CameraSN_Type.end())
            {
                continue;
            }

            const auto &cameraType = _mmap_CameraSN_Type.at(sn);

            // 避免重复初始化
            if (_mmap_sn_camera.find(sn) != _mmap_sn_camera.end())
            {
                continue;
            }

            std::unique_ptr<Pylon::CInstantCamera> instantcamera = std::make_unique<Pylon::CInstantCamera>();

            try
            {
                // 分离设备创建步骤
                Pylon::CDeviceInfo deviceInfo;
                deviceInfo.SetSerialNumber(sn.c_str());

                auto *pDevice = CTlFactory::GetInstance().CreateFirstDevice(deviceInfo); // 可能抛出GenericException
                if (!pDevice)
                {
                    continue;
                }

                instantcamera->Attach(pDevice); // 可能抛出GenericException
            }
            catch (const GenericException &e)
            {
                LogError("BaslerManager::initCameraList():Attach failed:{}", e.GetDescription());
                continue;
            }
            // 工厂模式创建相机实例
            auto camera = BaslerFactory::CreateCameraByType(cameraType);

            if (camera->init(std::move(instantcamera)))
            {
                _mmap_sn_camera.emplace(sn, std::move(camera));
                qDebug() << "Initialized camera:" << sn.c_str();
            }
        }
    }
    catch (const GenericException &e)
    {
        LogError("BaslerManager::initCameraList():Device enumeration failed:{}", e.GetDescription());
    }
}

void BaslerManager::onStartRecordTriggerByOneCam(QString camemra_name)
{
    auto now = std::chrono::system_clock::now();

    QString now_str = QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(now)).toString("yyyyMMdd_hhmmss");

    emit startRecordTriggerByOneCam("d/gwkj/droptrigger", now_str);
}