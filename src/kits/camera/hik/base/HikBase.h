#pragma once
#if __has_include(<windows.h>)
#include "hik_camera/WindowsPlayM4.h"
#elif __has_include(<unistd.h>)
#include "hik_camera/LinuxPlayM4.h"
#endif

#include "hik_camera/HCNetSDK.h"
#include "tis_global/EnumClass.h"
#include "tis_global/Struct.h"
#include <QObject>
#include "kits/common/object_pool/ObjectPool.h"

namespace _Kits
{
    class HikBase : public QObject
    {
        Q_OBJECT

      public:
        explicit HikBase(QObject *parent = nullptr);

        virtual ~HikBase() noexcept override;

        virtual bool start(TIS_Info::HikLogInfo &)
        {
            return true;
        };
        virtual bool closeCamera()
        {
            return true;
        };

      public slots:
        virtual void onWriteSubTitle(std::vector<QString>);

      signals:
        void sendImage(std::shared_ptr<TIS_Info::ImageBuffer> img);

      protected:
        long m_lUserID;         // 用户ID
        long m_lRealPlayHandle; // 预览句柄
#if __has_include(<windows.h>)
        long m_lPort; // 播放端口
        long mi_imageWidth;
        long mi_imageHeight;
        long mi_Channel;
#elif __has_include(<unistd.h>)
        int m_lPort;
        int mi_imageWidth;
        int mi_imageHeight;
        int mi_Channel;
#endif

        int mi_xStartPos = 20;
        int mi_yStartPos = 100;
        int mi_offset_dis = 32;

        NET_DVR_PREVIEWINFO m_struPlayInfo; // 预览参数
        inline static void *g_pUser = nullptr;
        bool mb_isStop = false; // 停止标志

        TIS_Info::HikLogInfo m_cameraLogInfo; // 海康相机登录信息

        std::shared_ptr<ObjectPool<TIS_Info::ImageBuffer>> m_ImageBufferPools;
    };

} // namespace _Kits