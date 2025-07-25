#include "HistoryUniqueIDRecviever.h"
#include "IUniqueIdRecieve.h"
#include "kits/common/log/CRossLogger.h"

namespace _Kits
{
    bool HistoryUniqueIDRecviever::UniqueIdCallBack(const double dTimeStamp, const long long llFileTime)
    {
        HistoryUniqueIDRecviever::Instance().setFileTimeAndTimeStamp(llFileTime, dTimeStamp);
        return true;
    }
    HistoryUniqueIDRecviever &HistoryUniqueIDRecviever::Instance()
    {
        static HistoryUniqueIDRecviever instance;
        return instance;
    }

    bool HistoryUniqueIDRecviever::start(const std::string &strServerIp, const int ServerPort, const std::string &strLocalIp)
    {
        if (!m_startflag.load())
        {
            m_strServerIp = strServerIp;
            m_strLocalIp = strLocalIp;
            m_serverPort = ServerPort;
        }
        m_startflag.store(true);
        m_recvUniqueIdThread = std::thread(&HistoryUniqueIDRecviever::recvUniqueIdThread, this);
        return true;
    }
    bool HistoryUniqueIDRecviever::stop()
    {
        m_startflag.store(false);
        if (m_recvUniqueIdThread.joinable())
        {
            m_recvUniqueIdThread.join();
        }
        return true;
    }
    void HistoryUniqueIDRecviever::getFileTimeAndTimeStamp(long long &fileTime, double &timeStamp)
    {
        std::lock_guard<std::mutex> mlock(m_lockUserId);
        fileTime = m_llUniqueFileTime;
        timeStamp = m_dUniqueTimeStamp;
    }

    void HistoryUniqueIDRecviever::setFileTimeAndTimeStamp(long long fileTime, double timeStamp)
    {
        std::lock_guard<std::mutex> mlock(m_lockUserId);
        m_llUniqueFileTime = fileTime;
        m_dUniqueTimeStamp = timeStamp;
    }

    void HistoryUniqueIDRecviever::recvUniqueIdThread()
    {
        IUniqueIdRecieveDll *uniqueIdRecieve = CreateUniqueIdRecieveDll();
        bool bLastOpenState = false;
        bool bOpen = false;

        while (m_startflag.load())
        {
            if (!bOpen)
            {
                if (!uniqueIdRecieve->Open(m_strServerIp.c_str(), m_serverPort, m_strLocalIp.c_str()))
                {
                    bOpen = false;
                    LogError("UniqueIdReciever::Open()  Client open failed");
                }
                else
                {
                    bOpen = true;
                    uniqueIdRecieve->AddCallback(HistoryUniqueIDRecviever::UniqueIdCallBack);
                    LogInfo("UniqueIdReciever::Open()  Client open success");
                }
            }
            else if (uniqueIdRecieve->IsTimeOut())
            {
                // Logger->Info("uniqueIdRecieve time out  , reconnect");
                uniqueIdRecieve->Close();
                // Sleep(100);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                bOpen = false;
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        if (bOpen)
        {
            uniqueIdRecieve->Close();
        }

        FreeUniqueIdRecieveDll(uniqueIdRecieve);
        uniqueIdRecieve = NULL;
    }
} // namespace _Kits