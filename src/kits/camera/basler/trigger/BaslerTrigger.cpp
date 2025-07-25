// BaslerTrigger.cpp
#include "BaslerTrigger.h"
#include "kits/common/object_pool/ObjectPool.h"
#include <QDebug>
#include <memory>
#include <qlogging.h>
#include "kits/common/log/CRossLogger.h"
#include "tis_global/EnumPublic.h"

namespace _Kits
{
    BaslerTrigger::BaslerTrigger() : BaslerBase(), m_camera(nullptr)
    {
    }

    BaslerTrigger::~BaslerTrigger() noexcept
    {
        closeCamera();
    }

    void BaslerTrigger::ImageHandler(Pylon::CInstantCamera &camera, const Pylon::CGrabResultPtr &ptrGrabResult)
    {
        try
        {
            if (ptrGrabResult->GrabSucceeded())
            {
                if (ptrGrabResult->GetPixelType() == -1)
                {
                    LogError("BaslerTrigger::ImageHandler: Pixel type not supported");
                    return;
                }

                // 获取图像数据
                uint32_t width = ptrGrabResult->GetWidth();
                uint32_t height = ptrGrabResult->GetHeight();
                uint8_t *pBuffer = (uint8_t *)ptrGrabResult->GetBuffer();

                // 获取或创建图像缓冲区
                auto imgBuffer = m_pBufferPools->getObject(width, height, 3);

                // 复制图像数据
                if (imgBuffer->data.size() >= ptrGrabResult->GetBufferSize())
                {
                    std::memcpy(imgBuffer->data.data(), pBuffer, ptrGrabResult->GetBufferSize());

                    // 填充图像信息
                    imgBuffer->timestamp = ptrGrabResult->GetTimeStamp();
                    imgBuffer->width = width;
                    imgBuffer->height = height;
                    imgBuffer->pixFormat = static_cast<int>(TIS_Info::EnumPublic::AVPixelFormat::AV_PIX_FMT_BGR8);

                    // 发送图像信号
                    emit sendImage(imgBuffer);
                }
                else
                {
                    LogError("BaslerTrigger::ImageHandler: Image buffer size mismatch");
                }
            }
            else
            {
                LogError("BaslerTrigger::ImageHandler: Grab failed - {}"), QString(ptrGrabResult->GetErrorDescription());
            }
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerTrigger::ImageHandler: %1").arg(e.GetDescription()).toStdString());
        }
    }

    bool BaslerTrigger::init(std::unique_ptr<Pylon::CInstantCamera> camera)
    {
        try
        {
            if (!m_camera->IsPylonDeviceAttached())
            {
                LogError("BaslerTrigger::init: Camera device not attached.");
                return false;
            }

            m_camera->Attach(camera->DetachDevice());

            m_pBufferPools = std::make_shared<ObjectPool<TIS_Info::ImageBuffer>>();

            return true;
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerTrigger::init: %1").arg(e.GetDescription()).toStdString());
            return false;
        }
    }

    bool BaslerTrigger::start()
    {
        try
        {
            if (m_camera == nullptr)
            {
                LogError("BaslerTrigger::start: Camera not initialized");
                return false;
            }

            if (!m_camera->IsOpen())
            {
                m_camera->Open();
            }

            if (!SetTrigger("On", "Line1", "RisingEdge"))
            {
                LogError("BaslerTrigger::start: SetTrigger函数失败");
                return false;
            }

            m_pImageEventHandler = std::make_unique<MyImageEventHandler<BaslerTrigger>>(this, &BaslerTrigger::ImageHandler);
            // 注册图像回调
            m_camera->RegisterImageEventHandler(m_pImageEventHandler.get(), Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

            // 开始抓取
            m_camera->StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);

            return true;
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerTrigger::start: %1").arg(e.GetDescription()).toStdString());
            return false;
        }
    }

    bool BaslerTrigger::closeCamera()
    {
        try
        {
            if (m_camera != nullptr)
            {
                if (m_camera->IsGrabbing())
                {
                    m_camera->StopGrabbing();
                }

                m_pImageEventHandler->DestroyImageEventHandler();

                if (m_camera->IsOpen())
                {
                    m_camera->Close();

                    // 重置相机指针
                    m_camera = nullptr;
                }

                return true;
            }
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerTrigger::closeCamera: %1").arg(e.GetDescription()).toStdString());
            return false;
        }

        return false;
    }

    bool BaslerTrigger::setGainRaw(uint16_t gain)
    {
        try
        {
            if (!m_camera || !m_camera->IsOpen())
            {
                LogError("BaslerTrigger::setGainRaw: Camera is not open");
                return false;
            }
            // 设置增益
            m_camera->GainRaw.SetValue(gain);
            // 验证增益是否设置成功
            if (m_camera->GainRaw.GetValue() != gain)
            {
                LogError("BaslerTrigger::setGainRaw: Failed to set gain");
                return false;
            }
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerTrigger::setGainRaw: %1").arg(e.GetDescription()).toStdString());
            return false;
        }

        return true;
    }

    bool BaslerTrigger::setExposureTime(uint16_t exposure)
    {
        try
        {
            if (!m_camera || !m_camera->IsOpen())
            {
                LogError("BaslerTrigger::setExposureTime: Camera is not open");
                return false;
            }
            // 设置曝光时间
            m_camera->ExposureTimeRaw.SetValue(exposure);
            // 验证曝光时间是否设置成功
            if (m_camera->ExposureTimeRaw.GetValue() != exposure)
            {
                LogError("BaslerTrigger::setExposureTime: Failed to set exposure time");
                return false;
            }
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerTrigger::setExposureTime: %1").arg(e.GetDescription()).toStdString());
            return false;
        }

        return true;
    }

    bool BaslerTrigger::SetTrigger(std::string sMode, std::string sSource, std::string sEdge)
    {
        if (!m_camera && !m_camera->IsOpen())
            return false;

        GenApi::INodeMap &m_nodemap = m_camera->GetNodeMap();
        // const CFloatPtr cameraGen = m_nodemap.GetNode("TriggerSoftware");
        try
        {
            Pylon::CEnumParameter(m_nodemap, "TriggerMode").SetValue(sMode.c_str());       // 1.触发开关
            Pylon::CEnumParameter(m_nodemap, "TriggerSelector").SetValue("FrameStart");    // 2.触发方式选择    FrameStart
            Pylon::CEnumParameter(m_nodemap, "TriggerSource").SetValue(sSource.c_str());   // 3.触发源：Line1、Software
            Pylon::CEnumParameter(m_nodemap, "TriggerActivation").SetValue(sEdge.c_str()); // 4.上升沿或者下降沿 RisingEdge、FallingEdge
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerTrigger::SetTrigger: %1").arg(e.GetDescription()).toStdString());
            return false;
        }
        return true;
    }

} // namespace _Kits