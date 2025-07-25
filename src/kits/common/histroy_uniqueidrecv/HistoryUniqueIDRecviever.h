#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <string>

namespace _Kits
{
class HistoryUniqueIDRecviever
{
public:
    static HistoryUniqueIDRecviever& Instance();
    void getFileTimeAndTimeStamp(long long& fileTime, double& timeStamp);
    bool start(const std::string& strServerIp, const int ServerPort, const std::string& strLocalIp);
	bool stop();
private:
    std::thread m_recvUniqueIdThread;
    std::atomic_bool m_startflag = false;
    std::string m_strServerIp = "";
    std::string m_strLocalIp = "";
	int m_serverPort = 0;
    long long m_llUniqueFileTime;
	double m_dUniqueTimeStamp;
	std::mutex m_lockUserId;
private:
    void recvUniqueIdThread();
    void setFileTimeAndTimeStamp(long long fileTime, double timeStamp);
    static bool UniqueIdCallBack(const double dTimeStamp, const long long llFileTime);
private:
    HistoryUniqueIDRecviever(/* args */) = default;
    ~HistoryUniqueIDRecviever() = default;
    HistoryUniqueIDRecviever(const HistoryUniqueIDRecviever&) = delete;
    HistoryUniqueIDRecviever& operator=(const HistoryUniqueIDRecviever&) = delete;
    HistoryUniqueIDRecviever(HistoryUniqueIDRecviever&&) = delete;
    HistoryUniqueIDRecviever& operator=(HistoryUniqueIDRecviever&&) = delete;
};

}
