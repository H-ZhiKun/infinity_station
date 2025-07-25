#include "RfidVFR61M.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <QJsonObject>
#include <QJsonDocument>

namespace _Kits
{
    RfidVFR61M::RfidVFR61M() : m_pTcpClient(nullptr), m_pUdpClient(nullptr), mqui_tcp_port(0), mqui_udp_port(0)
    {
        // m_pUdpClient = std::make_unique<UdpClient>();
    }

    RfidVFR61M::~RfidVFR61M()
    {
        // uint8_t command = 0x81;
        // sendCommandFrame(command, 0x00);
        // m_pUdpClient.reset();
        m_timer.stop();
    }

    bool RfidVFR61M::Init(const YAML::Node &config)
    {
        if (config["TransmitFrequency"])
        {
            m_transmit_frequency = config["TransmitFrequency"].as<int>();
        }
        if (config["tcpip"] && config["tcpport"])
        {
            mqstr_tcp_ip = config["tcpip"].as<std::string>().c_str();
            mqui_tcp_port = config["tcpport"].as<quint16>();
            m_pTcpClient = std::make_unique<TcpClient>(mqstr_tcp_ip, mqui_tcp_port);
            // transmit_frequency = config["transmit_frequency"].as<int>();
            m_timer.setInterval(m_transmit_frequency);
            // return true;
        }
        if (config["csv_path"])
        {
            auto csv_path = config["csv_path"].as<std::string>();
            if (!csv_path.empty())
            {
                m_csvData = copyLineData(csv_path);
                m_rfidMap = ListDataTOUnorderedMap(m_csvData, 2); // 假设第三列为key
                return true;
            }
        }

        return false;
    }

    bool RfidVFR61M::Start()
    {
        if (!m_pTcpClient)
            return false;

        m_pTcpClient->start();
        connect(m_pTcpClient.get(), &TcpClient::recvData, this, &RfidVFR61M::OnrecvData);

        connect(&m_timer, &QTimer::timeout, this, [this]() {
            sendCommandFrame(0x80, 0x00); // 定时查询标签
        });
        m_timer.start();

        return true;
    }

    bool RfidVFR61M::Stop()
    {
        uint8_t command = 0x81;
        sendCommandFrame(command, 0x00);
        m_timer.stop(); // 停止定时器
        return true;
    }

    void RfidVFR61M::OnrecvData(const QByteArray &data)
    {

        ConcurrentPool::runTask([this, data]() {
            std::vector<uint8_t> bytes(data.begin(), data.end());

            // 获取命令码（第3个字节，索引为2）
            uint8_t lenth = bytes[2];

            switch (lenth)
            {
            case 0x04: // 0x80命令的返回
                handleInventoryResponse(bytes);
                break;
            case 0x11: // 0x41命令的返回
                handleTagDataResponse(bytes);
                break;
            default:
                break;
            }
        });
    }

    void RfidVFR61M::handleInventoryResponse(const std::vector<uint8_t> &data)
    {
        if (data.size() < 7)
        {
            LogError("Invalid inventory response length");
            return;
        }

        // 标签数量在倒数第三、二字节（大端）
        uint16_t tagCount = (data[data.size() - 3] << 8) | data[data.size() - 2];

        if (tagCount > 0)
        {
            sendCommandFrame(0x41, 0x01); // 获取标签数据
        }
        else
        {
            sendCommandFrame(0x44, 0x00); // 无标签时发送清除缓存区
        }
    }

    void RfidVFR61M::handleTagDataResponse(const std::vector<uint8_t> &data)
    {
        Rfid61MData result = parseTagData(data);
        if (result.status != 0x00)
        {
            return;
        }

        emit sendOutData(result.tagId);
        if (_mb_isSaveNaturalData)
        {
            emit sendRfidNaturalData(data, result.tagId);
        }
        if (m_rfidMap.find(result.tagId) != m_rfidMap.end())
        {
            // 找到对应的标签数据
            const QStringList &tagData = m_rfidMap[result.tagId];

            // qDebug() << "tagData size:" << tagData.size();
            QByteArray tagDataBytes;
            tagDataBytes = rfid_data_trans(tagData);
            // 打印 JSON 字符串内容
            LogInfo("Tag data JSON: ", tagDataBytes.toStdString());
            emit sendRfidBackCheckData(tagDataBytes);
        }
    }

    RfidVFR61M::Rfid61MData RfidVFR61M::parseTagData(const std::vector<uint8_t> &data)
    {
        Rfid61MData result;

        // 基本校验
        if (data.size() < 8 || data[0] != 0x0B)
        {
            LogError("Invalid tag data frame");
            return result;
        }

        // 解析基本信息
        result.readerAddress = data[1];
        uint8_t lenField = data[2];
        result.status = data[3];

        if (result.status != 0x00)
            return result;

        // 标签数量（1字节）
        uint8_t tagCount = data[4];
        if (tagCount == 0)
            return result;

        // 计算数据区长度（每组13字节）
        const int TAG_GROUP_SIZE = 13;
        int dataStart = 6;
        int expectedLength = dataStart + tagCount * TAG_GROUP_SIZE + 1; // +1 for checksum

        if (data.size() != expectedLength)
        {
            LogError("Data length mismatch");
            return result;
        }

        // 只解析第一个标签的EPC（12字节）
        if (tagCount >= 1)
        {
            int offset = dataStart;
            if (offset + 12 <= data.size())
            {
                // 提取EPC（假设从offset+1开始，前1字节为天线号）
                std::vector<uint8_t> epcBytes(data.begin() + offset + 1, data.begin() + offset + 13);
                result.tagId = ByteToHexStr(epcBytes);
            }
        }

        // 校验和验证
        uint8_t calcChecksum = calculateChecksum(data, data.size() - 1);
        if (calcChecksum != data.back())
        {
            LogError("Checksum mismatch");
            result.tagId.clear(); // 清空tagId
        }

        return result;
    }

    uint8_t RfidVFR61M::calculateChecksum(const std::vector<uint8_t> &data, size_t length)
    {
        uint8_t checksum = 0;
        for (size_t i = 0; i < length; ++i)
        {
            checksum += data[i];
        }
        return (~checksum + 1) & 0xFF; // 相加取反加 1
    }

    std::vector<uint8_t> _Kits::RfidVFR61M::sendCommandFrame(uint8_t command, uint8_t parameter, uint8_t readerAddress)
    {
        std::vector<uint8_t> frame;

        // 帧头
        frame.push_back(0x0A);

        // 读写器地址
        frame.push_back(readerAddress);

        // 计算包长，Len = 参数长度 + 2（命令码 + 校验和）
        frame.push_back(0X03);

        // 命令码
        frame.push_back(command);

        // 参数
        frame.push_back(parameter);

        // 校验和
        uint8_t checksum = calculateChecksum(frame, frame.size());
        frame.push_back(checksum);

        // 在发送数据前再次检查连接状态
        if (m_pTcpClient->getConnect())
        {
            m_pTcpClient->sendData(QByteArray(reinterpret_cast<const char *>(frame.data()), frame.size()));
            // LogInfo("Command frame sent successfully");
        }
        else
        {
            // LogError("Failed to send command frame: TCP connection lost");
        }

        return frame;
    }

    QString RfidVFR61M::ByteToHexStr(const std::vector<uint8_t> &data)
    {
        QString hexStr;
        for (int i = 0; i < data.size(); ++i)
        {
            char hex1, hex2;
            int value = data[i];
            int v1 = value / 16;
            int v2 = value % 16;
            // 将商转换为字母
            if (v1 >= 0 && v1 <= 9)
                hex1 = (char)(48 + v1);
            else
                hex1 = (char)(55 + v1);
            // 将余数转成字母
            if (v2 >= 0 && v2 <= 9)
                hex2 = (char)(48 + v2);
            else
                hex2 = (char)(55 + v2);
            hexStr.append(hex1);
            hexStr.append(hex2);
        }
        return hexStr;
    }
    QList<QStringList> RfidVFR61M::copyLineData(const std::string csv_path)
    {
        if (csv_path.empty())
        {
            LogError("CSV path is empty");
            return {};
        }
        QList<QStringList> csvData;
        auto readCSVFile = std::make_unique<ReadCSVFile>();
        if (!readCSVFile->readFile(csv_path, csvData))
        {
            LogError("Read CSV file failed");
        }
        if (csvData.isEmpty())
        {
            LogError("No data found in CSV file");
            return {};
        }

        return csvData; // 假设传入的 tagIds 是从 CSV 中提取的
    }

    std::unordered_map<QString, QStringList> RfidVFR61M::ListDataTOUnorderedMap(const QList<QStringList> &csvData, int keyColumn)
    {
        std::unordered_map<QString, QStringList> resultMap;

        if (csvData.isEmpty())
        {
            LogError("CSV data is empty");
            return resultMap;
        }

        for (const auto &row : csvData)
        {
            if (row.size() <= keyColumn)
                continue; // 确保 keyColumn 有效
            QString key = row[keyColumn];
            resultMap[key] = row;
        }

        return resultMap;
    }

    QByteArray RfidVFR61M::rfid_data_trans_json(const QStringList &data)
    {
        TIS_Info::rfid_data rfid_data;
        rfid_data.iCount = 0;
        rfid_data.iItemLength = 0;
        strncpy(rfid_data.strItem, data.size() > 2 ? data[2].toStdString().c_str() : "", sizeof(rfid_data.strItem) - 1); // rfid号
        strncpy(rfid_data.strPole, data.size() > 3 ? data[3].toStdString().c_str() : "", sizeof(rfid_data.strPole) - 1); // 杆号
        strncpy(rfid_data.strMaoDuan, "", sizeof(rfid_data.strMaoDuan) - 1);
        strncpy(rfid_data.strStation, data.size() > 6 ? data[6].toStdString().c_str() : "", sizeof(rfid_data.strStation) - 1);
        rfid_data.tTime = 0; // 如有时间字段可补充

        // 构造Json对象
        QJsonObject obj;
        obj["iCount"] = rfid_data.iCount;
        obj["iItemLength"] = rfid_data.iItemLength;
        obj["tTime"] = static_cast<qint64>(rfid_data.tTime);
        obj["strStation"] = QString::fromUtf8(rfid_data.strStation);
        obj["strMaoDuan"] = QString::fromUtf8(rfid_data.strMaoDuan);
        obj["strPole"] = QString::fromUtf8(rfid_data.strPole);
        obj["strItem"] = QString::fromUtf8(rfid_data.strItem);

        QJsonDocument doc(obj);
        return doc.toJson(QJsonDocument::Compact);
    }
    QByteArray RfidVFR61M::rfid_data_trans(const QStringList &data)
    {
        TIS_Info::rfid_data rfid_data;
        rfid_data.iCount = 0;
        rfid_data.iItemLength = 0;
        strncpy(rfid_data.strItem, data.size() > 2 ? data[2].toStdString().c_str() : "", sizeof(rfid_data.strItem) - 1); // rfid号
        strncpy(rfid_data.strPole, data.size() > 3 ? data[3].toStdString().c_str() : "", sizeof(rfid_data.strPole) - 1); // 杆号
        strncpy(rfid_data.strMaoDuan, "", sizeof(rfid_data.strMaoDuan) - 1);
        strncpy(rfid_data.strStation, data.size() > 6 ? data[6].toStdString().c_str() : "", sizeof(rfid_data.strStation) - 1);
        rfid_data.tTime = 0; // 如有时间字段可补充

        // 打印每个字段内容
        qDebug() << "iCount:" << rfid_data.iCount;
        qDebug() << "iItemLength:" << rfid_data.iItemLength;
        qDebug() << "strItem:" << QString::fromUtf8(rfid_data.strItem);
        qDebug() << "strPole:" << QString::fromUtf8(rfid_data.strPole);
        qDebug() << "strMaoDuan:" << QString::fromUtf8(rfid_data.strMaoDuan);
        qDebug() << "strStation:" << QString::fromUtf8(rfid_data.strStation);
        qDebug() << "tTime:" << rfid_data.tTime;

        // 直接将结构体内容转为QByteArray
        return QByteArray(reinterpret_cast<const char *>(&rfid_data), sizeof(rfid_data));
    }

    void RfidVFR61M::onisSaveNaturalData(const bool isSaveNaturalData)
    {
        _mb_isSaveNaturalData = isSaveNaturalData;
    }

} // namespace _Kits
