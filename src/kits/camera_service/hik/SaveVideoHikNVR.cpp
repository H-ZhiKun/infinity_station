#include "SaveVideoHikNVR.h"

#include "SaveVideoHikNVR.h"
#include <QDebug>

namespace _Kits
{
SaveVideoHikNVR::SaveVideoHikNVR()
: SaveVideoBase(nullptr)
{
   
}

SaveVideoHikNVR::~SaveVideoHikNVR()
{
}

int SaveVideoHikNVR::initSaveVideo(const QVariant &var)
{
    auto varmap = var.toMap();
    std::string cameraID = varmap.value("cameraID").toString().toStdString();
    std::string cameraName = varmap.value("cameraName").toString().toStdString();
    std::string cameraPassword = varmap.value("cameraPassword").toString().toStdString();
    short cmaeraPort = varmap.value("cameraPort").toInt(); 
    m_ui8Channel = varmap.value("channel").toUInt();
    // 初始化SDK
    NET_DVR_Init();

    // 设置连接超时时间和重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);

    // 登录设备
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40 struDeviceInfo = {0};
    
    #if __has_include(<windows.h>)
        strcpy_s(struLoginInfo.sDeviceAddress, cameraID.c_str());
        strcpy_s(struLoginInfo.sUserName, cameraName.c_str());
        strcpy_s(struLoginInfo.sPassword, cameraPassword.c_str());
    #elif __has_include(<unistd.h>)
        strncpy(struLoginInfo.sDeviceAddress, cameraID.c_str(),sizeof(struLoginInfo.sDeviceAddress));
        strncpy(struLoginInfo.sUserName, cameraName.c_str(),sizeof(struLoginInfo.sUserName));
        strncpy(struLoginInfo.sPassword, cameraPassword.c_str(),sizeof(struLoginInfo.sPassword));
    #endif

    m_lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfo);
    if (m_lUserID < 0) {
        return 1;
    }

    m_bConnected = true;
    return 0;
}
int SaveVideoHikNVR::setSubtitle(int x_StartPos, int y_StartPos, int offset_dis)
{
    if (!m_bConnected) {
        return false;
    }

    // memset(&m_struOSD, 0, sizeof(m_struOSD));
    m_struOSD.dwSize = sizeof(m_struOSD);

    // // 设置字幕叠加
    // for (size_t i = 0; i < m_vec_subtitle.size() && i < MAX_STRINGNUM_V30; i++) {
    //     m_struOSD.struStringInfo[i].wShowString = 1;  // 显示字符
    //     m_struOSD.struStringInfo[i].wStringSize = m_vec_subtitle[i].length();
    //     memcpy(m_struOSD.struStringInfo[i].sString, m_vec_subtitle[i].c_str(), m_vec_subtitle[i].length());
    //     m_struOSD.struStringInfo[i].wShowStringTopLeftX = x_StartPos;
    //     m_struOSD.struStringInfo[i].wShowStringTopLeftY = y_StartPos + i * offset_dis;
    // }

    return NET_DVR_SetDVRConfig(m_lUserID, NET_DVR_SET_SHOWSTRING_V30, m_ui8Channel, &m_struOSD, sizeof(m_struOSD));
}
void SaveVideoHikNVR::stopRecord()
{
    if (!m_bDownloading) {
        return;
    }

    if (!NET_DVR_StopGetFile(m_lDownloadHandle)) {
        return;
    }

    m_lDownloadHandle = -1;
    m_bDownloading = false;
    return;
}

int SaveVideoHikNVR::startRecord(const QVariant &var,const std::string& filepath)
{
    if (!m_bConnected || m_bDownloading) {
        return false;
    }

    // // 构造文件名
    // sprintf_s(m_szDownloadFileName, "%s\\Record_%04d%02d%02d_%02d%02d%02d.mp4",
    //          savePath.c_str(),
    //          timeRange.startTime.dwYear,
    //          timeRange.startTime.dwMonth,
    //          timeRange.startTime.dwDay,
    //          timeRange.startTime.dwHour,
    //          timeRange.startTime.dwMinute,
    //          timeRange.startTime.dwSecond);

    // 开始下载录像
    NET_DVR_PLAYCOND struDownloadCond = {0};
    // struDownloadCond.dwChannel = m_cameraInfo.mui8_Channel;
    // memcpy(&struDownloadCond.struStartTime, &timeRange.startTime, sizeof(struDownloadCond.struStartTime));
    // memcpy(&struDownloadCond.struStopTime, &timeRange.endTime, sizeof(struDownloadCond.struStopTime));
    
    
    char * filename = const_cast<char*> (filepath.c_str());
    m_lDownloadHandle = NET_DVR_GetFileByTime_V40(m_lUserID, filename, &struDownloadCond);
    
    if (m_lDownloadHandle < 0) {
        return false;
    }

    // 设置下载回调
    // if (!NET_DVR_SetPlayBackESCallBack(m_lDownloadHandle, downloadCallback, this)) {
    //     NET_DVR_StopGetFile(m_lDownloadHandle);
    //     m_lDownloadHandle = -1;
    //     return false;
    // }

    // 开始下载
    if (!NET_DVR_PlayBackControl_V40(m_lDownloadHandle, NET_DVR_PLAYSTART, NULL, 0, NULL, NULL)) {
        NET_DVR_StopGetFile(m_lDownloadHandle);
        m_lDownloadHandle = -1;
        return 1;
    }

    m_bDownloading = true;
    return 0;
}




}