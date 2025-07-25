#include "TcpPrivateClient.h"
#include <QThread>
#include <qobject.h>
namespace _Kits
{
    TcpPrivateClient::TcpPrivateClient(const QString &ip, QTcpSocket *client, qintptr socketId, QObject *parent)
        : ip_(ip), client_(client), socketId_(socketId), QObject(parent)
    {
        connect(client_, &QTcpSocket::readyRead, this, &TcpPrivateClient::onReadyRead);
    }
    TcpPrivateClient::~TcpPrivateClient() noexcept
    {
        client_->disconnectFromHost();
        client_->deleteLater();
    }
    const QString TcpPrivateClient::getIp()
    {
        return ip_;
    }
    qintptr TcpPrivateClient::socketId()
    {
        return socketId_;
    }
    void TcpPrivateClient::sendData(const QByteArray &data)
    {
        if (QThread::currentThread() == this->thread())
        {
            sendRawData(data);
        }
        else
        {
            QMetaObject::invokeMethod(this, "sendRawData", Qt::QueuedConnection, Q_ARG(QByteArray, data));
        }
    }

    qint64 TcpPrivateClient::sendRawData(const QByteArray &data)
    {
        qint64 bytes = 0;
        if (client_->state() == QAbstractSocket::ConnectedState)
        {
            qint64 bytes = client_->write(data);
            client_->flush();
        }
        return bytes;
    }

    void TcpPrivateClient::onReadyRead()
    {
        QByteArray data = client_->readAll();
        emit recvData(data);
    }
} // namespace _Kits