
#include "BaumerManager.h"
using namespace _Kits;
BaumerManager::BaumerManager()
{
}

BaumerManager::~BaumerManager()
{
    stopCaptureThread();
    stop();
}
void BaumerManager::init(const YAML::Node &config)
{
    if (config["camera_info"]["cameras"])
    {

        interfaceMAC_ =
            config["camera_info"]["interface"]["mac"].as<std::string>();
        std::transform(interfaceMAC_.begin(),
                       interfaceMAC_.end(),
                       interfaceMAC_.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        std::replace(interfaceMAC_.begin(), interfaceMAC_.end(), '-', ':');
        interfaceMask_ =
            config["camera_info"]["interface"]["mask"].as<std::string>();
        interfaceIp_ =
            config["camera_info"]["interface"]["ip"].as<std::string>();

        nodeParams_ = config["camera_info"]["cameras"];
        initializeBGAPI();
        searchCamera();
        for (int i = 0; i < 10; i++)
        {

            qDebug() << "search camera";
            searchCamera();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        // searchCamera();
        // startCaptureThread();
        // for (const auto &group : nodeParams_)
        // {
        //     const auto &key = group["display_window"].as<std::string>();
        //     mapCameraCounts_[key] = 0;
        // }
    }
}
void BaumerManager::initializeBGAPI()
{
    try
    {
        BGAPI2::SystemList *system_list = BGAPI2::SystemList::GetInstance();
        system_list->Refresh();
        for (BGAPI2::SystemList::iterator sys_iter = system_list->begin();
             sys_iter != system_list->end();
             sys_iter++)
        {
            BGAPI2::System *system_pointer = sys_iter->second;
            BGAPI2::String tl_type = system_pointer->GetTLType();
            if (tl_type != "GEV")
            {
                continue;
            }
            system_pointer->Open();
            pSystem_ = system_pointer;
            BGAPI2::InterfaceList *interface_list =
                system_pointer->GetInterfaces();
            interface_list->Refresh(100);
            for (BGAPI2::InterfaceList::iterator ifc_iter =
                     interface_list->begin();
                 ifc_iter != interface_list->end();
                 ifc_iter++)
            {
                ifc_iter->second->Open();
                // BGAPI2::DeviceList *dev_list =
                // ifc_iter->second->GetDevices(); dev_list->Refresh(100);
                // qDebug() << "device lists size:" << dev_list->size();
                // if (dev_list->size() == 0)
                // {
                //     qDebug() << "device size = 0";
                //     ifc_iter->second->Close();
                //     continue;
                // }
                std::string name = ifc_iter->second->GetDisplayName().get();
                std::string ip =
                    ifc_iter->second->GetNode("GevInterfaceSubnetIPAddress")
                        ->GetValue()
                        .get();
                std::string mac =
                    ifc_iter->second->GetNode("GevInterfaceMACAddress")
                        ->GetValue()
                        .get();
                std::string mask =
                    ifc_iter->second->GetNode("GevInterfaceSubnetMask")
                        ->GetValue()
                        .get();
                // 为相机对应数据流注册掉线触发事件
                if (mac != interfaceMAC_)
                {
                    qDebug() << "interface mac not matched." << mac;
                    ifc_iter->second->Close();
                    continue;
                }
                else if (ip != interfaceIp_)
                {
                    qDebug() << "interface mac not matched." << ip;
                    ifc_iter->second->Close();
                    continue;
                }
                ifc_iter->second->RegisterPnPEvent(
                    BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
                // // ifc_iter->second->RegisterPnPEventHandler(
                //     this, (BGAPI2::Events::PnPEventHandler)&PnPEventHandler);
                u32InterfaceIp_ =
                    ifc_iter->second->GetNode("GevInterfaceSubnetIPAddress")
                        ->GetInt();
                u32InterfaceMask_ =
                    ifc_iter->second->GetNode("GevInterfaceSubnetMask")
                        ->GetInt();
                qDebug() << "interface founded: ";
                qDebug() << "founded ip: {}" << ip;
                qDebug() << "founded MAC: {}" << mac;
                qDebug() << "founded mask: {}" << mask;
            }
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type:" << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
}

void BaumerManager::deinitializeBGAPI()
{
    try
    {
        if (pSystem_ != nullptr)
        {
            BGAPI2::InterfaceList *interfaceList = pSystem_->GetInterfaces();
            for (BGAPI2::InterfaceList::iterator ifc_iter =
                     interfaceList->begin();
                 ifc_iter != interfaceList->end();
                 ifc_iter++)
            {
                if (ifc_iter->second->IsOpen())
                {
                    ifc_iter->second->UnregisterPnPEvent();
                    ifc_iter->second->Close();
                }
            }
            pSystem_->Close();
        }
        BGAPI2::SystemList::ReleaseInstance();
        pSystem_ = nullptr;
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type: " << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
}

void BaumerManager::searchCamera()
{
    try
    {
        qDebug() << "process in search";
        BGAPI2::InterfaceList *interface_list = pSystem_->GetInterfaces();
        interface_list->Refresh(100);
        for (BGAPI2::InterfaceList::iterator ifc_iter = interface_list->begin();
             ifc_iter != interface_list->end();
             ifc_iter++)
        {
            qDebug() << "process in 0";
            if (ifc_iter->second->IsOpen())
            {
                BGAPI2::DeviceList *dev_list = ifc_iter->second->GetDevices();
                dev_list->Refresh(100);
                qDebug() << "process in 1, size: " << dev_list->size();
                // qDebug() << "device listsdev_list
                // size:" << dev_list->size(); dev_list->Refresh(100);
                for (BGAPI2::DeviceList::iterator device_iter =
                         dev_list->begin();
                     device_iter != dev_list->end();
                     device_iter++)
                {
                    qDebug() << "process in 2";
                    BGAPI2::NodeMap *pDeviceNodeMap =
                        device_iter->second->GetNodeList();
                    std::string number =
                        device_iter->second->GetSerialNumber().get();
                    if (forceIP(number, pDeviceNodeMap))
                    {
                        // continue;
                    }
                    std::string status =
                        device_iter->second->GetAccessStatus().get();
                    if (status == "RW" && !device_iter->second->IsOpen())
                    {
                        qDebug() << "process in add:" << number;
                        addCamera(number, device_iter->second);
                        qDebug() << "add camera success SNNumber:" << number;
                    }
                }

                // for (BGAPI2::DeviceList::iterator dev_iter =
                // dev_list->begin();
                //      dev_iter != dev_list->end();
                //      dev_iter++)
                // {
                //     qDebug() << "InterfaceList::iterator process in 2";
                //     BGAPI2::NodeMap *pDevNodeMap =
                //         dev_iter->second->GetNodeList();
                //     std::string SNNumber =
                //         dev_iter->second->GetSerialNumber().get();
                //     qDebug() << "current sn:" << SNNumber;
                //     if (forceIP(SNNumber, pDevNodeMap))
                //     {
                //         continue;
                //     }
                //     std::string status =
                //         dev_iter->second->GetAccessStatus().get();
                //     if (status == "RW" && !dev_iter->second->IsOpen())
                //     {
                //         addCamera(SNNumber, dev_iter->second);
                //         qDebug() << "add camera success SNNumber:" <<
                //         SNNumber;
                //     }
                // }
            }
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type: " << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
}

bool BaumerManager::forceIP(const std::string &snNumber,
                            BGAPI2::NodeMap *nodeMap)
{
    bool ret = false;
    uint8_t index = 0;
    for (; index < nodeParams_.size(); index++)
    {
        if (nodeParams_[index]["sn_number"].as<std::string>() == snNumber)
        {
            try
            {
                std::string subNetIp =
                    nodeMap->GetNode("GevDeviceIPAddress")->GetValue().get();
                qDebug() << "subNetIp:" << subNetIp;
                std::string subNetMask =
                    nodeMap->GetNode("GevDeviceSubnetMask")->GetValue().get();
                qDebug() << "subNetMask:" << subNetMask;
                std::string ip = nodeParams_[index]["ip"].as<std::string>();
                qDebug() << "ip:" << ip;
                std::string mask = nodeParams_[index]["mask"].as<std::string>();
                qDebug() << "mask" << mask;
                if (ip != subNetIp || mask != subNetMask)
                {
                    bo_int64 iDeviceMacAddress =
                        nodeMap->GetNode("GevDeviceMACAddress")->GetInt();
                    nodeMap->GetNode("MACAddressNeededToForce")
                        ->SetInt(iDeviceMacAddress);
                    nodeMap->GetNode("ForcedIPAddress")->SetValue(ip.c_str());
                    nodeMap->GetNode("ForcedSubnetMask")
                        ->SetValue(mask.c_str());
                    nodeMap->GetNode("ForcedGateway")
                        ->SetInt(u32InterfaceIp_ & u32InterfaceMask_);
                    nodeMap->GetNode("ForceIP")->Execute();
                    ret = true;
                }
            }
            catch (BGAPI2::Exceptions::IException &ex)
            {
                qDebug() << "Error Type: " << ex.GetType().get();
                qDebug() << "Error function: " << ex.GetFunctionName().get();
                qDebug() << "Error description: "
                         << ex.GetErrorDescription().get();
            }
        }
    }
    return ret;
}
void BaumerManager::captureThreadFunc()
{

    while (bCaptureRunning_)
    {
        captureImageStart();                                        // 获取图像
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 控制帧率
    }
}
void BaumerManager::startCaptureThread()
{

    if (!bCaptureRunning_)
    {
        bCaptureRunning_ = true;
        captureThread_ = std::thread(&BaumerManager::captureThreadFunc, this);
    }
}
void BaumerManager::stopCaptureThread()
{
    if (bCaptureRunning_)
    {
        bCaptureRunning_ = false; // 停止线程循环
        if (captureThread_.joinable())
        {
            captureThread_.join(); // 等待线程结束
        }
    }
}
bool BaumerManager::addCamera(const std::string &snNumber, BGAPI2::Device *dev)
{

    bool ret = false;

    for (const auto &group : nodeParams_)
    {
        if (group["sn_number"] &&
            snNumber == group["sn_number"].as<std::string>())
        {
            std::unique_ptr<BaumerCamera> camera_obj =
                std::make_unique<BaumerCamera>(dev, group);
            camera_obj->initialize();
            if (!camera_obj->getInitialized())
            {
                return false;
            }
            camera_obj->setSnStr(snNumber);
            // QObject::connect(camera_obj.get(),
            //                  &BaumerCamera::sendImage,
            //                  this,
            //                  []() { emit });
            mapCameras_[snNumber] = std::move(camera_obj);
            BGAPI2::NodeMap *node = dev->GetNodeList();
            qDebug() << "add camera success sn:" << snNumber << "ip:"
                     << node->GetNode("GevDeviceIPAddress")->GetValue().get();
            ret = true;
            break;
        }
    }

    return ret;
}
void BaumerManager::stop()
{
    bHold_ = false;

    // timerSearch_->stop();
    // mapCameras_.clear();
    deinitializeBGAPI();
}

void BaumerManager::captureImageStart()
{
    // std::unique_lock<std::mutex> lock(mtxCamera_);
    for (auto &[key, cam] : mapCameras_)
    {
        auto img = cam->getCurrentImage();
        if (!img.isNull())
        {
            emit signalsImgGet(key, img);
        }
    }
}

void BaumerManager::recvImage(const std::string &snNunber, const QImage &img)
{
    std::lock_guard<std::mutex> lock(mtxCamera_);

    m_mapImage[snNunber].push(img);
}