#pragma once

#ifdef _WIN32
#include <functional>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/EnumParameter.h>
#elif defined(__linux__) && defined(__aarch64__)
#include <pylonc/PylonC.h>
#endif

#include "tis_global/EnumClass.h"
#include "tis_global/Struct.h"
#include <QObject>
#include "kits/camera/basler/base/BaslerBase.h"
#include "kits/common/object_pool/ObjectPool.h"
#include <memory>
#include <mutex>

namespace _Kits
{
    class BaslerPatrolIns : public BaslerBase
    {
        Q_OBJECT

      public:
        explicit BaslerPatrolIns();
        virtual ~BaslerPatrolIns() noexcept override;

        virtual bool init(std::unique_ptr<Pylon::CInstantCamera> camera) override;
        virtual bool start() override;
        virtual bool closeCamera() override;

        static constexpr const char *CameraType = "PATROL_INSPECTION"; // 添加相机类型标识

      signals:
        void imageCaptured(std::shared_ptr<TIS_Info::ImageBuffer> image);

      private:
        void configureTriggerMode();
        void initializeCameraParameters();
        // 设置触发模式
        bool setTriggerMode(bool softwareTrigger);
        // 主动获取图像接口
      public slots:
        void onGrabImage(int timeout = 1000);

      private:
        std::unique_ptr<Pylon::CBaslerUniversalInstantCamera> _m_camera;    // 相机指针
        std::shared_ptr<ObjectPool<TIS_Info::ImageBuffer>> _m_pBufferPools; // 缓冲区池
        bool _mb_softwareTriggerEnabled;                                    // 软件触发使能标志
        std::mutex _m_cameraMutex;                                          // 相机访问互斥锁
    };

} // namespace _Kits