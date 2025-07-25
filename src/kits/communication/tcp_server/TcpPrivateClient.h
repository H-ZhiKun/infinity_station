#pragma once
#include <QObject>
#include <QTcpSocket>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace _Kits
{
    class TcpPrivateClient : public QObject
    {
        Q_OBJECT
      public:
        explicit TcpPrivateClient(const QString &ip, QTcpSocket *socket, qintptr socketId, QObject *parent = nullptr);
        virtual ~TcpPrivateClient() noexcept;
        const QString getIp();                 // 新连接 获取客户端ip 用于判断客户端是否为目标ip
        qintptr socketId();                    //  已有连接 断开时判断是否当前客户端
        void sendData(const QByteArray &data); // 发送数据异步接口
      signals:
        void recvData(const QByteArray &data); // 接收业务数据信号
      private slots:
        qint64 sendRawData(const QByteArray &data);
        void onReadyRead();

      private:
        QTcpSocket *client_ = nullptr;
        QString ip_;
        qintptr socketId_ = 0;
    };
} // namespace _Kits