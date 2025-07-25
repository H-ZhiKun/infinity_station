#include "pressure_spot_controller.h"

namespace _Controllers
{
    void PressureSpotController::setUdpClient(const std::unordered_map<std::string, std::shared_ptr<_Kits::UdpClient>> &clients)
    {
        auto finder = clients.find(m_udpName);
        if (finder != clients.end())
        {
            QObject::connect(finder->second.get(), &_Kits::UdpClient::dataReceived, [this](std::shared_ptr<std::vector<QByteArray>> data) {
                onFT910DataReceived(data);
            });
        }
    }

    void PressureSpotController::onFT910DataReceived(const std::shared_ptr<std::vector<QByteArray>> &ptrDatas)
    {
        constexpr int MIN_FRAME_SIZE = 910;
        constexpr char FRAME_HEADER[] = "\xAA\x55"; // 帧头模式
        for (const auto &data : *ptrDatas)
        {
            if (data.size() < MIN_FRAME_SIZE)
            {
                continue;
            }
            if (std::memcmp(data.constData(), FRAME_HEADER, 2) == 0)
            {
                std::lock_guard<std::mutex> lock(m_mutexData);
                int index = 107;
                for (size_t i = 0; i < 8; i++)
                {
                    QByteArray dataToConvert = data.mid(index, 4);
                    std::reverse(dataToConvert.begin(), dataToConvert.end());
                    m_pressureSpotData.press[i] = dataToConvert.toInt() / 10000.0;
                    index += 4;
                }

                index = 139;
                for (size_t i = 0; i < 2; i++)
                {
                    QByteArray dataToConvert = data.mid(index, 4);
                    std::reverse(dataToConvert.begin(), dataToConvert.end());
                    m_pressureSpotData.horAcc[i] = dataToConvert.toInt() / 10000.0;
                    index += 4;
                    dataToConvert = data.mid(index, 4);
                    std::reverse(dataToConvert.begin(), dataToConvert.end());
                    m_pressureSpotData.conAcc[i] = dataToConvert.toInt() / 10000.0;
                    index += 4;
                    dataToConvert = data.mid(index, 4);
                    std::reverse(dataToConvert.begin(), dataToConvert.end());
                    m_pressureSpotData.vidAcc[i] = dataToConvert.toInt() / 10000.0;
                    index += 4;
                }
            }
        }
    }

} // namespace _Controllers
