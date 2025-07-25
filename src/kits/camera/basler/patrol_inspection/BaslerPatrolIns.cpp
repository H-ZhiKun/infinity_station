// BaslerPatrolIns.cpp
#include "BaslerPatrolIns.h"
#include "kits/common/object_pool/ObjectPool.h"
#include <QDebug>
#include <memory>
#include <qlogging.h>
#include "kits/common/log/CRossLogger.h"
#include "tis_global/EnumPublic.h"

namespace _Kits
{
    BaslerPatrolIns::BaslerPatrolIns() : BaslerBase(), _m_camera(nullptr), _mb_softwareTriggerEnabled(true)
    {
    }

    BaslerPatrolIns::~BaslerPatrolIns() noexcept
    {
        closeCamera();
    }

    bool BaslerPatrolIns::init(std::unique_ptr<Pylon::CInstantCamera> camera)
    {
        try
        {
            if (!camera->IsPylonDeviceAttached())
            {
                LogError("BaslerPatrolIns::init: Camera device not attached.");
                return false;
            }

            _m_camera->Attach(camera->DetachDevice());
            _m_pBufferPools = std::make_shared<ObjectPool<TIS_Info::ImageBuffer>>();

            initializeCameraParameters();

            return true;
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerPatrolIns::init: %1").arg(e.GetDescription()).toStdString());
            return false;
        }
    }

    bool BaslerPatrolIns::start()
    {
        configureTriggerMode();
        return true;
    }

    bool BaslerPatrolIns::closeCamera()
    {
        try
        {
            if (_m_camera != nullptr)
            {
                if (_m_camera->IsGrabbing())
                {
                    _m_camera->StopGrabbing();
                }

                if (_m_camera->IsOpen())
                {
                    _m_camera->Close();

                    // 重置相机指针
                    _m_camera = nullptr;
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

    void BaslerPatrolIns::initializeCameraParameters()
    {
        try
        {
            if (_m_camera->IsOpen())
            {
                _m_camera->Close();
            }

            // 打开相机连接
            _m_camera->Open();

            // zzh 以下是否需要存在
            // 设置像素格式为默认值
            // if (GenApi::IsWritable(_m_camera->PixelFormat))
            // {
            //     _m_camera->PixelFormat.SetValue(Pylon::PixelFormatEnums::PixelFormat_Mono8);
            // }

            // 设置缓冲区处理参数
            // _m_camera->MaxNumBuffer = 5;

            // 设置自动曝光和增益
            // if (GenApi::IsWritable(_m_camera->ExposureAuto))
            // {
            //     _m_camera->ExposureAuto.SetValue(ExposureAutoEnums::ExposureAuto_Once);
            // }

            // if (GenApi::IsWritable(_m_camera->GainAuto))
            // {
            //     _m_camera->GainAuto.SetValue(GainAutoEnums::GainAuto_Once);
            // }
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerPatrolIns::initializeCameraParameters: %1").arg(e.GetDescription()).toStdString());
        }
    }

    void BaslerPatrolIns::configureTriggerMode()
    {
        try
        {
            if (!_m_camera || !_m_camera->IsOpen())
            {
                LogError("BaslerPatrolIns::configureTriggerMode: Camera not initialized");
                return;
            }

            GenApi::INodeMap &nodemap = _m_camera->GetNodeMap();

            // 设置触发模式
            Pylon::CEnumParameter(nodemap, "TriggerMode").SetValue("On");
            Pylon::CEnumParameter(nodemap, "TriggerSelector").SetValue("FrameStart");

            // 根据配置设置触发源
            const char *triggerSource = _mb_softwareTriggerEnabled ? "Software" : "Line1";
            Pylon::CEnumParameter(nodemap, "TriggerSource").SetValue(triggerSource);

            // 设置触发沿
            Pylon::CEnumParameter(nodemap, "TriggerActivation").SetValue("RisingEdge");
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerPatrolIns::configureTriggerMode: %1").arg(e.GetDescription()).toStdString());
        }
    }

    bool BaslerPatrolIns::setTriggerMode(bool softwareTrigger)
    {
        if (_mb_softwareTriggerEnabled == softwareTrigger)
        {
            return true;
        }

        _mb_softwareTriggerEnabled = softwareTrigger;
        configureTriggerMode();
        return true;
    }

    void BaslerPatrolIns::onGrabImage(int timeout)
    {
        std::lock_guard<std::mutex> lock(_m_cameraMutex);

        try
        {
            if (!_m_camera || !_m_camera->IsOpen())
            {
                LogError("BaslerPatrolIns::grabImage: Camera not initialized");
                return;
            }

            // 如果使用软件触发，发送触发信号
            if (_mb_softwareTriggerEnabled)
            {
                Pylon::CCommandParameter(_m_camera->GetNodeMap(), "TriggerSoftware").Execute();
            }

            // 等待图像抓取完成
            if (!_m_camera->WaitForFrameTriggerReady(timeout, Pylon::TimeoutHandling_ThrowException))
            {
                LogError("BaslerPatrolIns::grabImage: Timeout waiting for trigger ready");
                return;
            }

            // 开始抓取单帧图像
            _m_camera->ExecuteSoftwareTrigger();

            // 获取抓取结果
            Pylon::CGrabResultPtr grabResult;
            if (!_m_camera->RetrieveResult(timeout, grabResult, Pylon::TimeoutHandling_ThrowException))
            {
                LogError("BaslerPatrolIns::grabImage: Failed to retrieve image");
                return;
            }

            // 检查抓取结果有效性
            if (!grabResult->GrabSucceeded())
            {
                LogError(("BaslerPatrolIns::grabImage: Grab failed - {}"), grabResult->GetErrorDescription().c_str());
                return;
            }

            // 获取或创建图像缓冲区
            auto imgBuffer = _m_pBufferPools->getObject(grabResult->GetWidth(), grabResult->GetHeight(), 3);

            // 复制图像数据
            if (imgBuffer->data.size() >= grabResult->GetBufferSize())
            {
                std::memcpy(imgBuffer->data.data(), grabResult->GetBuffer(), grabResult->GetBufferSize());

                // 填充图像信息
                imgBuffer->timestamp = grabResult->GetTimeStamp();
                imgBuffer->width = grabResult->GetWidth();
                imgBuffer->height = grabResult->GetHeight();
                imgBuffer->pixFormat = static_cast<int>(TIS_Info::EnumPublic::AVPixelFormat::AV_PIX_FMT_GRAY8); // zzh 需要写一个转换类

                // 触发图像捕获信号
                emit sendImage(imgBuffer);
            }
            else
            {
                LogError("BaslerPatrolIns::grabImage: Image buffer size mismatch");
                return;
            }
        }
        catch (const Pylon::GenericException &e)
        {
            LogError(QString("BaslerPatrolIns::grabImage: %1").arg(e.GetDescription()).toStdString());
            return;
        }
    }

} // namespace _Kits