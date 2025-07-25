#include "arcbee.h"
#include "kits/common/log/CRossLogger.h"
#

using namespace _Kits;

Arcbee::Arcbee(std::string title) : QObject(nullptr)
{
    mstr_title = title;
}

bool Arcbee::onInitArc(const std::unordered_map<std::string, std::shared_ptr<_Kits::UdpClient>> &map_udpClient)
{
    auto finder = map_udpClient.find(mstr_title);
    if (finder == map_udpClient.end())
    {
        LogError("[Arcbee::onInitArc] can not find tcp client.name = {}", mstr_title);
        return false;
    }

    mb_arcProcess_running.store(true);

    connect(finder->second.get(), &UdpClient::dataReceived, this, &Arcbee::onReadSockDatagrams);

    return true;
}

void Arcbee::onReadSockDatagrams(std::shared_ptr<std::vector<QByteArray>> ptrDatas)
{
    if (mb_arcProcess_running.load())
    {
        m_list_arcProcess_data = std::move(*ptrDatas);
    }
}

void Arcbee::processVoltageData()
{
}
