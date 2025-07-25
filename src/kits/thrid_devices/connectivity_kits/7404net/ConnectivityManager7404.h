#pragma once

#include <QObject>
#include "kits/connectivity_kits/base/speedBase.h"
#include "speed7404Net.h"
#include "kits/communication/udp/UdpClient.h"
#include <QHostAddress>
#include <QByteArray>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <QVariant>
#include <QVariantMap>
#include <QTimer>
#include <QElapsedTimer>
#include "kits/connectivity_kits/base/ConnectivityManagerBase.h"

namespace _Kits
{

    class ConnectivityManager7404 : public ConnectivityManagerBase
    {
        Q_OBJECT

      public:
        ConnectivityManager7404(QObject *parent = nullptr);
        virtual ~ConnectivityManager7404();

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;
        virtual QString getName() const override;

      private:
      signals:
        void sendOutSpeedDataTotal(TIS_Info::SpeedData &speedData);  // 几个通道速度之和
        void sendOutSpeedDataSingle(TIS_Info::SpeedData &speedData); // 单个通道速度

      public slots:
        virtual void onControlSingleDo(const QVariant &data) override;
        virtual void onRecvSpeedSingle(uint8_t channel) override;
        virtual void onisSaveNaturalData(bool) override;

      private slots:
        void onRecvUdpData(const QByteArray &data, const QHostAddress &sender, quint16 senderPort);

      private:
        // 读取所有4个通道的计数值
        std::vector<uint32_t> readAllCounters();

        // 读取单个通道的计数值
        std::vector<uint32_t> readSingleCounter(uint8_t channel);

        // 清零所有4个通道的计数值
        bool resetAllCounters();

        // 清零单个通道的计数值
        bool resetSingleCounter(uint8_t channel);

        // 读取所有4个通道的开关量采集值
        std::vector<bool> readAllDigitalInputs();

        // 控制单个DO输出
        bool controlSingleDO(uint8_t channel, bool state);

        // 一次控制4个DO输出
        bool controlAllDO(const std::vector<bool> &states);

        // 接收队列中的UDP数据
        QByteArray recvUdpMessage();

        // 定时发送速度数据
        void sendSpeedData();

        // 计算通道速度之和
        TIS_Info::SpeedData calcSpeedData();

      private:
        const static constexpr uint8_t mui_channel_start_num = 0x78; // 通道起始地址
        const static constexpr uint8_t mui_channel_max_num = 4;      // 最大通道数
        const static constexpr uint8_t mui_channel_gap = 0x02;       // 通道间隔

      private:
        std::unique_ptr<speedBase> m_Speed;
        std::vector<uint32_t> mvec_nowpulse;
        std::vector<uint32_t> mvec_lastpulse;

        std::unique_ptr<UdpClient> m_udpClient;
        QHostAddress mqhost_address;
        quint16 mqull_port;
        std::atomic_bool mab_is_connect;

        std::mutex m_mutex_recvArray;
        std::deque<QByteArray> mvec_qbt_recvArray;

        QTimer m_timer;
        uint8_t m_time_interval;

        uint16_t mus_wheel_pusle;
        float mf_wheel_diameter;

        QElapsedTimer m_elapsedTimer;  // 添加耗时测量工具
        qint64 m_lastElapsed = 0;      // 记录上次时间戳
        double m_actualInterval = 0.0; // 实际间隔（second）
    };

} // namespace _Kits