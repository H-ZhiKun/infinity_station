#include "CameraHik.h"
#include <QDebug>
#include <libavutil/pixfmt.h>
#include <memory>
#include <mutex>
#include <qimage.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <stdio.h>
#include <string.h>


using namespace _Kits;

void *CameraHik::g_pUser = nullptr;
CameraHik::CameraHik()
    : CameraBase(nullptr), m_lUserID(-1), m_lRealPlayHandle(-1), m_lPort(-1),
      m_bIsGrabbing(false)
{
    memset(&m_struPlayInfo, 0, sizeof(NET_DVR_PREVIEWINFO));

}

CameraHik::~CameraHik()
{
    stopGrab();
    closeCamera();
}

bool CameraHik::openCamera(const YAML::Node &config)
{
    if (m_lUserID >= 0)
    {
        qDebug() << "Camera already connected!";
        return false;
    }

    if (ReadConfig(config) == false)
    {
        qDebug() << "ReadConfig failed!";
        return false;
    }

    // 初始化 SDK
    if (!NET_DVR_Init())
    {
        qDebug() << "Failed to initialize SDK!";
        return false;
    }

    if (nullptr ==  m_convertRGB)
    {
        m_convertRGB = std::make_unique<HikConvertRGB>();
    }
    m_convertRGB->initConvert();

    connect(m_convertRGB.get(), &HikConvertRGB::convertRGBEndSingal, this, &CameraHik::onRecvConvertedRGB);

    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40 struDeviceInfo = {0};

    #if __has_include(<windows.h>)
        strcpy_s(struLoginInfo.sDeviceAddress, m_cameraInfo.mstr_Camera_ID.c_str());
        strcpy_s(struLoginInfo.sUserName, m_cameraInfo.mstr_CameraUserName.c_str());
        strcpy_s(struLoginInfo.sPassword, m_cameraInfo.mstr_CameraPassword.c_str());
    #elif __has_include(<unistd.h>)
        strncpy(struLoginInfo.sDeviceAddress, m_cameraInfo.mstr_Camera_ID.c_str(),sizeof(struLoginInfo.sDeviceAddress));
        strncpy(struLoginInfo.sUserName, m_cameraInfo.mstr_CameraUserName.c_str(),sizeof(struLoginInfo.sUserName));
        strncpy(struLoginInfo.sPassword, m_cameraInfo.mstr_CameraPassword.c_str(),sizeof(struLoginInfo.sPassword));
    #endif

    struLoginInfo.wPort = m_cameraInfo.mus_CameraPort;

    m_lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfo);
    if (m_lUserID < 0)
    {
        qDebug() << "Login failed, error code:" << NET_DVR_GetLastError();
        return false;
    }
    //初始化视频存储
    // initSaveVideo(1);

    return true;
}

bool CameraHik::closeCamera()
{
    if (m_lUserID >= 0)
    {
        stopGrab();
        if (!NET_DVR_Logout(m_lUserID))
        {
            qDebug() << "Logout failed, error code:" << NET_DVR_GetLastError();
            return false;
        }
        m_lUserID = -1;
    }

    PlayM4_CloseStream(m_lPort);
    PlayM4_FreePort(m_lPort);
    NET_DVR_Cleanup();

    return true;
}

bool CameraHik::startGrab()
{
    if (m_lUserID < 0)
    {
        qDebug() << "Camera not connected!";
        return false;
    }

    if (m_bIsGrabbing)
    {
        return true;
    }

    if (ImgChannel::GetImgChannel(1) != QImage::Format_Invalid)
    {
        m_color_channel =
            ImgChannel::GetImgChannel(m_cameraInfo.mi_CameraPixelFormat);
    }

    ImageHeightWidth();

    m_struPlayInfo.hPlayWnd = NULL;
    m_struPlayInfo.lChannel = m_cameraInfo.mui8_Channel;
    m_struPlayInfo.dwStreamType = 0;
    m_struPlayInfo.dwLinkMode = 0;

    m_lRealPlayHandle = NET_DVR_RealPlay_V40(
        m_lUserID, &m_struPlayInfo, RealDataCallBack, this);
    if (m_lRealPlayHandle < 0)
    {
        qDebug() << "Start realplay failed, error code:"
                 << NET_DVR_GetLastError();
        return false;
    }

    m_bIsGrabbing = true;
    return true;
}

bool CameraHik::stopGrab()
{
    // mb_threadSubAdd_run_ = false;

    if (m_lRealPlayHandle >= 0)
    {
        if (!NET_DVR_StopRealPlay(m_lRealPlayHandle))
        {
            qDebug() << "Stop realplay failed, error code:"
                     << NET_DVR_GetLastError();
            return false;
        }
        m_lRealPlayHandle = -1;
        m_bIsGrabbing = false;
    }

    PlayM4_FreePort(m_lPort);

    return true;
}

bool CameraHik::getImage()
{
    if (m_lRealPlayHandle < 0)
    {
        qDebug() << "Realplay not started!";
        return false;
    }
    // 此处路径需要外部传出
    // char szPicFileName[256] = {0};
    // sprintf_s(szPicFileName, "capture_%d.jpg", (int)time(NULL));
    std::string strPicFileName = "capture_";
    // int curtime = (int)time(NULL);
    std::string strTime = std::to_string((int)time(NULL));
    strPicFileName += strTime;
    strPicFileName += ".jpg";

    if (!NET_DVR_CapturePicture(m_lRealPlayHandle, const_cast<char*>(strPicFileName.c_str())))
    {
        qDebug() << "Capture picture failed, error code:"
                 << NET_DVR_GetLastError();
        return false;
    }

    
    return true;
}


#if __has_include(<windows.h>)
void CALLBACK CameraHik::DecCBFun(long nPort,
                                  char *pBuf,
                                  long nSize,
                                  FRAME_INFO *pFrameInfo,
                                  long nReserved1,
                                  long nReserved2)
{
    if (nSize > 0 && pBuf != nullptr)
    {
        CameraHik *pThis = static_cast<CameraHik *>(g_pUser);
        if (pThis == nullptr)
        {
            qDebug() << "pThis is null.";
            return;
        }

        if (pFrameInfo->nType == T_YV12)
        {
            pThis->pushImage(pBuf, pFrameInfo, nSize);
        }
    }
}

#elif __has_include(<unistd.h>)
void CALLBACK CameraHik::DecCBFun(int nPort,
                                  char *pBuf,
                                  int nSize,
                                  FRAME_INFO *pFrameInfo,
                                  void* nReserved1,
                                  int nReserved2)
{
    if (nSize > 0 && pBuf != nullptr)
    {
        CameraHik *pThis = static_cast<CameraHik *>(g_pUser);
        if (pThis == nullptr)
        {
            qDebug() << "pThis is null.";
            return;
        }

        if (pFrameInfo->nType == T_YV12)
        {
            pThis->pushImage(pBuf, pFrameInfo, nSize);
        }
    }
}
#endif

void CALLBACK CameraHik::RealDataCallBack(LONG lRealHandle,
                                          DWORD dwDataType,
                                          BYTE *pBuffer,
                                          DWORD dwBufSize,
                                          void *pUser)
{
    CameraHik *pThis = static_cast<CameraHik *>(pUser);
    g_pUser = pUser;
    static bool bInit = false;

    switch (dwDataType)
    {
    case NET_DVR_SYSHEAD: // 系统头数据
        if (!bInit && dwBufSize > 0)
        {
            // 初始化 PlayM4 解码库
            if (PlayM4_GetPort(&pThis->m_lPort) == 1 &&
                PlayM4_SetStreamOpenMode(pThis->m_lPort, STREAME_REALTIME) == 1)
            {

                if (PlayM4_OpenStream(pThis->m_lPort,
                                        pBuffer,
                                        dwBufSize,
                                        20 * 1024 * 1024) == 1)
                {
                    // 设置解码回调，输出 RGB 数据
                    PlayM4_SetDecCallBack(pThis->m_lPort, DecCBFun);
                    PlayM4_Play(pThis->m_lPort, NULL);
                    bInit = true;
                    qDebug() << "解码库初始化成功";
                }
                else
                {
                    // qDebug() << "PlayM4_OpenStream 失败 errcode:" <<
                    // PlayM4_GetLastError(pThis->m_lPort);
                }
            }
            break;

        case NET_DVR_STREAMDATA: // 流数据
            if (bInit && dwBufSize > 0 && pBuffer != nullptr)
            {
                // QSharedPointer<QByteArray> bufferData =
                // QSharedPointer<QByteArray>::create(reinterpret_cast<char*>(pBuffer),
                // dwBufSize);
                if (PlayM4_InputData(pThis->m_lPort, pBuffer, dwBufSize))
                {
                }
            }
            break;

        default:
            break;
        }
    }
}

bool CameraHik::ReadConfig(const YAML::Node &config)
{
    m_cameraInfo.mstr_Camera_Type = config["camera_type"].as<std::string>();
    m_cameraInfo.mstr_Camera_ID = config["camera_ip"].as<std::string>();
    m_cameraInfo.mus_CameraLeft = config["camera_left"].as<unsigned short>();
    m_cameraInfo.mus_CameraRight = config["camera_right"].as<unsigned short>();
    m_cameraInfo.mus_CameraTop = config["camera_top"].as<unsigned short>();
    m_cameraInfo.mus_CameraBottom = config["camera_bottom"].as<unsigned short>();
    m_cameraInfo.mus_CameraGain = config["camera_gain"].as<unsigned short>();
    m_cameraInfo.mus_CameraExposure = config["camera_exposure"].as<unsigned short>();
    m_cameraInfo.mus_CameraFps = config["camera_fps"].as<unsigned short>();
    m_cameraInfo.mi_CameraPixelFormat = static_cast<PixelFormat>(config["camera_pixelformat"].as<int>());
    m_cameraInfo.mus_CameraPort = config["camera_port"].as<unsigned short>();
    m_cameraInfo.mstr_CameraPassword = config["camera_pwd"].as<std::string>();
    m_cameraInfo.mstr_CameraUserName = config["camera_username"].as<std::string>();
    m_cameraInfo.mstr_CameraName = config["camera_name"].as<std::string>();
    m_cameraInfo.mus_IsShowImg = static_cast<ReturnCode>(config["isshow_img"].as<int>());
    m_cameraInfo.mi_IsShowData = static_cast<ReturnCode>(config["isshow_data"].as<int>());
    m_cameraInfo.mstr_SaveImageType = (config["save_img_type"].as<std::string>());
    m_cameraInfo.mi_SaveImageMod = static_cast<PicSaveType>(config["save_img_mod"].as<int>());
    m_cameraInfo.mui8_Channel = config["channel"].as<unsigned char>();
    m_cameraInfo.mstr_SaveVideoType =
        config["save_video_type"].as<std::string>();
    m_cameraInfo.mstr_Video_SavePath =
        config["save_video_path"].as<std::string>();

    return true;
}
void CameraHik::ImageHeightWidth()
{
    BOOL bFlag = PlayM4_GetPort(&m_lPort);
    if (!bFlag)
    {
        qDebug() << "Failed to get port, error code:"
                 << PlayM4_GetLastError(m_lPort);
        return;
    }

    BOOL ret = PlayM4_GetPictureSize(m_lPort,
                                     (LONG *)&m_imgInfo.m_img_width,
                                     (LONG *)&m_imgInfo.m_img_height);
    if (!ret)
    {
        qDebug() << "Failed to get picture size, error code:"
                 << PlayM4_GetLastError(m_lPort);
    }
}

void CameraHik::onRecvConvertedRGB(const QImage &image)
{
    emit ImageOutPut(image, m_cameraInfo.mstr_CameraName);    
}

void CameraHik::pushImage(char *pBuf, FRAME_INFO *pFrameInfo, long nSize)
{
    if(nullptr != m_convertRGB)
    {
        m_convertRGB->pushImage(pBuf, pFrameInfo, nSize);
    }
}