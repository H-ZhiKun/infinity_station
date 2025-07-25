#include "kits/pressure_spot_kits/PressureSpotFT910/PressureSpotFT910.h"
#include "kits/communication/udp/UdpClient.h"
namespace _Kits
{

    bool PressureSpotFT910::Init()
    {
        // m_udpClient=std::make_unique<_Kits::UdpClient>(this);
        // m_udpClient->bind(mqhost_address, mqull_port);
        return true;
    }

    bool PressureSpotFT910::Start()
    {
        return true;
    }

    bool PressureSpotFT910::Stop()
    {
        // if (m_udpClient!=nullptr)
        // {
        //     m_udpClient->close();
        // }

        return true;
    }
    void PressureSpotFT910::OnReceiveData(const QByteArray &data)
    {
        if (data.length() > 909 && data[0] == 0xaa && data[1] == 0x55) // 协议规定的数据长度
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

    void PressureSpotFT910::GetPressureSpotData(TIS_Info::PressureSpotData &data)
    {
        std::lock_guard<std::mutex> lock(m_mutexData);
        data = m_pressureSpotData;
    }

} // namespace _Kits