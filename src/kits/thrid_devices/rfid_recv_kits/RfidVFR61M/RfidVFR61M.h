#pragma once

#include "kits/common/log/CRossLogger.h"
#include <QTimer>
#include <vector>
#include "tis_global/Struct.h"
#include "kits/common/read_csv/ReadCsvFile.h"
#include <unordered_map>
#include "kits/thrid_devices/rfid_recv_kits/RfidBase/RfidBase.h"
#include "kits/communication/tcp_client/TcpClient.h"
#include "kits/communication/udp/UdpClient.h"
namespace _Kits
{

    class RfidVFR61M : public RfidBase
    {
        Q_OBJECT

      public:
        RfidVFR61M();
        virtual ~RfidVFR61M() override;

        virtual bool Init(const YAML::Node &) override;
        virtual bool Start() override;
        virtual bool Stop() override;

      private:
        struct Rfid61MData
        {
            uint8_t readerAddress;             // 读写器地址
            uint8_t status;                    // 状态
            uint8_t command;                   // 长度
            std::vector<uint8_t> responseData; // 响应数据
            QString tagId;                     // 多标签数据
        };

        std::unique_ptr<TcpClient> m_pTcpClient;
        std::unique_ptr<UdpClient> m_pUdpClient;            // 使用UdpClient发送数据
        QString mqstr_tcp_ip;                               // tcp ip
        quint16 mqui_tcp_port;                              // tcp端口
        QString mqstr_udp_ip;                               // udp ip
        quint16 mqui_udp_port;                              // udp端口
        std::unordered_map<QString, QStringList> m_rfidMap; // 存储标签数据的映射
        QList<QStringList> m_csvData;                       // 存储CSV数据
        int m_transmit_frequency = 1000;                      // 发送频率

        // int marktoint(const uint8_t* mark);
        Rfid61MData parseTagData(const std::vector<uint8_t> &data); // 解析数据

        uint8_t calculateChecksum(const std::vector<uint8_t> &data, size_t length); // 计算校验和

        std::vector<uint8_t> sendCommandFrame(uint8_t command,
                                              uint8_t parameter,
                                              uint8_t readerAddress = 0x03); // 发送命令帧，使用设置波特率测试

        QTimer m_timer; // 定时器

        QString ByteToHexStr(const std::vector<uint8_t> &data); // 将字节数组转换为十六进制字符串

        void handleInventoryResponse(const std::vector<uint8_t> &data); // 处理0x80命令的返回帧

        void handleTagDataResponse(const std::vector<uint8_t> &data); // 处理0x41命令的返回帧

        QByteArray rfid_data_trans_json(const QStringList &data); // 将数据写到结构体并转换为二进制数据
        QByteArray rfid_data_trans(const QStringList &data);

        std::unique_ptr<ReadCSVFile> m_readCSVFile = nullptr;

        QList<QStringList> copyLineData(const std::string csv_path);

        std::unordered_map<QString, QStringList> ListDataTOUnorderedMap(const QList<QStringList> &csvData, int keyColumn);

      protected:
      protected slots:
        virtual void OnrecvData(const QByteArray &data) override;
        virtual void onisSaveNaturalData(bool) override;
    };

} // namespace _Kits
