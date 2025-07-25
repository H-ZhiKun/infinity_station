#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include "tis_global/Struct.h"

#ifdef _WIN32
#ifdef DUAGON_API_EXPORT
#define DUAGON_DLL_EXPORT __declspec(dllexport)
#else
#define DUAGON_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DUAGON_DLL_EXPORT __attribute__((visibility("default")))
#endif

namespace _Kits
{
    class DUAGON_DLL_EXPORT DuagonCardMVB
    {
      public:
        DuagonCardMVB();
        ~DuagonCardMVB();
        bool init(const std::vector<TIS_Info::DuagonCardData> &initData); // 初始化板卡
        bool writeData(const TIS_Info::DuagonCardData &data);             // 外部发送数据到本地缓存
        void setCallBack(const std::function<void(const TIS_Info::DuagonCardData &)> &recvFunc);

      private:
        bool isLittleEndian();
        void threadProcess();
        void processItem(TIS_Info::DuagonCardData &item);
        bool configPorts(const int port, const int size, bool bRead);
        bool configService();
        bool readData(TIS_Info::DuagonCardData &data);

      private:
        bool bInit = false;
        std::mutex m_mtxData;
        std::thread m_thProcess;
        std::atomic_bool m_bHolder = true;
        std::vector<TIS_Info::DuagonCardData> m_userCardData; // 端口配置
        std::function<void(const TIS_Info::DuagonCardData &)> m_readCallbackfunc = nullptr;
    };
} // namespace _Kits