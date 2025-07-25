#include "HikNvrRecord.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include "kits/common/log/CRossLogger.h"
#include <iostream>

using namespace _Kits;

HikNvrRecord::HikNvrRecord()
{
    NET_DVR_Init();
}

HikNvrRecord::~HikNvrRecord()
{
    logout();
    NET_DVR_Cleanup();
}

bool HikNvrRecord::init(const TIS_Info::HikLogInfo &config)
{
    m_ip = config.mstr_ipAddr;
    m_port = config.mus_port;
    m_user = config.mstr_userName;
    m_password = config.mstr_password;
    return login(m_ip, m_port, m_user, m_password);
}

bool HikNvrRecord::reinit()
{
    return login(m_ip, m_port, m_user, m_password);
}

bool HikNvrRecord::login(const std::string &ip, int port, const std::string &user, const std::string &password)
{
    NET_DVR_USER_LOGIN_INFO loginInfo = {0};
    strncpy(loginInfo.sDeviceAddress, ip.c_str(), sizeof(loginInfo.sDeviceAddress) - 1);
    strncpy(loginInfo.sUserName, user.c_str(), sizeof(loginInfo.sUserName) - 1);
    strncpy(loginInfo.sPassword, password.c_str(), sizeof(loginInfo.sPassword) - 1);
    loginInfo.wPort = port;

    m_lUserID = NET_DVR_Login_V40(&loginInfo, &m_struDeviceInfo);
    if (m_lUserID < 0)
    {
        std::cerr << "Login failed, error: " << NET_DVR_GetLastError() << std::endl;
        return false;
    }
    std::cout << "Login success, userID: " << m_lUserID << std::endl;
    // 登录者ID
    m_localDeviceInfo.iIPStartChan = m_struDeviceInfo.struDeviceV30.byStartDChan; // 起始的 IP通道号
    m_localDeviceInfo.iIPChanNum =
        m_struDeviceInfo.struDeviceV30.byIPChanNum + m_struDeviceInfo.struDeviceV30.byHighDChanNum * 256; // IP通道 的个数
    m_localDeviceInfo.iStartChan = m_struDeviceInfo.struDeviceV30.byStartChan;                            // 起始的 模拟通道号
    m_localDeviceInfo.iDeviceChanNum = m_struDeviceInfo.struDeviceV30.byChanNum;                          // 模拟通道 的个数

    return getDeviceInfo();
}

void HikNvrRecord::logout()
{
    if (m_lUserID >= 0)
    {
        NET_DVR_Logout(m_lUserID);
        m_lUserID = -1;
    }
}

void _Kits::HikNvrRecord::DownloadOneFileByTime(
    std::string filePath, LONG lChannel, TIS_Info::systemTime startTime, TIS_Info::systemTime endTime, bool isAsync)
{
    if (isAsync)
    {
        ConcurrentPool::runTask([this, filePath, lChannel, startTime, endTime]() { downloadFile(lChannel, startTime, endTime, filePath); });
    }
    else
    {
        downloadFile(lChannel, startTime, endTime, filePath);
    }
}

void _Kits::HikNvrRecord::DownloadOneFileByTime(
    std::string filePath, const std::string &ip, TIS_Info::systemTime startTime, TIS_Info::systemTime endTime, bool isAsync)
{
    LONG lChannel = getChannelByIp(ip);

    if (lChannel < 0)
    {
        return;
    }

    if (isAsync)
    {
        ConcurrentPool::runTask([this, filePath, lChannel, startTime, endTime]() { downloadFile(lChannel, startTime, endTime, filePath); });
    }
    else
    {
        downloadFile(lChannel, startTime, endTime, filePath);
    }
}

LONG _Kits::HikNvrRecord::getChannelByIp(const std::string &ip)
{
    if (m_ipChannelMap.find(ip) != m_ipChannelMap.end())
    {
        return m_ipChannelMap[ip];
    }

    return 0;
}

std::string _Kits::HikNvrRecord::getIPByChannel(LONG lChannel)
{
    if (m_channelIpMap.find(lChannel) != m_channelIpMap.end())
    {
        return m_channelIpMap[lChannel];
    }

    return std::string();
}

NET_DVR_TIME _Kits::HikNvrRecord::ConstructTime(TIS_Info::systemTime time)
{
    NET_DVR_TIME struStartTime;
    struStartTime.dwYear = time.iYear;
    struStartTime.dwMonth = time.iMonth;
    struStartTime.dwDay = time.iDay;
    struStartTime.dwHour = time.iHour;
    struStartTime.dwMinute = time.iMinute;
    struStartTime.dwSecond = time.iSecond;
    return struStartTime;
}

void _Kits::HikNvrRecord::downloadFile(LONG lChannel, TIS_Info::systemTime startTime, TIS_Info::systemTime endTime, std::string filePath)
{
    NET_DVR_PLAYCOND DownloadCond = {0};
    DownloadCond.dwChannel = lChannel;
    NET_DVR_TIME struStartTime = ConstructTime(startTime);
    NET_DVR_TIME struStopTime = ConstructTime(endTime);
    memcpy(&(DownloadCond.struStartTime), &(struStartTime), sizeof(NET_DVR_TIME));
    memcpy(&(DownloadCond.struStopTime), &(struStopTime), sizeof(NET_DVR_TIME));
    // 1.4 调用接口
    LONG lDownload = NET_DVR_GetFileByTime_V40(m_lUserID, (char *)filePath.c_str(), &DownloadCond);
    if (lDownload == -1)
    {
        return;
    }

    // 2. 执行下载命令
    BOOL bExecute = NET_DVR_PlayBackControl_V40(lDownload, NET_DVR_PLAYSTART, NULL, 0, NULL, 0);
    if (!bExecute)
    {
        return;
    }

    int nProgress = NET_DVR_GetDownloadPos(lDownload);
    while (nProgress != 100)
    {
        nProgress = NET_DVR_GetDownloadPos(lDownload);
        Sleep(10);
    }

    // 3. 关闭下载
    BOOL bCloseDown = NET_DVR_StopGetFile(lDownload);
    if (!bCloseDown)
    {
        return;
    }
}

bool _Kits::HikNvrRecord::getDeviceInfo()
{
    // 1. 判断设备是否有IP通道
    if (m_localDeviceInfo.iIPChanNum <= 0)
    {
        // 设备没有IP通道
        return FALSE;
    }
    // 设备有IP通道

    // 2. 准备参数，调用接口，获取IP通道参数参数
    // 2.1 准备参数
    DWORD dwCommand = NET_DVR_GET_IPPARACFG_V40;           // [in] 设备配置命令
    LONG lChannel = 0;                                     // [in] 组号，从0开始，每组64个通道
    NET_DVR_IPPARACFG_V40 struIpParamCfg;                  // [out] 接收数据的缓冲
    DWORD dwOutBufferSize = sizeof(NET_DVR_IPPARACFG_V40); // [in] 接收数据的缓冲长度
    DWORD dwBytesReturned = 0;                             // [out] 实际收到的数据长度
                                                           // 2.2 调用接口，获取IP通道参数参数
    BOOL bGetIpParamCfg = NET_DVR_GetDVRConfig(m_lUserID, dwCommand, lChannel, &struIpParamCfg, dwOutBufferSize, &dwBytesReturned);
    if (!bGetIpParamCfg)
    {
        return FALSE;
    }

    if (dwBytesReturned != sizeof(NET_DVR_IPPARACFG_V40))
    {
        return FALSE;
    }

    // 3. 保存IP通道参数
    for (int i = 0; i < MAX_IP_CHANNEL; i++) // 数组后32个元素存储IP通道的 信息
    {
        if (i < m_localDeviceInfo.iIPChanNum) // 存储 现有IP通道 的信息
        {
            // i + m_localDeviceInfo.iIPStartChan 才是通道号
            m_channelIpMap[i + m_localDeviceInfo.iIPStartChan] = struIpParamCfg.struIPDevInfo[i].struIP.sIpV4;
            m_ipChannelMap[struIpParamCfg.struIPDevInfo[i].struIP.sIpV4] = i + m_localDeviceInfo.iIPStartChan;
        }
    }
    return TRUE;
}