#pragma once

#include <qimage.h>
#include <qtmetamacros.h>
#pragma once
#include "HCNetSDK.h"
#include <QObject>
#include <string>
#include <vector>
#include "kits/camera_service/CameraBase.h"
#include "kits/camera_service/CameraFactory.h"

// #include "../detect_kit/panto/include/modelinferenceframework_tensorrt_Instantiate.h"

namespace _Kits
{

#if __has_include(<windows.h>)
#include "WindowsPlayM4.h"
#elif __has_include(<unistd.h>)
#include "LinuxPlayM4.h"
#endif

    class CameraHikNvr : public CameraBase
    {
        Q_OBJECT
        constexpr static int HK_IMAGE_BUF_SIZE = 3840 * 2160 * 4 + 54;

      public:
        CameraHikNvr();
        ~CameraHikNvr();
        /*
         * @brief:打开相机
         * @param: strIp:相机ip地址
         * @param: strUser:相机用户名
         * @param: strPwd:相机密码
         * @param: iPort:相机端口
         * @return: bool
         */
        bool openCamera(const YAML::Node &config) override;

        /*
         * @brief:关闭相机
         * @return: bool
         */
        bool closeCamera() override;

        /*
         * @brief:开始抓图
         * @return: bool
         */
        bool startGrab() override;

        /*
         * @brief:停止抓图
         * @return: bool
         */
        bool stopGrab() override
        {
            return true;
        };

        /*
         * @brief:获取图片
         * @return: bool
         */
        bool getImage() override
        {
            return false;
        }

        std::string getCameraName() override
        {
            return m_cameraInfo.mstr_CameraName;
        }

        /*
         * @brief: 停止下载录像
         * @return: bool - 成功返回true,失败返回false
         */
        bool stopDownload();

        // /*
        // * @brief: 获取下载进度
        // * @return: int - 下载进度(0-100),失败返回-1
        // */
        // int getDownloadProgress();
        // 在private部分添加以下成员变量:
      private:
        // 海康SDK相关变量
        LONG m_lUserID;         // 用户句柄
        LONG m_lRealPlayHandle; // 预览句柄
        bool m_bConnected;      // 连接状态
        bool m_bGrabbing;       // 抓图状态

        unsigned char *m_pImageBuffer; // 图像缓冲区
        // HANDLE m_hCaptureEvent;         // 采集事件
        // CRITICAL_SECTION m_csCallback;   // 临界区对象

        // 图像参数
        int m_iImageWidth;
        int m_iImageHeight;
        int m_iImageChannels;

        // 字幕叠加参数
        NET_DVR_SHOWSTRING_V30 m_struOSD;

        NET_DVR_PREVIEWINFO m_struPlayInfo;

        LONG m_lDownloadHandle;         // 录像下载句柄
        bool m_bDownloading;            // 下载状态
        char m_szDownloadFileName[256]; // 下载文件名
        int m_lPort = 0;

        // 在signals部分添加以下信号:
      signals:

      private:
        static void CALLBACK RealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *pUser);
        bool ReadConfig(const YAML::Node &config) override;
        // 下载进度回调
        static void CALLBACK downloadCallback(DWORD dwType, void *lpBuffer, DWORD dwBufLen, void *pUserData);
        void ImageHeightWidth() override;

      private:
        static inline QImage::Format m_color_channel = QImage::Format_Grayscale16; // 默认16位灰度图

      private:
        class CameraHikNvrRegister
        {
          public:
            CameraHikNvrRegister()
            {
                ICameraFactory::RegisterCameraCreateFactory<CameraHikNvr>("HIKNVR");
            }
        };

        inline static CameraHikNvrRegister m_register;
    };

} // namespace _Kits