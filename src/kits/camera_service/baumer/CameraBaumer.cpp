#include "CameraBaumer.h"
// #include "bgapi2_genicam.hpp"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <qcontainerfwd.h>
#include <qlogging.h>

namespace _Kits
{
    using namespace BGAPI2;

    bool CameraBaumer::mb_isinit = false;

    void BGAPI2CALL CameraBaumer::PnPEventHandler(void *callBackOwner, BGAPI2::Events::PnPEvent *pPnPEvent)
    {

        if (NULL != pPnPEvent && NULL != callBackOwner)
        {
            CameraBaumer *pCameraBaumer = static_cast<CameraBaumer *>(callBackOwner);
            std::string EventTypeStr = ((pPnPEvent->GetPnPType() == 0) ? "removed" : "added");
            std::string SNNumber = pPnPEvent->GetSerialNumber().get();
            if (EventTypeStr == "removed")
            {
                // 回调删除出错相机
                qDebug() << "camera remove singal: " << SNNumber;
                // pCameraBaumer->removeCamera(SNNumber);
            }
            else if (EventTypeStr == "added")
            {
                //   改为主动添加
                qDebug() << "camera add  singal" << SNNumber;
                pCameraBaumer->searchCamera(SNNumber);
            }
        }
    }

    CameraBaumer::CameraBaumer(QObject *parent)
        : CameraBase(parent), m_systems(nullptr), m_device(nullptr), m_stream(nullptr), m_bConnected(false), m_bStreaming(false)
    {
    }

    CameraBaumer::~CameraBaumer()
    {
        closeCamera();
    }

    bool CameraBaumer::openCamera(const YAML::Node &config)
    {
        if (!ReadConfig(config))
        {
            qDebug() << "camera config error";
            return false;
        }

        try
        {
            //    for(int i =0;i<4;i++)
            //    {
            //         if(initializeSdk())
            //         {
            //             if(m_bConnected==true)
            //             {
            //                 break;
            //             }
            //         }

            //    }
            // 初始化SDK
            for (int i = 0; i < 3; i++)
            {
                if (initializeSdk())
                {
                    if (m_device != nullptr)
                    {
                        break;
                    }
                }
            }

            // if (!initializeSdk())
            // {
            //     qDebug() << "Baumer SDK init error";
            //     return false;
            // }

            // if (!initializeSdk())
            // {
            //     return false;
            // }
            // 获取数据流
            if (openDataStream())
            {
                // if(!startGrab())
                // {
                //     qDebug() << "start grab error";
                //     return false;
                // }
                bOpen_ = true;
                return true;
            }
            else
            {
                qDebug() << "open data stream error";
                return false;
            }

            //  m_thread_cachevideo = std::thread(&CameraBaumer::CacheVideo, this);
            // connect(this, &CameraBaumer::imageReady, this, &CameraBaumer::OnImageReady);

            return true;
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Baumer camera error:" << QString::fromStdString(ex.GetErrorDescription().get());
            return false;
        }
    }

    bool CameraBaumer::startGrab()
    {
        try
        {
            if (m_device->GetRemoteNode(SFNC_ACQUISITION_START)->IsWriteable())
            {
                m_device->GetRemoteNode(SFNC_ACQUISITION_START)->Execute();
                m_bStreaming = true;
                m_bIsGrabbing = true;
            }

            return true;
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Baumer camera error:" << ex.GetErrorDescription().get();
            return false;
        }
    }

    void CameraBaumer::ImageCallback(void *callbackOwner, BGAPI2::Buffer *pBuffer)
    {
        CameraBaumer *camera = static_cast<CameraBaumer *>(callbackOwner);
        if (camera == nullptr || pBuffer == NULL)
        {
            return;
        }
        try
        {
            if (pBuffer->GetIsIncomplete())
            {
                // pBuffer->QueueBuffer();
                qDebug() << "image is incomplete";
            }
            else
            {

                int width = static_cast<int>(pBuffer->GetWidth());
                int height = static_cast<int>(pBuffer->GetHeight());
                // 转换QImage并发送信号
                void *pRawBuffer = pBuffer->GetMemPtr();
                QImage image(static_cast<uchar *>(pRawBuffer), width, height, QImage::Format_Grayscale8);
                QImage imageCopy = image.copy();
                // QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");
                // QString fileName = QString("image_%1.jpg").arg(timestamp);
                // imageCopy.save(fileName);
                emit camera->ImageOutPut(imageCopy, camera->m_cameraInfo.mstr_CameraName);
                // emit camera->imageReady(image, camera->m_cameraInfo.mstr_CameraName);
            }
            // 重新入队缓冲区以接收新数据
            pBuffer->QueueBuffer();
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Baumer callback error:" << ex.GetErrorDescription().get();
            // 即使出错也尝试重新入队
            // pBuffer->QueueBuffer();
        }
    }

    bool CameraBaumer::getImage()
    {
        if (!m_bStreaming)
        {
            return false;
        }

        try
        {
            BGAPI2::Buffer *pBufferFilled = m_stream->GetFilledBuffer(1000);

            if (pBufferFilled != nullptr)
            {
                // 将图像数据转换为QImage
                QImage image((unsigned char *)pBufferFilled->GetMemPtr(),
                             pBufferFilled->GetWidth(),
                             pBufferFilled->GetHeight(),
                             QImage::Format_Grayscale8);

                emit ImageOutPut(image, m_cameraInfo.mstr_CameraName);

                return true;
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Baumer camera error:" << QString::fromStdString(ex.GetErrorDescription().get());
        }

        return false;
    }

    void CameraBaumer::ImageHeightWidth()
    {
        if (m_device)
        {
            m_imgInfo.m_img_width = m_device->GetRemoteNode("Width")->GetInt();
            m_imgInfo.m_img_height = m_device->GetRemoteNode("Height")->GetInt();
            emit imgInfoOut(m_imgInfo);
        }
    }

    // 其他必要的实现...
    // 根据实际需求实现其他虚函数
    // 初始化SDK
    bool CameraBaumer::initializeSdk()
    {
        try
        {
            //     if (CameraBaumer::mb_isinit)
            //     {
            //         return true;
            //     }

            //     // 获取系统列表实例
            //     m_systems = BGAPI2::SystemList::GetInstance();
            //     // 刷新系统列表
            //     m_systems->Refresh();

            //     bool isfindDevice = false;

            //     int i = 0;

            //     for (BGAPI2::SystemList::iterator sys_iter = m_systems->begin(); sys_iter != m_systems->end(); sys_iter++)
            //     {
            //         BGAPI2::System *system_pointer = sys_iter->second;
            //         BGAPI2::String tl_type = system_pointer->GetTLType();

            //         if (tl_type != "GEV")
            //         {
            //             continue;
            //         }
            //         // if (!system_pointer->IsOpen())
            //         // {
            //         system_pointer->Open();
            //         // }
            //         if (i++ > 1)
            //         {
            //             continue;
            //         }

            //         m_system = system_pointer;
            //         m_interfaces = system_pointer->GetInterfaces();

            //         m_interfaces->Refresh(100);

            //         for (BGAPI2::InterfaceList::iterator ifc_iter = m_interfaces->begin(); ifc_iter != m_interfaces->end(); ifc_iter++)
            //         {
            //             if (!ifc_iter->second->IsOpen())
            //             {
            //                 ifc_iter->second->Open();
            //             }
            //             std::string name = ifc_iter->GetDisplayName().get();
            //             std::string ip = ifc_iter->second->GetNode("GevInterfaceSubnetIPAddress")->GetValue().get();
            //             std::string mac = ifc_iter->second->GetNode("GevInterfaceMACAddress")->GetValue().get();
            //             std::string mask = ifc_iter->second->GetNode("GevInterfaceSubnetMask")->GetValue().get();

            //             // 为相机注册掉线触发
            //             ifc_iter->second->RegisterPnPEvent(BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
            //             ifc_iter->second->RegisterPnPEventHandler(this, (BGAPI2::Events::PnPEventHandler)&PnPEventHandler);
            //             qDebug() << "CAMERA FOUNDED ";
            //             qDebug() << "ip:" << ip;
            //             qDebug() << "mac:" << mac;
            //             qDebug() << "mask:" << mask;
            //         }
            //     }

            //     CameraBaumer::mb_isinit = true;
            //     return true;
            // }
            // catch (BGAPI2::Exceptions::IException &ex)
            // {
            //     qDebug() << "Baumer camera error:" << QString::fromStdString(ex.GetErrorDescription().get());
            //     qDebug() << "camera Error Type :" << QString::fromStdString(ex.GetType().get());

            //     qDebug() << "camera Error function:" << QString::fromStdString(ex.GetFunctionName().get());

            //     return false;
            // }

            BGAPI2::SystemList *system_list = BGAPI2::SystemList::GetInstance();
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
                m_systems = pSystem;
                initInterface();
                qDebug() << "baumer system init successed.";
            }
            if (m_systems == nullptr)
            {
                qDebug() << "baumer system init failed.";
                return false;
            }
            return true;
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Error Type:" << ex.GetType().get();
            qDebug() << "Error function: " << ex.GetFunctionName().get();
            qDebug() << "Error description: " << ex.GetErrorDescription().get();
            if (m_systems == nullptr)
            {
                BGAPI2::SystemList::ReleaseInstance();
            }
            return false;
        }
    }
    void CameraBaumer::initInterface()
    {
        try
        {
            BGAPI2::InterfaceList *ifl = m_systems->GetInterfaces();
            ifl->Refresh(1000);
            for (auto iter = ifl->begin(); iter != ifl->end(); iter++)
            {
                auto ifc = iter->second;
                if (ifc->IsOpen())
                {
                    continue;
                }

                ifc->Open();
                std::string name = ifc->GetDisplayName().get();
                std::string ip = ifc->GetNode("GevInterfaceSubnetIPAddress")->GetValue().get();
                std::string mac = ifc->GetNode("GevInterfaceMACAddress")->GetValue().get();
                std::string mask = ifc->GetNode("GevInterfaceSubnetMask")->GetValue().get();

                ifc->second->RegisterPnPEvent(BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
                ifc->second->RegisterPnPEventHandler(this, &CameraBaumer::PnPEventHandler);
                qDebug() << "init baumer interface founded: ";
                qDebug() << "founded ip: " << ip;
                qDebug() << "founded MAC: " << mac;
                qDebug() << "founded mask: " << mask;
                std::string needSN = m_cameraInfo.mstr_Camera_ID;
                BGAPI2::DeviceList *device_list = ifc->GetDevices();
                device_list->Refresh(1000);
                qDebug() << "device lists size:" << device_list->size();

                for (BGAPI2::DeviceList::iterator device_iter = device_list->begin(); device_iter != device_list->end(); device_iter++)
                {
                    auto dev = device_iter->second;
                    std::string SNnumber = dev->GetSerialNumber().get();
                    std::string status = dev->GetAccessStatus().get();
                    qDebug() << "SNnumber" << SNnumber;
                    qDebug() << "status" << status;
                    if (needSN.empty())
                    {
                        continue;
                    }
                    if (SNnumber == needSN && status == "RW")
                    {
                        // 找到相机
                        m_device = dev;
                        m_bConnected = true;
                        // mb_isinit = true;
                        qDebug() << "camera founded sn:" << SNnumber;
                        break;
                    }
                }

                // if(searchCamera(needSN))
                // {
                //     qDebug() << "camera found with SN:" << QString::fromStdString(needSN);
                // }

                // m_bInit = true;
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Error Type:" << ex.GetType().get();
            qDebug() << "Error function: " << ex.GetFunctionName().get();
            qDebug() << "Error description: " << ex.GetErrorDescription().get();
        }
    }
    // 查找并打开相机
    bool CameraBaumer::searchCamera(const std::string &needNumber)
    {
        try
        {
            BGAPI2::InterfaceList *interface_list = m_systems->GetInterfaces();
            interface_list->Refresh(1000);
            for (BGAPI2::InterfaceList::iterator ifc_iter = interface_list->begin(); ifc_iter != interface_list->end(); ifc_iter++)
            {
                auto ifc = ifc_iter->second;
                if (!ifc->IsOpen())
                {
                    continue;
                }
                BGAPI2::DeviceList *device_list = ifc->GetDevices();
                device_list->Refresh(1000);
                qDebug() << "device lists size:" << device_list->size();

                for (BGAPI2::DeviceList::iterator device_iter = device_list->begin(); device_iter != device_list->end(); device_iter++)
                {
                    auto dev = device_iter->second;
                    std::string SNnumber = dev->GetSerialNumber().get();
                    std::string status = dev->GetAccessStatus().get();
                    qDebug() << "SNnumber" << SNnumber;
                    qDebug() << "status" << status;
                    if (needNumber.empty())
                    {
                        continue;
                    }
                    if (SNnumber == needNumber && status == "RW")
                    {
                        // 找到相机
                        m_device = dev;
                        m_bConnected = true;
                        // mb_isinit = true;
                        qDebug() << "camera founded sn:" << SNnumber;
                        continue;
                    }
                }
            }
            // 仅连接一个相机
            if (m_device == nullptr)
            {
                qDebug() << "camera not found";
                return false;
            }
            return true;
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            if (m_device != nullptr)
            {
                m_device->Close();
                m_device = nullptr;
                // pInterface->Close();
            }
            m_systems->Close();
            m_systems = nullptr;
            BGAPI2::SystemList::ReleaseInstance();
            qDebug() << "Baumer camera error:" << QString::fromStdString(ex.GetErrorDescription().get());
            return false;
        }
    }
    bool CameraBaumer::openDataStream()
    {
        bool ret = false;
        try
        {
            if (m_device == nullptr)
            {
                // searchCamera(m_cameraInfo.mstr_Camera_ID);
                qDebug() << "device is null";
                return false;
            }
            m_device->Open();
        }
        catch (BGAPI2::Exceptions::ResourceInUseException &ex)
        {
            qDebug() << "ResourceInUseException" << ex.GetErrorDescription().get();
        }
        catch (BGAPI2::Exceptions::AccessDeniedException &ex)
        {
            qDebug() << "AccessDeniedException" << ex.GetErrorDescription().get();
        }
        configureCamera();
        try
        {
            auto datastream_list = m_device->GetDataStreams();
            datastream_list->Refresh();
            qDebug() << "datastream_list size" << datastream_list->size();
            if (datastream_list->size() > 0)
            {
                m_stream = datastream_list->begin()->second;
                m_stream->Open();
                addBufferToStream();
                m_stream->RegisterNewBufferEvent(BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
                m_stream->RegisterNewBufferEventHandler(this, (BGAPI2::Events::NewBufferEventHandler)&ImageCallback);
                m_stream->StartAcquisitionContinuous();
                m_bStreaming = true;
                ret = true;
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            ret = false;
            qDebug() << "error Description:" << ex.GetErrorDescription().get();
            qDebug() << "error FunctionName:" << ex.GetFunctionName().get();
            qDebug() << "error Type:" << ex.GetType().get();
        }
        return ret;
    }
    void CameraBaumer::addBufferToStream()
    {

        try
        {
            // m_device->GetDataStreams()->Refresh();
            BGAPI2::BufferList *bufferList = m_stream->GetBufferList();
            for (int i = 0; i < 5; i++)
            {
                BGAPI2::Buffer *buffer = new BGAPI2::Buffer();
                bufferList->Add(buffer);
                // buffer->QueueBuffer();
                streamBuffers_.push_back(buffer);
            }

            for (BGAPI2::BufferList::iterator bufIterator = bufferList->begin(); bufIterator != bufferList->end(); bufIterator++)
            {
                bufIterator->QueueBuffer();
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "error Description" << ex.GetErrorDescription().get();
            qDebug() << "error functionname" << ex.GetFunctionName().get();
            qDebug() << "error type " << ex.GetType().get();
        }
    }

    void CameraBaumer::storeImg(BGAPI2::Buffer *pBufferFilled)
    {
    }
    bool CameraBaumer::ReadConfig(const YAML::Node &config)
    {

        try
        {
            m_cameraInfo.mstr_Camera_Type = config["camera_type"].as<std::string>();
            m_cameraInfo.mstr_Camera_ID = config["camera_ip"].as<std::string>();
            m_cameraInfo.mus_CameraLeft = config["camera_left"].as<unsigned short>();
            m_cameraInfo.mus_CameraRight = config["camera_right"].as<unsigned short>();
            m_cameraInfo.mus_CameraTop = config["camera_top"].as<unsigned short>();
            m_cameraInfo.mus_CameraBottom = config["camera_bottom"].as<unsigned short>();
            m_cameraInfo.mus_CameraGain = config["camera_gain"].as<unsigned short>();
            m_cameraInfo.mus_CameraExposure = config["camera_exposure"].as<unsigned short>();
            m_cameraInfo.mus_CameraFps = config["camera_fps"].as<unsigned short>();
            m_cameraInfo.mi_CameraPixelFormat = static_cast<PixelFormat>(config["camera_pixelformat"].as<int>());
            m_cameraInfo.mus_CameraPort = config["camera_port"].as<unsigned short>();
            m_cameraInfo.mstr_CameraPassword = config["camera_pwd"].as<std::string>();
            m_cameraInfo.mstr_CameraUserName = config["camera_username"].as<std::string>();
            m_cameraInfo.mstr_CameraName = config["camera_name"].as<std::string>();
            m_cameraInfo.mus_IsShowImg = static_cast<ReturnCode>(config["isshow_img"].as<int>());
            m_cameraInfo.mi_IsShowData = static_cast<ReturnCode>(config["isshow_data"].as<int>());
            m_cameraInfo.mstr_SaveImageType = (config["save_img_type"].as<std::string>());
            m_cameraInfo.mi_SaveImageMod = static_cast<PicSaveType>(config["save_img_mod"].as<int>());
            m_cameraInfo.mui8_Channel = config["channel"].as<unsigned char>();
            m_cameraInfo.mstr_SaveVideoType = config["save_video_type"].as<std::string>();
            m_cameraInfo.mstr_Video_SavePath = config["save_video_path"].as<std::string>();
        }
        catch (const YAML::Exception &e)
        {
            qDebug() << "YAML config error:" << e.what();
            return false;
        }

        return true;
    }

    // 配置相机参数
    void CameraBaumer::configureCamera()
    {
        try
        {
            // 曝光
            double expose = static_cast<double>(m_cameraInfo.mus_CameraExposure);
            // 增益
            int gain = static_cast<int>(m_cameraInfo.mus_CameraGain);

            // int frameRate = static_cast<int>(m_cameraInfo.mus_CameraFps);
            //  设置触发模式为关闭(连续采集)
            qDebug() << "triggermode value" << m_device->GetRemoteNode(SFNC_TRIGGERMODE)->GetValue();
            BGAPI2::String triggerMode_ = m_device->GetRemoteNode(SFNC_TRIGGERMODE)->GetValue();
            if (triggerMode_ != "Off")
            {
                m_device->GetRemoteNode(SFNC_TRIGGERMODE)->SetValue("Off");
            }

            qDebug() << "dev status 3" << m_device->GetAccessStatus();

            // 设置曝光时间
            if (m_cameraInfo.mus_CameraExposure > 0)
            {
                qDebug() << "SFNC_EXPOSUREAUTO:" << m_device->GetRemoteNode(SFNC_EXPOSUREAUTO)->GetValue();
                m_device->GetRemoteNode(SFNC_EXPOSUREAUTO)->SetValue("Off");
                m_device->GetRemoteNode("ExposureTime")->SetDouble(expose);
            }

            // 设置增益
            m_device->GetRemoteNode(SFNC_GAINAUTO)->SetString("Off");
            m_device->GetRemoteNode(SFNC_GAIN)->SetInt(gain);

            // 设置帧率
            // if (m_cameraInfo.mus_CameraFps > 0)
            // {
            //     m_device->GetRemoteNode(SFNC_ACQUISITION_FRAMERATE)
            //         ->SetInt(frameRate);
            // }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Baumer camera error:" << QString::fromStdString(ex.GetErrorDescription().get());
            qDebug() << "error function" << ex.GetFunctionName().get();
            qDebug() << "error type" << ex.GetType().get();
        }
    }

    void CameraBaumer::clearBuffersFromStream()
    {
        try
        {
            if (m_stream != nullptr)
            {
                m_stream->StopAcquisition();
                auto bufferList = m_stream->GetBufferList();
                bufferList->DiscardAllBuffers();
                while (bufferList->size() > 0)
                {
                    BGAPI2::Buffer *buffer = bufferList->begin()->second;
                    bufferList->RevokeBuffer(buffer);
                    delete buffer;
                }
                m_stream->UnregisterNewBufferEvent();
                m_stream->RegisterNewBufferEvent(Events::EVENTMODE_UNREGISTERED);
                m_stream->Close();

                m_bStreaming = false;
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "camera error Description " << ex.GetErrorDescription().get();
            qDebug() << "camera error FunctionName " << ex.GetFunctionName().get();
            qDebug() << "camera error Type " << ex.GetType().get();
        }
    }
    void CameraBaumer::stopDev()
    {
        if (m_device == nullptr)
        {
            return;
        }
        try
        {
            m_device->Close();
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "camera error GetErrorDescription:" << ex.GetErrorDescription().get();
            qDebug() << "camera error GetFunctionName:" << ex.GetFunctionName().get();
            qDebug() << "camera error GetType" << ex.GetType().get();
        }
    }
    // 释放SDK资源
    void CameraBaumer::releaseSdk()
    {
        try
        {
            if (m_systems != nullptr)
            {
                BGAPI2::InterfaceList *interfaceList = m_systems->GetInterfaces();
                for (BGAPI2::InterfaceList::iterator ifc_iter = interfaceList->begin(); ifc_iter != interfaceList->end(); ifc_iter++)
                {
                    if (ifc_iter->second->IsOpen())
                    {
                        ifc_iter->second->UnregisterPnPEvent();
                        ifc_iter->second->Close();
                    }
                }
                m_systems->Close();
            }
            BGAPI2::SystemList::ReleaseInstance();
            //     if(m_stream!=nullptr)
            //     {
            //         m_stream = nullptr;
            //     }
            //    if (m_device!=nullptr)
            //    {
            //     m_device = nullptr;
            //    }
            //     m_systems = nullptr;
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Baumer camera error:" << QString::fromStdString(ex.GetErrorDescription().get());
        }
    }

    bool CameraBaumer::closeCamera()
    {
        // if (!m_bConnected)
        // {
        //     return true;
        // }

        stopGrab();
        clearBuffersFromStream();
        stopDev();
        // for (auto &it : streamBuffers_)
        // {
        //     delete it;
        // }
        // streamBuffers_.clear();
        releaseSdk();
        m_bConnected = false;
        return true;
    }

    bool CameraBaumer::stopGrab()
    {
        if (!m_bStreaming)
        {
            return true;
        }

        try
        {
            //----------------VCXG 13M堡盟相机 没这个节点，直接停
            // if(m_device->GetRemoteNodeList()->GetNodePresent("AcquisitionAbort"))
            // {
            //     m_device->GetRemoteNode("AcquisitionAbort")->Execute();
            //     qDebug() << "AcquisitionAbort executed";
            // }

            // auto node = m_device->GetRemoteNode("AcquisitionAbort");
            // if (node->IsWriteable())
            // {
            //     node->Execute();
            // }
            // else
            // {
            //     qDebug() << "AcquisitionAbort node is not writable";
            // }
            // //m_device->GetRemoteNode("AcquisitionAbort")->Execute();
            qDebug() << "AcquisitionAbort executed";

            m_device->GetRemoteNode("AcquisitionStop")->Execute();

            qDebug() << "AcquisitionAbort executed 2";
            // m_device->GetRemoteNode("AcquisitionStop")->Execute();
            // m_bStreaming = false;

            // bufferList->FlushAllToInputQueue();
            m_bIsGrabbing = false;
            return true;
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Baumer camera error:" << QString::fromStdString(ex.GetErrorDescription().get());
            return false;
        }
    }

    std::string CameraBaumer::getCameraName()
    {
        return m_cameraInfo.mstr_CameraName;
    }

    std::chrono::steady_clock::time_point CameraBaumer::convertToChronoTime(const DownloadTime &dt)
    {
        using namespace std::chrono;
        tm timeInfo = {0};
        timeInfo.tm_year = dt.dwYear - 1900; // tm_year is years since 1900
        timeInfo.tm_mon = dt.dwMonth - 1;    // tm_mon is 0-based
        timeInfo.tm_mday = dt.dwDay;
        timeInfo.tm_hour = dt.dwHour;
        timeInfo.tm_min = dt.dwMinute;
        timeInfo.tm_sec = dt.dwSecond;

        auto tp = system_clock::from_time_t(mktime(&timeInfo));
        return steady_clock::now() + (tp - system_clock::now());
    }

    bool CameraBaumer::downloadRecord(const DownloadTimeRange &timeRange, const std::string &savePath)
    {
        auto startTime = convertToChronoTime(timeRange.startTime);
        auto endTime = convertToChronoTime(timeRange.endTime);

        int duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
        int itotalFps = m_cameraInfo.mus_CameraFps * duration;

        ConcurrentPool::runTask([this, timeRange, savePath, itotalFps]() {
            // std::lock_guard<std::mutex> lock(m_mutex_cachevideo);
            //  auto node_cachevideo = m_node_cachevideo;
            //  for (; m_node_cachevideo != node_cachevideo; m_node_cachevideo = m_node_cachevideo->m_next)
            //  {
            //      if (m_node_cachevideo->m_node->mb_isWorking == false)
            //      {

            //         m_node_cachevideo->m_node->mb_isWorking = true;

            //         auto img = m_queue_cachevideo.back().copy();

            //         for(int i = 0; i < itotalFps; i++)
            //         {
            //             if (img == m_queue_cachevideo.back())
            //             {
            //                 i--;
            //                 continue;
            //             }

            //             img = m_queue_cachevideo.back().copy();
            //             m_node_cachevideo->m_node->m_encodeVideo->WriteRGBData(img.constBits() ,
            //                                                                     img.width() * img.height() ,
            //                                                                     AV_PIX_FMT_RGB24);

            //         }

            //         m_node_cachevideo->m_node->mb_isWorking = false;

            //     }

            // }
        });

        return false;
    }
    void CameraBaumer::removeCamera(const std::string &snNumber)
    {
    }
    // void CameraBaumer::OnImageReady(const QImage &image,
    //                                 const std::string &camera_name)
    // {
    //     if(mb_cachevideo)
    //     {
    //         m_queue_cachevideo.push_back(image.copy());
    //     }

    //     auto imageUse = image.copy();
    //     if (nullptr != m_encodeVideo)
    //     {
    //         m_encodeVideo->WriteRGBData(imageUse.constBits() ,imageUse.width() * imageUse.height() ,AV_PIX_FMT_GRAY8);
    //     }
    // }

    // void CameraBaumer::writeVideofunc(int no, AVPacket &pkt, int flag)
    // {
    //     if (nullptr != m_muxingVideo)
    //     {
    //         m_muxingVideo->writeVideo(0, pkt, 0);
    //     }
    // }

    void CameraBaumer::startcachevideo()
    {
        // if (m_bIsRecording)
        // {
        //     mb_cachevideo = true;
        // }
    }

    void CameraBaumer::CacheVideo()
    {
        // mi_cachevideo_imagesave_size_before = m_cameraInfo.mus_CameraFps * m_cameraInfo.mi_cacheforvideo_timesize_before;

        // while (mb_cachevideo)
        // {
        //     std::unique_lock<std::mutex> lock(m_mutex_cachevideo);
        //     m_cond_cachevideo.wait(lock);
        //     if(!mb_cachevideo || m_queue_cachevideo.size() < 0)
        //     {
        //         break;
        //     }

        //     if (m_queue_cachevideo.size() > mi_cachevideo_imagesave_size_before)
        //     {
        //         m_queue_cachevideo.pop_back();
        //     }

        // }
    }

} // namespace _Kits