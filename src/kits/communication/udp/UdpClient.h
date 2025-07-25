#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <atomic>
#include <qtypes.h>
#include <unordered_map>
#include <yaml-cpp/node/node.h>
#include <QTimer>
namespace _Kits
{
    class UdpClient : public QObject
    {
        Q_OBJECT

      public:
        struct UdpBindInfo
        {
            quint16 port_ = 0;
            QString ip_;
            bool reuse_address = true;
            quint16 ttl_ = 1;
        };
        struct UdpTarget
        {
            QString name_;     // 目标名称
            quint16 type_ = 0; // 单播 = 1, 组播 = 2, 广播 = 3
            quint16 port_ = 0;
            QString ip_;
            QNetworkInterface interface_; // 网卡mac,例如(00-E0-97-21-B5-12)
        };

        explicit UdpClient(QObject *parent = nullptr);
        virtual ~UdpClient();
        bool start(const YAML::Node &config);
        // 异步发送数据, 单播, 组播, 广播通用
        void sendData(const QString &targetName, const QByteArray &data);

      signals:
        // 接收到数据信号
        void dataReceived(std::shared_ptr<std::vector<QByteArray>>);

      private slots:
        void onReadyRead();
        void sendRawData(const QString &targetName, const QByteArray &data);

      protected:
        // 绑定本地端口
        bool bind(const QString &ip, quint16 port, bool reuseAddress = true);
        // 加入组播组
        bool joinMulticastGroup(const QHostAddress &groupAddress, const QNetworkInterface &ifa);

        // 离开组播组
        bool leaveMulticastGroup(const QHostAddress &groupAddress, const QNetworkInterface &ifa);

        // 设置TTL (仅对组播和广播有效)
        void setTtl(int ttl);

      private:
        std::string m_title;
        QUdpSocket *m_udpSocket;
        UdpBindInfo m_bindInfo;
        std::atomic_bool m_bInit = false;
        std::unordered_map<QString, UdpTarget> m_mapTargets; // 存放通信对端目标信息
        QTimer *m_countTimer = nullptr;
        std::atomic<uint32_t> m_recvPacketCount{0}; // 包数计数器
    };
} // namespace _Kits
