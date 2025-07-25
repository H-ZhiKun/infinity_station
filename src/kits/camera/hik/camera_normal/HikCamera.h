#pragma once

#include <QObject>
#include <cstring>
#include <mutex>
#include <qtmetamacros.h>
#include <string>
#include <vector>
#include <deque>
#include <thread>
#include <condition_variable>
#include <QStringConverter>
#include <QTimer>

#include "kits/camera/hik/base/HikBase.h"
#include "kits/common/object_pool/ObjectPool.h"
#include "kits/common/log/CRossLogger.h"
#include "tis_global/Struct.h"

#include <opencv2/opencv.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>

#include "kits/cuda/YuvConverter.h"

namespace _Kits
{

    class HikCamera : public HikBase
    {
        Q_OBJECT

      public:
        explicit HikCamera();
        virtual ~HikCamera() noexcept;
        virtual bool start(TIS_Info::HikLogInfo &);
        virtual bool closeCamera();

      private:
        bool init(TIS_Info::HikLogInfo &);
        bool stopGrab();
        bool startGrab();
        void getImageHeightWidth();

        std::shared_ptr<TIS_Info::ImageBuffer> yv12Toyuv420p(std::shared_ptr<TIS_Info::ImageBuffer> img_yv12);
        std::shared_ptr<TIS_Info::ImageBuffer> YV12toRGB(std::shared_ptr<TIS_Info::ImageBuffer> img_yv12);
        void yvToyuv_thread();

        std::vector<QString> foldSubtitles(const std::vector<QString> &subTitle,
                                           int lineNum,
                                           int maxByteLength, // 字节长度限制
                                           QStringEncoder &encoder);

        int findSafeSplitPos(QStringEncoder &encoder, QStringDecoder &decoder, const QString &str, int maxBytes);

        void writeSubTitleToCamera();

      public:
        static constexpr const char *CameraType = "NORMAL";

      private:
        long m_lUserID;         // 用户ID
        long m_lRealPlayHandle; // 预览句柄
#if __has_include(<windows.h>)
        long m_lPort; // 播放端口
        long mi_imageWidth;
        long mi_imageHeight;
#elif __has_include(<unistd.h>)
        int m_lPort;
        int mi_imageWidth;
        int mi_imageHeight;
#endif
        NET_DVR_PREVIEWINFO m_struPlayInfo; // 预览参数
#if __has_include(<windows.h>)
        inline static std::unordered_map<long, void *> g_mapPortUser; // 端口与用户的映射
#elif __has_include(<unistd.h>)
        inline static std::unordered_map<int, void *> g_mapPortUser; // 端口与用户的映射
#endif

        bool mb_isStop = false; // 停止标志

        TIS_Info::HikLogInfo m_cameraLogInfo; // 海康相机登录信息

        std::shared_ptr<ObjectPool<TIS_Info::ImageBuffer>> m_ImageBufferPools;

        std::thread m_thread_yvToyuv; // 用于处理 YV12 到 YUV420P 的转换
        std::mutex m_mutex_yvToyuv;
        std::condition_variable m_cond_yvToyuv;
        std::deque<std::shared_ptr<TIS_Info::ImageBuffer>> mque_yv12; // 用于存储 YV12 图像数据

        std::vector<QString> mvec_subTitle_nofold;
        NET_DVR_SHOWSTRING_V30 m_subTitle_change;
        NET_DVR_SHOWSTRING_V30 m_subTitle_show;
        std::mutex m_mutex_subTitle;
        bool mb_subTitle_show = false; // 是否显示字幕
        QTimer m_timer_subTitle;

        size_t m_img_width = 0;
        size_t m_img_height = 0;

        std::shared_ptr<YuvConverter> _mp_yuvConverter;

      public slots:
        virtual void onWriteSubTitle(std::vector<QString>) override;
    };

} // namespace _Kits
