#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <atomic>
#include <cstdint>

namespace _Kits
{
    class TcpClient : public QObject
    {
        Q_OBJECT

      public:
        explicit TcpClient(const QString &host, uint16_t port, uint64_t reconnectms = 5000, QObject *parent = nullptr);
        virtual ~TcpClient() noexcept;
        TcpClient(const TcpClient &) = delete;
        TcpClient &operator=(const TcpClient &) = delete;
        bool start();
        bool getConnect();
        void sendData(const QByteArray &data);
        void enablePing(bool bEnable);
      signals:
        void recvData(const QByteArray &data);

      private slots:
        qint64 sendRawData(const QByteArray &data);
        void connectToServer();
        void onConnected();
        void onDisconnected();
        void onError(QAbstractSocket::SocketError errCode);
        void onReadyRead();
        void reconnect();
        void ping();

      private:
        QTcpSocket *tcp_ = nullptr;
        QTimer *reconnectTimer_ = nullptr;
        QTimer *pingTimer_ = nullptr;
        QString host_;
        uint16_t port_;
        uint64_t reconnectms_;            // 重连间隔时间(ms)
        uint16_t heartIntervalms_ = 5000; // 心跳间隔，单位毫秒, 默认 5秒
        QByteArray heartContent_ = "ping\n";
        std::atomic_bool bconnect_ = false;
    };
} // namespace _Kits
