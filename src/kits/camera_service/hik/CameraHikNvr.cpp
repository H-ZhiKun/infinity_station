#include "CameraHikNvr.h"
#include "kits/camera_service/ImgChannel.h"
#include <cstring>
#include <qimage.h>
#include <qtmetamacros.h>
#include <QImage>
#include "kits/camera_service/CameraFactory.h"

using namespace _Kits;

CameraHikNvr::CameraHikNvr() 
: m_bConnected(false), m_bGrabbing(false), m_bDownloading(false) {
    
}

CameraHikNvr::~CameraHikNvr() {
    closeCamera();
    //disconnect(this, &CameraHikNvr::subtitleSet, this, &CameraHikNvr::SetSubtitle);
}

bool CameraHikNvr::closeCamera()
{
    if (m_lUserID >= 0) {
        stopGrab();
        if (!NET_DVR_Logout(m_lUserID)) {
            qDebug() << "Logout failed, error code:" << NET_DVR_GetLastError();
            return false;
        }
        m_lUserID = -1;
    }
    return true;
}

bool CameraHikNvr::openCamera(const YAML::Node &config) {
    if(!ReadConfig(config)) {
        return false;
    }
    
    // 初始化SDK
    NET_DVR_Init();
    
    // 设置连接超时时间和重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);

    // 登录设备
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
    if (m_lUserID < 0) {
        return false;
    }

    m_bConnected = true;
    return true;
}

bool CameraHikNvr::startGrab() {
    if (!m_bConnected) {
        return false;
    }

    // 获取图像宽高
    ImageHeightWidth();
    emit imgInfoOut(m_imgInfo);
    if(ImgChannel::GetImgChannel(1) != QImage::Format_Invalid) {
        m_color_channel = ImgChannel::GetImgChannel(m_cameraInfo.mui8_Channel);
    }

    // 设置实时流回调
    m_struPlayInfo.hPlayWnd = NULL;         //窗口为空，设备SDK不解码只取流
    m_struPlayInfo.lChannel = m_cameraInfo.mui8_Channel;  //通道号
    m_struPlayInfo.dwStreamType = 0;        //主码流
    m_struPlayInfo.dwLinkMode = 0;          //TCP方式

    m_lRealPlayHandle = NET_DVR_RealPlay_V40(m_lUserID, &m_struPlayInfo, RealDataCallBack, this);
    if (m_lRealPlayHandle < 0) {
        return false;
    }

    m_bGrabbing = true;
    return true;
}

void CALLBACK CameraHikNvr::RealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser) {
    CameraHikNvr *pThis = (CameraHikNvr*)pUser;

    if (dwDataType == NET_DVR_SYSHEAD) {
        // 系统头数据
        return;
    }
    else if (dwDataType == NET_DVR_STREAMDATA) {
        // 视频流数据
        QImage img(pBuffer, 
        pThis->m_imgInfo.m_img_width, 
        pThis->m_imgInfo.m_img_height, 
        m_color_channel);

        if (pThis->m_pImageBuffer && dwBufSize <= HK_IMAGE_BUF_SIZE) {
        }
        if(pThis) {
            emit pThis->ImageOutPut(img, pThis->m_cameraInfo.mstr_CameraName);
        }
    }
}
bool CameraHikNvr::ReadConfig(const YAML::Node &config){
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

    return true;
}
bool CameraHikNvr::stopDownload() {
    if (!m_bDownloading) {
        return false;
    }

    if (!NET_DVR_StopGetFile(m_lDownloadHandle)) {
        return false;
    }

    m_lDownloadHandle = -1;
    m_bDownloading = false;
    return true;
}
void CALLBACK CameraHikNvr::downloadCallback(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData) {
  
}


void CameraHikNvr::ImageHeightWidth(){
    PlayM4_GetPictureSize(m_lPort,(LONG*) &m_imgInfo.m_img_width, (LONG*)&m_imgInfo.m_img_height);
}
