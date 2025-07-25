#pragma once

#include "kits/communication/udp/UdpClient.h"

#include "kits/common/database/connections/DatabaseConnections.h"
#include "kits/common/log/CRossLogger.h"

#include <QObject>
#include <vector>
#include <QtEndian> // 用于字节序转换

#include "tis_global/Struct.h"

namespace _Kits
{

    class Udp6017 : public QObject
    {
        Q_OBJECT
      public:
        Udp6017() : mi_expectRecvDataLen(15 + 5 * 2 * 2 * 8)
        {
        }

        bool setUdpClient(std::shared_ptr<_Kits::UdpClient> udpClient)
        {

            connect(udpClient.get(), &_Kits::UdpClient::dataReceived, this, &Udp6017::onRecv);

            return true;
        }

        /// @brief 设置机器时间
        /// @param SendTime 机器设置的发送间隔，这决定了接受包的长度
        void setSendTime(uint16_t SendTime)
        {
            // 6+9报头  间隔时间 * 每采集值对应的数据长度 * 每毫秒发送2个数据 * 采集通道数
            mi_expectRecvDataLen = 15 + SendTime * 2 * 2 * 8;
        }

      signals:
        void sendRetOut(const std::vector<double> data);
        void sendNeturalDataOut(const TIS_Info::_6017NeturalData data);

      public slots:
        void onRecv(std::shared_ptr<std::vector<QByteArray>> data)
        {
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            long long timestamp = duration.count();

            auto neturalData = data->back(); // 获取最后一个数据包

            emit sendNeturalDataOut(TIS_Info::_6017NeturalData{timestamp, neturalData});
            auto ret = processData(neturalData);
            emit sendRetOut(std::move(ret));
        }

      private:
        // 存储原始数据变量
        bool _mb_isSave_natrualData = false;
        std::vector<QByteArray> _mvec_natrualData;
        bool checkDataToWrite();

        uint32_t mi_expectRecvDataLen = 0;

        std::vector<double> processData(const QByteArray &data)
        {
            std::vector<double> result;
            if (data.size() < mi_expectRecvDataLen)
            {
                LogError("Received data size {} is less than expected size {}", data.size(), mi_expectRecvDataLen);
                return result;
            }

            // 跳过报文头和数据头
            int offset = 6 + 9; // 报文头6字节 + 数据头9字节

            // 每个通道的采样点数
            int samplesPerChannel = mi_expectRecvDataLen - 15; // 总长度减去报头和数据头
            samplesPerChannel /= (8 * 2);                      // 除以通道数和每个采样值的字节数

            // 遍历每个通道
            for (int i = 0; i < 8; ++i)
            {
                // 计算当前通道的偏移量
                int channelOffset = offset + i * samplesPerChannel * 2;

                // 打印通道标题
                //    LogInfo("Channel {} raw HEX data:", std::to_string(i));

                // 打印当前通道的所有原始字节
                QByteArray channelData = data.mid(channelOffset, samplesPerChannel * 2);
                QString hexString;
                for (int j = 0; j < channelData.size(); ++j)
                {
                    // 每2个字节为一组（一个采样值）
                    if (j > 0 && j % 2 == 0)
                    {
                        hexString += " "; // 添加分隔符
                    }

                    // 格式化为两位十六进制
                    hexString += QString("%1").arg(static_cast<uchar>(channelData[j]), 2, 16, QChar('0')).toUpper();
                }

                //    LogInfo("Channel {} raw HEX data: {}", std::to_string(i), hexString.toStdString());

                // 获取最后一个采样值（保留原有逻辑）
                int lastSampleOffset = channelOffset + (samplesPerChannel - 1) * 2;
                if (lastSampleOffset + 2 <= data.size())
                {
                    // 修复数值计算问题
                    uint16_t rawValue =
                        static_cast<uint8_t>(data[lastSampleOffset]) << 8 | static_cast<uint8_t>(data[lastSampleOffset + 1]);

                    // 处理有符号整数（如果数值超过32768则为负数）
                    int16_t signedValue = static_cast<int16_t>(rawValue);

                    double finalValue = static_cast<double>(signedValue);
                    result.push_back(finalValue);
                }
            }

            return result;
        }
    };
} // namespace _Kits