#include "BaumerManager.h"
#include "kits/camera/baumer/base/BaumerBase.h"
#include "kits/camera/baumer/camera_normal/BaumerCamera.h"
#include "kits/camera/baumer/arc/BaumerArc.h"
#include <chrono>
#include <memory>
#include <qdebug>
#include <qobject.h>
#include <qtimer.h>
#include <thread>
#include "kits/common/system_config/SystemConfig.h"
#include "kits/camera/baumer/factory/BaumerFactory.h"
#include "tis_global/EnumPublic.h"
#include "kits/common/thread_pool/ConcurrentPool.h"

using namespace _Kits;
using namespace _Modules;

void BGAPI2CALL BaumerManager::PnPEventHandler(void *callBackOwner, BGAPI2::Events::PnPEvent *pBuffer)
{
    BaumerManager *manager = (BaumerManager *)callBackOwner;
    std::string snNumber = pBuffer->GetSerialNumber().get();
    if (pBuffer->GetPnPType() == BGAPI2::Events::PnPType::PNPTYPE_DEVICEREMOVED)
    {
        qDebug() << "Camera " << snNumber << " is removed";
        manager->removeCamera(snNumber);
    }
    else if (pBuffer->GetPnPType() == BGAPI2::Events::PnPType::PNPTYPE_DEVICEADDED)
    {
        qDebug() << "Camera " << snNumber << " is added";
        manager->searchCamera(snNumber);
    }
}

BaumerManager::BaumerManager(QObject *parent) : m_locationCtrl_(std::make_unique<LocationCtrl>()), _mqstr_SavePath("d:/gwkj/arcRecord/")
{
}
BaumerManager::~BaumerManager()
{
}

bool BaumerManager::init(const YAML::Node &config)
{
#if defined(USE_KIT_CAMERA_BAUMER_NORMAL)
    BaumerFactory::RegisterCameraCreateFactory<BaumerCamera>(std::string(BaumerCamera::CameraType));
#endif
#if defined(USE_KIT_CAMERA_BAUMER_ARC)
    BaumerFactory::RegisterCameraCreateFactory<BaumerArc>(std::string(BaumerArc::CameraType));
#endif

    auto &sysConfig = _Kits::SystemConfig::instance();
    auto baumerFilePath =
        sysConfig.configVersionPath() + config["config_path"].as<std::string>("/config/camera_config/baumer/camera_info.yaml");
    auto Baumerconfig = _Kits::SystemConfig::instance().loadYamlFile(baumerFilePath);

    m_thInit_ = std::thread([this, Baumerconfig] {
        for (auto &camconfig : Baumerconfig["baumer_info"])
        {
            mmap_CameraSN_Name[camconfig["sn"].as<std::string>()] = camconfig["camera_name"].as<std::string>();
            mmap_CameraSN_Type[camconfig["sn"].as<std::string>()] = camconfig["type"].as<std::string>();

            if (camconfig["type"].as<std::string>() == BaumerArc::CameraType)
            {
                mvec_CameraName_arc_.push_back(camconfig["camera_name"].as<std::string>());
            }
            else if (camconfig["type"].as<std::string>() == BaumerCamera::CameraType)
            {
                mvec_CameraName_video_.push_back(camconfig["camera_name"].as<std::string>());
            }
        }

        while (true)
        {

            initSystem();
            if (m_bInit)
            {
                m_bStart = true;
                return;
            }
            deInit();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });
    return true;
}

bool BaumerManager::start()
{
    m_thread_start_ = std::thread([this] {
        while (!m_bStart && !m_bStop) // 等待初始化成功
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (m_bStop)
        {
            return;
        }

        emit initVideoCtrl(mvec_CameraName_video_); // 初始化全程录像Control
        emit initArcCtrl(mvec_CameraName_arc_);     // 初始化燃弧录像Control

        for (const auto &[sn, name] : mmap_CameraSN_Name)
        {
            if (m_mapCamera.find(sn) != m_mapCamera.end())
            {
                m_mapCamera[sn]->setName(name);
                connect(m_mapCamera[sn].get(), &BaumerBase::sendImage, this, &BaumerManager::sendImage);
                connect(m_mapCamera[sn].get(), &BaumerBase::sendArcImage, this, &BaumerManager::sendArcImage);
            }
        }

        ConcurrentPool::runAfter(10000, [this] {
            TIS_Info::arcData arcData;
            arcData.arcDeviceIndex = "noTask";
            arcData.arcId = 0;
            arcData.arccount = 0;
            arcData.arcpulse = 0;
            arcData.arctime = 0;
            arcData.cameraName = "test_baumer1";
            arcData.fileName = "test_baumer1";
            arcData.occurtime = "0";

            onStartArc(arcData);
            onStartRecord("noTask");
        });
    });

    connect(m_locationCtrl_.get(), &LocationCtrl::updateLocationSignal, this, &BaumerManager::onStartRecord);

    return true;
}

bool BaumerManager::stop()
{
    emit stopRecord();
    emit stopArc();

    m_bStop = true;

    if (m_thInit_.joinable())
        m_thInit_.join();

    if (m_thread_start_.joinable())
        m_thread_start_.join();

    closeCameras();

    deInit();
    return true;
}

bool BaumerManager::removeCamera(const std::string &snNumber)
{
    try
    {
        m_mapCamera.erase(snNumber);
        return true;
    }
    catch (const std::exception &e)
    {
        qDebug() << "BaumerManager::removeCamera:" << e.what() << '\n';
    }
}
bool BaumerManager::searchCamera(BGAPI2::Interface *ifc, const std::string &needNumber)
{
    try
    {
        if (ifc == nullptr)
        {
            return false;
        }

        if (!ifc->IsOpen())
        {
            return false; // 这里开了也无法传入回调，开启这个接口不是这个函数的职责
        }
        BGAPI2::DeviceList *devList = ifc->GetDevices();
        devList->Refresh(100);
        for (auto dev_iter = devList->begin(); dev_iter != devList->end(); dev_iter++)
        {
            auto pDevice = dev_iter->second;
            // // 我不确定名称是不是Baumer，目标是为了过滤掉非Baumer的相机   zzhh ???
            // if (pDevice->GetVendor() != "Baumer")
            // {
            //     continue;
            // }
            if (pDevice->GetAccessStatus() != "RW")
            {
                continue;
            }
            std::string snNumber = pDevice->GetSerialNumber().get();
            if (!needNumber.empty())
            {
                if (snNumber != needNumber)
                {
                    continue;
                }
            }
            std::string str_type = mmap_CameraSN_Type[snNumber];
            auto pCamera = BaumerFactory::CreateCameraByType(str_type);
            if (pCamera)
            {
                pDevice->Open();
                pCamera->init(pDevice); // init只会在pDevice为空时才会失败
                if (pCamera->start())   // start失败（异常）需要关闭pDevice
                {

                    if (str_type == BaumerCamera::CameraType)
                    {
                        QObject::connect(pCamera.get(), &BaumerCamera::sendImage, this, &BaumerManager::sendImage);
                    }
                    else if (str_type == BaumerArc::CameraType)
                    {
                        QObject::connect(pCamera.get(), &BaumerArc::sendArcImage, this, &BaumerManager::sendArcImage);
                    }

                    m_mapCamera[snNumber] = std::move(pCamera);
                    return true;
                }
            }
        }
    }

    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type:" << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }

    return false;
}
bool _Modules::BaumerManager::searchCamera(const std::string &needNumber)
{
    {
        try
        {
            BGAPI2::InterfaceList *ifl = m_pSystem->GetInterfaces();
            ifl->Refresh(100);
            for (auto iter = ifl->begin(); iter != ifl->end(); iter++)
            {
                auto ifc = iter->second;
                if (!ifc->IsOpen())
                {
                    ifc->Open();
                }
                ifc->Open();
                BGAPI2::DeviceList *devList = ifc->GetDevices();
                devList->Refresh(100);
                for (auto dev_iter = devList->begin(); dev_iter != devList->end(); dev_iter++)
                {
                    auto pDevice = dev_iter->second;
                    // // 我不确定名称是不是Baumer，目标是为了过滤掉非Baumer的相机   zzhh ???
                    // if (pDevice->GetVendor() != "Baumer")
                    // {
                    //     continue;
                    // }
                    if (pDevice->GetAccessStatus() != "RW")
                    {
                        continue;
                    }
                    std::string snNumber = pDevice->GetSerialNumber().get();
                    if (!needNumber.empty())
                    {
                        if (snNumber != needNumber)
                        {
                            continue;
                        }
                    }
                    std::string str_type = mmap_CameraSN_Type[snNumber];
                    auto pCamera = BaumerFactory::CreateCameraByType(str_type);
                    if (pCamera)
                    {
                        pDevice->Open();
                        pCamera->init(pDevice); // init只会在pDevice为空时才会失败
                        if (pCamera->start())   // start失败（异常）需要关闭pDevice
                        {

                            if (str_type == BaumerCamera::CameraType)
                            {
                                QObject::connect(pCamera.get(), &BaumerCamera::sendImage, this, &BaumerManager::sendImage);
                            }
                            else if (str_type == BaumerArc::CameraType)
                            {
                                QObject::connect(pCamera.get(), &BaumerArc::sendArcImage, this, &BaumerManager::sendArcImage);
                            }

                            m_mapCamera[snNumber] = std::move(pCamera);
                            return true;
                        }
                    }
                }
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Error Type:" << ex.GetType().get();
            qDebug() << "Error function: " << ex.GetFunctionName().get();
            qDebug() << "Error description: " << ex.GetErrorDescription().get();
        }

        return false;
    }
}
void BaumerManager::initInterface()
{
    try
    {
        BGAPI2::InterfaceList *ifl = m_pSystem->GetInterfaces();
        ifl->Refresh(1000);
        for (auto iter = ifl->begin(); iter != ifl->end(); iter++)
        {
            auto ifc = iter->second;
            if (!ifc->IsOpen())
            {
                ifc->Open();
            }

            std::string name = ifc->GetDisplayName().get();
            std::string ip = ifc->GetNode("GevInterfaceSubnetIPAddress")->GetValue().get();
            std::string mac = ifc->GetNode("GevInterfaceMACAddress")->GetValue().get();
            std::string mask = ifc->GetNode("GevInterfaceSubnetMask")->GetValue().get();

            ifc->second->RegisterPnPEvent(BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
            ifc->second->RegisterPnPEventHandler(this, &BaumerManager::PnPEventHandler);
            qDebug() << "init baumer interface founded: ";
            qDebug() << "founded ip: " << ip;
            qDebug() << "founded MAC: " << mac;
            qDebug() << "founded mask: " << mask;
            searchCamera(ifc);
            m_bInit = true;
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type:" << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
}

void BaumerManager::initSystem()
{
    try
    {
        BGAPI2::SystemList *system_list =
            BGAPI2::SystemList::GetInstance(); // 如果同时使用USB3和GigE接口相机，则需要创建两个System，并分别实例化
        system_list->Refresh();
        for (auto sys_iter = system_list->begin(); sys_iter != system_list->end(); sys_iter++)
        {
            BGAPI2::System *pSystem = sys_iter->second;
            BGAPI2::String tl_type = pSystem->GetTLType();
            if (tl_type != "GEV")
            {
                continue;
            }
            if (pSystem->IsOpen())
            {
                continue;
            }
            pSystem->Open();
            m_pSystem = pSystem;
            initInterface();
            qDebug() << "baumer system init successed.";
        }
        if (m_pSystem == nullptr)
        {
            qDebug() << "baumer system init failed.";
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type:" << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
}

void BaumerManager::closeCameras()
{
    try
    {
        m_mapCamera.clear();
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type:" << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
}

void BaumerManager::deInit()
{
    try
    {
        if (m_pSystem == nullptr)
        {
            return;
        }
        if (!m_pSystem->IsOpen())
        {
            return;
        }
        BGAPI2::InterfaceList *interface_list = m_pSystem->GetInterfaces();
        interface_list->Refresh(1000);
        for (auto ifc_iter = interface_list->begin(); ifc_iter != interface_list->end(); ifc_iter++)
        {
            auto pInterface = ifc_iter->second;
            if (!pInterface->IsOpen())
            {
                continue;
            }
            pInterface->UnregisterPnPEvent();
            pInterface->Close();
        }
        m_pSystem->Close();
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {

        qDebug() << "Error Type:" << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
    BGAPI2::SystemList::ReleaseInstance();
}

void BaumerManager::onStartRecord(QString filename)
{
    emit startRecord(filename, _mqstr_SavePath);
}

void BaumerManager::onLocation(const QVariant &var)
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

void BaumerManager::notifyTask(TIS_Info::TaskInfo taskInfo)
{
    if (taskInfo.nTaskstate == TIS_Info::EnumPublic::TASK_STOPPED) // 任务结束
    {
        emit stopRecord();
        return;
    }

    _mqstr_SavePath = taskInfo.strTaskSavePath;

    m_locationCtrl_->updateLocation("");
}

void BaumerManager::onStartArc(TIS_Info::arcData arcdata) // camera name  arccontroller 组装的filename
{
    emit startArc(arcdata, _mqstr_SavePath); //_mqstr_SavePath
}
