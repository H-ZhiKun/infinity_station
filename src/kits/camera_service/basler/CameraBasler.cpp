#include "CameraBasler.h"
#include <QDebug>

namespace _Kits
{

    CameraBasler::CameraBasler(QObject *parent) : CameraBase(parent), m_camera(nullptr)
    {
#ifdef _WIN32
        Pylon::PylonInitialize();
#elif defined(__linux__) && defined(__aarch64__)
        PylonInitialize();
#endif
    }

    CameraBasler::~CameraBasler()
    {
        closeCamera();
#ifdef _WIN32
        Pylon::PylonTerminate();
#elif defined(__linux__) && defined(__aarch64__)
        PylonTerminate();
#endif
    }

    bool CameraBasler::ReadConfig(const YAML::Node &config)
    {
        try
        {
            // 读取基本配置
            m_cameraInfo.mstr_Camera_Type = config["camera_type"].as<std::string>();
            m_cameraInfo.mstr_Camera_ID = config["camera_ip"].as<std::string>();
            m_cameraInfo.mus_CameraGain = config["camera_gain"].as<unsigned short>();
            m_cameraInfo.mus_CameraExposure = config["camera_exposure"].as<unsigned short>();
            m_cameraInfo.mus_CameraFps = config["camera_fps"].as<unsigned short>();
            m_cameraInfo.mstr_CameraName = config["camera_name"].as<std::string>();
            // 读取Basler特有配置
            m_serialNumber = config["serial_number"].as<std::string>();
            m_isColorCamera = config["is_color"].as<bool>();
            m_gainRaw = config["gain_raw"].as<double>();
            m_exposureTime = config["exposure_time"].as<double>();
            m_pixelFormat = config["pixel_format"].as<std::string>().c_str();
            m_reverseX = config["reverse_x"].as<bool>();
            m_reverseY = config["reverse_y"].as<bool>();
            m_istrigger = config["istrigger"].as<bool>();
            m_triggermode = config["triggermode"].as<std::string>();

            return true;
        }
        catch (const YAML::Exception &e)
        {
            qDebug() << "配置文件解析错误:" << e.what();
            return false;
        }
    }

    bool CameraBasler::openCamera(const YAML::Node &config)
    {
        if (!ReadConfig(config))
        {
            return false;
        }

        try
        {
#ifdef _WIN32
            // Windows平台实现
            m_camera = new Pylon::CBaslerUniversalInstantCamera;
            Pylon::CTlFactory &tlFactory = Pylon::CTlFactory::GetInstance();
            Pylon::DeviceInfoList_t devices;
            tlFactory.EnumerateDevices(devices);
            int numde = devices.size();
            if (!m_serialNumber.empty())
            {
                m_camera->Attach(
                    Pylon::CTlFactory::GetInstance().CreateFirstDevice(Pylon::CDeviceInfo().SetSerialNumber(m_serialNumber.c_str())));
                if (m_camera->IsGrabbing())
                {
                    m_camera->StopGrabbing();
                }
                if (m_camera->IsOpen())
                {
                    m_camera->Close();
                }
            }
            else
            {
                return false;
                // m_camera->Attach(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
            }

            m_camera->Open();
            if (m_camera->IsOpen())
            {
                {
                    GenApi::INodeMap &cameraNodeMap = m_camera->GetNodeMap();
                    GenApi::CEnumerationPtr ptrTriggerSel = cameraNodeMap.GetNode("TriggerSelector");
                    GenApi::CIntegerPtr heartbeatTimeout = cameraNodeMap.GetNode("GevHeartbeatTimeout");
                    ptrTriggerSel->FromString("FrameStart");
                    GenApi::CEnumerationPtr ptrTrigger = cameraNodeMap.GetNode("TriggerMode");
                    GenApi::CEnumerationPtr ptrTriggerSource = cameraNodeMap.GetNode("TriggerSource");
                    GenApi::CEnumerationPtr ptrTriggerActivation = cameraNodeMap.GetNode("TriggerActivation");
                    GenApi::CEnumerationPtr ptrTriggerSelector = cameraNodeMap.GetNode("TriggerSelector");
                    ptrTrigger->SetIntValue(m_istrigger ? 1 : 0); // freerun
                    if (m_istrigger)
                    {
                        ptrTriggerSelector->FromString("FrameStart");
                        if (m_triggermode == "Line1")
                        {
                            ptrTriggerSource->FromString("Line1");
                        }
                        else if (m_triggermode == "Line2")
                        {
                            ptrTriggerSource->FromString("Line2");
                        }
                        else if (m_triggermode == "Line3")
                        {
                            ptrTriggerSource->FromString("Line3");
                        }
                    }
                    heartbeatTimeout->SetValue(3000);
                }
                ConfigureCamera();
                return true;
            }

            // 设置触发模式，心跳时间
            // //获取当前触发模式
            // {
            //     INodeMap &cameraNodeMap = m_basler.GetNodeMap();
            //     CEnumerationPtr  ptrTriggerSel = cameraNodeMap.GetNode ("TriggerSelector");
            //     ptrTriggerSel->FromString("FrameStart");
            //     CEnumerationPtr  ptrTrigger  = cameraNodeMap.GetNode ("TriggerMode");
            //     return ptrTrigger->GetIntValue() == 1;
            // }

#elif defined(__linux__) && defined(__aarch64__)
            // ARM Linux平台实现
            GENAPIC_RESULT result;
            size_t numDevices = 0;

            // 枚举设备
            result = PylonEnumerateDevices(&numDevices);
            if (result != GENAPI_E_OK || numDevices == 0)
            {
                qDebug() << "未找到相机设备";
                return false;
            }

            // 创建并打开设备
            result = PylonCreateDeviceByIndex(0, &m_camera);
            if (result != GENAPI_E_OK)
            {
                qDebug() << "创建相机设备失败";
                return false;
            }

            result = PylonDeviceOpen(m_camera, PYLONC_ACCESS_MODE_CONTROL | PYLONC_ACCESS_MODE_STREAM);
            if (result != GENAPI_E_OK)
            {
                qDebug() << "打开相机设备失败";
                return false;
            }

            // 配置相机参数
            ConfigureCameraARM();
            return true;
#endif
        }
        catch (const std::exception &e)
        {
            qDebug() << "相机打开失败:" << e.what();
        }

        return false;
    }

#ifdef _WIN32
    void CameraBasler::ConfigureCamera()
    {
        // Windows平台相机配置
        // m_camera->TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
        // m_camera->PixelFormat.SetValue(m_pixelFormat);

        if (m_camera->GainRaw.IsWritable())
        {
            // m_camera->GainRaw.SetValue(m_gainRaw);
        }

        if (m_camera->ExposureTimeRaw.IsWritable())
        {
            // m_camera->ExposureTimeRaw.SetValue(m_exposureTime);
        }

        if (m_camera->ReverseX.IsWritable())
        {
            // m_camera->ReverseX.SetValue(m_reverseX);
        }
        if (m_camera->ReverseY.IsWritable())
        {
            // m_camera->ReverseY.SetValue(m_reverseY);
        }
        auto imghandler = new MyImageEventHandler<CameraBasler>(this, &CameraBasler::OnImageGrabbed);
        m_camera->RegisterImageEventHandler(imghandler, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);
    }
#elif defined(__linux__) && defined(__aarch64__)
    void CameraBasler::ConfigureCameraARM()
    {
        // ARM Linux平台相机配置
        GENAPIC_RESULT result;

        // 设置触发模式
        result = PylonDeviceSetIntegerFeature(m_camera, "TriggerMode", 0);

        // 设置增益
        result = PylonDeviceSetFloatFeature(m_camera, "Gain", m_gainRaw);

        // 设置曝光时间
        result = PylonDeviceSetFloatFeature(m_camera, "ExposureTime", m_exposureTime);

        // 设置图像格式
        result = PylonDeviceSetEnumFeature(m_camera, "PixelFormat", m_pixelFormat);

        // 获取图像尺寸
        int64_t width, height;
        PylonDeviceGetIntegerFeature(m_camera, "Width", &width);
        PylonDeviceGetIntegerFeature(m_camera, "Height", &height);
        m_imageWidth = static_cast<int>(width);
        m_imageHeight = static_cast<int>(height);

        // 创建流缓冲
        result = PylonDeviceSetIntegerFeature(m_camera, "StreamBufferHandlingMode", STREAM_BUFFER_HANDLING_MODE_OLDEST_FIRST);
        result = PylonDeviceSetIntegerFeature(m_camera, "StreamBufferCountMode", STREAM_BUFFER_COUNT_MODE_MANUAL);
        result = PylonDeviceSetIntegerFeature(m_camera, "StreamBufferCountManual", 10);
    }
#endif

    bool CameraBasler::closeCamera()
    {
        if (m_camera)
        {
            try
            {
                if (m_camera->IsGrabbing())
                {
                    m_camera->StopGrabbing();
                }
                if (m_camera->IsOpen())
                {
                    m_camera->Close();
                }
                delete m_camera;
                m_camera = nullptr;
                return true;
            }
            catch (const Pylon::GenericException &e)
            {
                qDebug() << "相机关闭失败:" << e.GetDescription();
            }
        }
        return false;
    }

    bool CameraBasler::startGrab()
    {
        if (m_camera && m_camera->IsOpen())
        {
            try
            {
                m_camera->StartGrabbing(Pylon::GrabStrategy_LatestImageOnly, Pylon::GrabLoop_ProvidedByInstantCamera);
                m_bIsGrabbing = true;
                return true;
            }
            catch (const Pylon::GenericException &e)
            {
                qDebug() << "开始采集失败:" << e.GetDescription();
            }
        }
        return false;
    }

    bool CameraBasler::stopGrab()
    {
        if (m_camera && m_camera->IsGrabbing())
        {
            try
            {
                m_camera->StopGrabbing();
                m_bIsGrabbing = false;
                return true;
            }
            catch (const Pylon::GenericException &e)
            {
                qDebug() << "停止采集失败:" << e.GetDescription();
            }
        }
        return false;
    }

    bool CameraBasler::getImage()
    {
        if (!m_camera || !m_camera->IsGrabbing())
        {
            return false;
        }

        try
        {
            // 获取一帧图像
            if (m_camera->RetrieveResult(5000, m_grabResult, Pylon::TimeoutHandling_ThrowException))
            {
                if (m_grabResult->GrabSucceeded())
                {
                    QMutexLocker locker(&m_mutex);

                    // 转换为QImage
                    QImage image = ConvertToQImage(static_cast<const uint8_t *>(m_grabResult->GetBuffer()),
                                                   m_grabResult->GetWidth(),
                                                   m_grabResult->GetHeight(),
                                                   m_isColorCamera ? 3 : 1);

                    // 发送图像
                    if (!image.isNull())
                    {
                        emit ImageOutPut(image, getCameraName());
                        return true;
                    }
                }
            }
        }
        catch (const Pylon::GenericException &e)
        {
            qDebug() << "图像获取失败:" << e.GetDescription();
        }

        return false;
    }

    void CameraBasler::OnImageGrabbed(Pylon::CInstantCamera &camera, const Pylon::CGrabResultPtr &ptrGrabResult)
    {
        if (ptrGrabResult->GrabSucceeded())
        {
            QMutexLocker locker(&m_mutex);

            // 转换为QImage
            QImage image = ConvertToQImage(static_cast<const uint8_t *>(ptrGrabResult->GetBuffer()),
                                           ptrGrabResult->GetWidth(),
                                           ptrGrabResult->GetHeight(),
                                           m_isColorCamera ? 3 : 1);

            // 发送图像
            if (!image.isNull())
            {
                emit ImageOutPut(image, getCameraName());
            }
        }
    }

    QImage CameraBasler::ConvertToQImage(const uint8_t *buffer, int width, int height, int channels)
    {
        if (channels == 1)
        {
            // 灰度图像
            QImage image(width, height, QImage::Format_Grayscale8);
            for (int y = 0; y < height; ++y)
            {
                memcpy(image.scanLine(y), buffer + y * width, width);
            }
            return image;
        }
        else if (channels == 3)
        {
            // RGB彩色图像
            QImage image(width, height, QImage::Format_RGB888);
            for (int y = 0; y < height; ++y)
            {
                memcpy(image.scanLine(y), buffer + y * width * 3, width * 3);
            }
            return image;
        }
        return QImage();
    }

    void CameraBasler::ImageHeightWidth()
    {
        if (m_camera && m_camera->IsOpen())
        {
            m_imgInfo.m_img_width = static_cast<int>(m_camera->Width.GetValue());
            m_imgInfo.m_img_height = static_cast<int>(m_camera->Height.GetValue());
        }
    }

    std::string CameraBasler::getCameraName()
    {
        return m_cameraInfo.mstr_CameraName;
    }

} // namespace _Kits
