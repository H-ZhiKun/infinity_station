#pragma once

#include "kits/camera_service/CameraBase.h"
#include "kits/camera_service/CameraFactory.h"

#ifdef _WIN32

#include <functional>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/EnumParameter.h>

#elif defined(__linux__) && defined(__aarch64__)
#include <pylonc/PylonC.h>
#endif

#include <QMutex>
template <typename T>
class MyImageEventHandler : public Pylon::CImageEventHandler
{
  public:
    using MemberFunction = void (T::*)(Pylon::CInstantCamera &, const Pylon::CGrabResultPtr &);

    // 绑定对象和OnImageGrabbed成员函数
    MyImageEventHandler(T *obj, MemberFunction func) : m_obj(obj), m_func(func)
    {
    }

    // 重写事件处理函数
    virtual void OnImageGrabbed(Pylon::CInstantCamera &camera, const Pylon::CGrabResultPtr &grabResult) override
    {
        if (m_obj && m_func)
        {
            (m_obj->*m_func)(camera, grabResult); // 调用成员函数
        }
    }

  private:
    T *m_obj;
    MemberFunction m_func;
};

namespace _Kits
{

    class CameraBasler : public CameraBase
    {
        Q_OBJECT

      public:
        explicit CameraBasler(QObject *parent = nullptr);
        ~CameraBasler() override;
        bool openCamera(const YAML::Node &config) override;
        bool closeCamera() override;
        bool startGrab() override;
        bool stopGrab() override;
        bool getImage() override;
        Pylon::CBaslerUniversalInstantCamera *m_camera;
        std::string getCameraName() override;
        void OnImageGrabbed(Pylon::CInstantCamera &camera, const Pylon::CGrabResultPtr &ptrGrabResult);

        bool SetTrigger(std::string sMode, std::string sSource, std::string sEdge) // zzh 等重构
        {
            if (!m_camera)
                return false;

            GenApi::INodeMap &m_nodemap = ((Pylon::CInstantCamera *)m_camera)->GetNodeMap();
            // const CFloatPtr cameraGen = m_nodemap.GetNode("TriggerSoftware");
            try
            {
                Pylon::CEnumParameter(m_nodemap, "TriggerMode").SetValue(sMode.c_str());       // 1.触发开关
                Pylon::CEnumParameter(m_nodemap, "TriggerSelector").SetValue("FrameStart");    // 2.触发方式选择    FrameStart
                Pylon::CEnumParameter(m_nodemap, "TriggerSource").SetValue(sSource.c_str());   // 3.触发源：Line1、Software
                Pylon::CEnumParameter(m_nodemap, "TriggerActivation").SetValue(sEdge.c_str()); // 4.上升沿或者下降沿   RisingEdge、FallingEdge
            }
            catch (...)
            {
                return false;
            }
            return true;
        }

      protected:
        bool ReadConfig(const YAML::Node &config) override;
        void ImageHeightWidth() override;

#ifdef _WIN32
        void ConfigureCamera();
        Pylon::CGrabResultPtr m_grabResult;
        Pylon::String_t m_pixelFormat;
#elif defined(__linux__) && defined(__aarch64__)
        static void OnImageGrabbed(PYLON_DEVICE_HANDLE device, const unsigned char *buffer, const size_t payloadSize, void *userContext);
        PYLON_DEVICE_HANDLE m_camera;
        PYLON_STREAMBUFFER_HANDLE m_streamBuffer;
        PYLON_IMAGE_FORMAT m_pixelFormat;
#endif

        QImage ConvertToQImage(const uint8_t *buffer, int width, int height, int channels);
        QMutex m_mutex;

        std::string m_serialNumber; // 相机序列号
        bool m_isColorCamera;       // 是否为彩色相机
        double m_gainRaw;           // 增益值
        double m_exposureTime;      // 曝光时间
        bool m_reverseX;            // X轴翻转
        bool m_reverseY;            // Y轴翻转
        bool m_istrigger;           // 是否触发
        std::string m_triggermode;  // 触发模式
        int m_imageWidth;           // 图像宽度
        int m_imageHeight;          // 图像高度

      private:
        static bool RegisterSelf()
        {
            ICameraFactory::RegisterCameraCreateFactory<CameraBasler>("Basler");

            // #ifdef _WIN32
            //             ICameraFactory::RegisterCmaeraCrteatFactory<CameraBasler>("BaslerCamera_Win");
            // #elif defined(__linux__) && defined(__aarch64__)
            //             ICameraFactory::RegisterCmaeraCrteatFactory<CameraBasler>("BaslerCamera_ARM");
            // #endif
            return true;
        }
        static const bool registered;
    };

    // 在类定义外初始化静态成员
    // inline const bool CameraBasler::registered = CameraBasler::RegisterSelf();

} // namespace _Kits
