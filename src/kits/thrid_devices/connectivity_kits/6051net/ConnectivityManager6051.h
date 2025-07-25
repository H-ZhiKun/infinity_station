#pragma once
#include <yaml-cpp/yaml.h>
#include "kits/thrid_devices/connectivity_kits/base/ConnectivityManagerBase.h"
#include "kits/communication/tcp_client/TcpClient.h"
#include "kits/thrid_devices/connectivity_kits/base/speedBase.h"
#include <QElapsedTimer>
#include <QTimer>
#include <QVariant>
#include <deque>
#include <mutex>
#include <vector>
namespace _Kits
{
    class ConnectivityManager6051 : public ConnectivityManagerBase
    {
        Q_OBJECT

      public:
        ConnectivityManager6051(QObject *parent = nullptr);
        virtual ~ConnectivityManager6051();

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;
        virtual QString getName() const override;

      public slots:
        virtual void onRecvSpeedSingle(uint8_t channel) override;
        virtual void onControlSingleDo(const QVariant &data) override;
        virtual void onRecvYYJiheTrigger() override;
        virtual void onisSaveNaturalData(bool) override;

      private slots:
        void onRecvData(const QByteArray &data);

      private:
        QByteArray recvDataGet();
        std::vector<uint32_t> readAllCounters();
        void sendSpeedData();
        TIS_Info::SpeedData calcSpeedData();
        uint8_t hexCharToValue(char c);
        void GetSendData();

      private:
        std::unique_ptr<speedBase> m_Speed;

        std::unique_ptr<TcpClient> mp_tcp_client;
        QString m_ip;
        int m_port;
        std::mutex m_mutex_recvArray;
        std::condition_variable m_cond_recvArray;
        std::deque<QByteArray> mvec_qbt_recvArray;

        QByteArray m_qbt_recvData;

        uint8_t m_time_interval;

        uint16_t mus_wheel_pusle;
        double md_wheel_diameter;
        long long mll_time_interval;

        QElapsedTimer m_elapsedTimer;         // 添加耗时测量工具
        qint64 m_lastElapsed = 0;             // 记录上次时间戳
        double m_actualInterval = 0.0;        // 实际间隔（second）
        double md_kilometer = 0.0;            // 上次总距离（kilometer）
        std::vector<uint32_t> mvec_lastpulse; // 上次脉冲计数
        std::vector<uint32_t> mvec_nowpulse;  // 当前脉冲计数

        QByteArray m_pSendData;
        QString m_sSendData;
        uint8_t m_dwHexDataLen = 0;
        char m_arraySendStart[100];
        char m_arraySendClose[100];

        inline static constexpr uint8_t CMD_GETPULSE[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x18, 0x00, 0x04};
    };
} // namespace _Kits
