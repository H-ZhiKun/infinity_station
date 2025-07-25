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

namespace _Kits
{
    class BaslerTrigger : public BaslerBase
    {
        Q_OBJECT

      public:
        explicit BaslerTrigger();
        virtual ~BaslerTrigger() noexcept override;

        virtual bool init(std::unique_ptr<Pylon::CInstantCamera> camera) override;
        virtual bool start() override;
        virtual bool closeCamera() override;

        static constexpr const char *CameraType = "TRIGGER"; // 添加相机类型标识

      private slots:
        virtual bool setGainRaw(uint16_t gain) override;
        virtual bool setExposureTime(uint16_t exposure) override;
        virtual bool SetTrigger(std::string sMode, std::string sSource, std::string sEdge) override;

      private:
        void ImageHandler(Pylon::CInstantCamera &camera, const Pylon::CGrabResultPtr &ptrGrabResult);
        void openDataStream();  // 添加数据流控制函数
        void closeDataStream(); // 添加数据流控制函数

        std::unique_ptr<Pylon::CBaslerUniversalInstantCamera> m_camera;    // 添加相机指针成员
        std::shared_ptr<ObjectPool<TIS_Info::ImageBuffer>> m_pBufferPools; // 添加缓冲区对象池
        std::string m_name;                                                // 添加相机名称
        std::string m_ip;                                                  // 添加IP地址
        bool m_isConnected;                                                // 添加连接状态标志

        std::unique_ptr<MyImageEventHandler<_Kits::BaslerTrigger>> m_pImageEventHandler;
    };

} // namespace _Kits