#pragma once
#if __has_include(<windows.h>)
#include "hik_camera/WindowsPlayM4.h"
#elif __has_include(<unistd.h>)
#include "hik_camera/LinuxPlayM4.h"
#endif
#include "hik_camera/HCNetSDK.h"
#include <yaml-cpp/yaml.h>
#include <string>
#include "tis_global/Struct.h"

namespace _Kits
{

    class HikNvrRecord
    {
      public:
        explicit HikNvrRecord();
        ~HikNvrRecord();

        bool init(const TIS_Info::HikLogInfo &config);
        bool reinit();
        bool login(const std::string &ip, int port, const std::string &user, const std::string &password);
        void logout();
        void close();

        void DownloadOneFileByTime(
            std::string filePath, LONG lChannel, TIS_Info::systemTime startTime, TIS_Info::systemTime endTime, bool isAsync = true);
        void DownloadOneFileByTime(
            std::string filePath, const std::string &ip, TIS_Info::systemTime startTime, TIS_Info::systemTime endTime, bool isAsync = true);

      private:
        LONG getChannelByIp(const std::string &ip);
        std::string getIPByChannel(LONG lChannel);
        NET_DVR_TIME ConstructTime(TIS_Info::systemTime time);
        void downloadFile(LONG lChannel, TIS_Info::systemTime startTime, TIS_Info::systemTime endTime, std::string filePath);
        bool getDeviceInfo();
        bool SaveAnalogChanInfo();

        LONG m_lUserID = -1;
        LONG m_lRealPlayHandle = -1;
        NET_DVR_DEVICEINFO_V40 m_struDeviceInfo = {0};
        std::string m_ip;
        int m_port = 8000;
        std::string m_user;
        std::string m_password;

        std::map<LONG, std::string> m_channelIpMap; // 通道->IP映射
        std::map<std::string, LONG> m_ipChannelMap; // IP->通道映射

        typedef struct STRU_DEVICE_INFO
        {
            int iDeviceChanNum; // 设备的通道数
            int iStartChan;     // 设备开始通道号
            int iIPChanNum;     // 最大数字通道个数
            int iIPStartChan;   // 数字通道起始通道号
            int iEnableChanNum; // 有效通道数
            STRU_DEVICE_INFO()
            {
                iDeviceChanNum = -1;
                iStartChan = 0;
                iIPChanNum = 0;
                iEnableChanNum = -1;
            }
        } LOCAL_DEVICE_INFO, *pLOCAL_DEVICE_INFO;

        LOCAL_DEVICE_INFO m_localDeviceInfo; // 设备信息 的本地副本
    };

} // namespace _Kits