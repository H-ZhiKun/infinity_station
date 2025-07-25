#include "arcuv.h"
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;

ArcUV::ArcUV(std::string title) : mstr_title(title)
{
}

ArcUV::~ArcUV()
{
    if (mthread_arcProcess.joinable())
    {
        mthread_arcProcess.join();
    }
}

bool ArcUV::start()
{

    mthread_arcProcess = std::thread(&ArcUV::processVoltageData, this);
    return true;
}

bool ArcUV::onInitArc(const std::unordered_map<std::string, std::shared_ptr<_Kits::UdpClient>> &map_udpClient)
{
    auto finder = map_udpClient.find(mstr_title);
    if (finder == map_udpClient.end())
    {
        LogError("can not find tcp client.name = {}", mstr_title);
        return false;
    }
    mb_arcProcess_running.store(true);

    connect(finder->second.get(), &UdpClient::dataReceived, this, &ArcUV::onReadSockDatagrams);

    return true;
}

void ArcUV::onReadSockDatagrams(std::shared_ptr<std::vector<QByteArray>> ptrDatas)
{
    if (data.size() > 0)
    {
        m_list_arcProcess_data.push_back(std::move(*ptrDatas));
        m_cond_arcProcess.notify_one();
    }
}

void ArcUV::processVoltageData()
{
    while (mb_arcProcess_running.load())
    {
        std::unique_lock<std::mutex> lock(m_mtx_arcProcess);
        m_cond_arcProcess.wait(lock, [this] { return !m_list_arcProcess_data.empty() || !mb_arcProcess_running.load(); });

        if (!mb_arcProcess_running.load())
        {
            return;
        }

        auto data = m_list_arcProcess_data.front();
        m_list_arcProcess_data.pop_front();

        if (data->size() >= sizeof(double))
        {
            emit voltageReceived(*reinterpret_cast<const double *>(data->constData()));
        }
    }
}
