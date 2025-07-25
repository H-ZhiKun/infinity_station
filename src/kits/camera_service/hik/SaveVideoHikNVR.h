#pragma once
#include "kits/camera_service/encodevideo.h"
#include <QImage>
#include <queue>
#include <memory>
#include <atomic>
#include <QObject>
#include "kits/camera_service/SaveVideoBase.h"
#include "HCNetSDK.h"

#if __has_include(<windows.h>)
#include "WindowsPlayM4.h"
#elif __has_include(<unistd.h>)
#include "LinuxPlayM4.h"
#endif
namespace _Kits
{


class SaveVideoHikNVR:public SaveVideoBase
{

    Q_OBJECT

  public:
    SaveVideoHikNVR();
    ~SaveVideoHikNVR();
    int initSaveVideo(const QVariant &var) override;
    int setSubtitle(int x_StartPos, int y_StartPos, int offset_dis) override;
public slots:
    void pushImage(const QImage& img) {return;} 
    int startRecord(const QVariant &var,const std::string& filepath);
    void stopRecord();
private:
    LONG m_lDownloadHandle;         // 录像下载句柄
    bool m_bDownloading;            // 下载状态
    LONG m_lUserID;                 // 用户句柄
    LONG m_lRealPlayHandle;         // 预览句柄  
    bool m_bConnected;              // 连接状态
    uint8_t m_ui8Channel;            // 通道号
    // 字幕叠加参数
    NET_DVR_SHOWSTRING_V30 m_struOSD;

    NET_DVR_PREVIEWINFO m_struPlayInfo;

};

}

