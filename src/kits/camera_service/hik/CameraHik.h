#pragma once
#include "HCNetSDK.h"
#include <QObject>
#include <cstring>
#include <mutex>
#include <qtmetamacros.h>
#include <string>
#include <vector>
#include "kits/camera_service/CameraBase.h"
#include "HikConvertRGB.h"
#include "kits/camera_service/CameraFactory.h"

namespace _Kits
{

#if __has_include(<windows.h>)
#include "WindowsPlayM4.h"
#elif __has_include(<unistd.h>)
#include "LinuxPlayM4.h"
#endif

    class CameraHik : public CameraBase
    {
        Q_OBJECT

      public:
        CameraHik();
        ~CameraHik();
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
        bool stopGrab() override;

        /*
         * @brief:获取图片
         * @return: bool
         */
        bool getImage() override;

        std::string getCameraName() override
        {
            return m_cameraInfo.mstr_CameraName;
        }

      private:
        void pushImage(char *pBuf, FRAME_INFO *pFrameInfo, long nSize);

        // 在private部分添加以下成员变量:
      private:
        LONG m_lUserID;                     // 用户ID
        LONG m_lRealPlayHandle;             // 预览句柄
        LONG m_lPort;                       // 播放端口
        NET_DVR_PREVIEWINFO m_struPlayInfo; // 预览参数
        bool m_bIsGrabbing;
        static inline QImage::Format m_color_channel = QImage::Format_Grayscale16; // 默认16位灰度图
        static void *g_pUser;
        std::unique_ptr<HikConvertRGB> m_convertRGB = nullptr;

      private:
        class CameraHikRegister
        {
          public:
            CameraHikRegister()
            {
                ICameraFactory::RegisterCameraCreateFactory<CameraHik>("HIK");
            }
        };

        inline static CameraHikRegister m_register;

        // 在signals部分添加以下信号:
      signals:

      private:
        static void CALLBACK RealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *pUser);
#if __has_include(<windows.h>)
        static void CALLBACK DecCBFun(long nPort, char *pBuf, long nSize, FRAME_INFO *pFrameInfo, long nReserved1, long nReserved2);
#elif __has_include(<unistd.h>)
        static void CALLBACK DecCBFun(int nPort, char *pBuf, int nSize, FRAME_INFO *pFrameInfo, void *nReserved1, int nReserved2);
#endif

        bool ReadConfig(const YAML::Node &config) override;
        void ImageHeightWidth() override;
      public slots:
        void onRecvConvertedRGB(const QImage &image);
    };

} // namespace _Kits