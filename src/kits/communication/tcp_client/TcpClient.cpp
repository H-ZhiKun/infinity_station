#include "TcpClient.h"
#include "kits/common/log/CRossLogger.h"
#include <QThread>
#include <qtimer.h>

namespace _Kits
{
    TcpClient::TcpClient(const QString &host, uint16_t port, uint64_t reconnectms, QObject *parent)
        : QObject(parent), host_(host), port_(port), reconnectms_(reconnectms)
    {
    }

    TcpClient::~TcpClient() noexcept
    {
        if (pingTimer_ != nullptr)
        {
            if (pingTimer_->isActive())
                pingTimer_->stop();
        }

        if (reconnectTimer_ != nullptr)
        {
            if (reconnectTimer_->isActive())
                reconnectTimer_->stop();
        }

        if (tcp_->state() == QAbstractSocket::ConnectedState)
        {
            // LogError("Disconnecting from host...");
            tcp_->disconnectFromHost();

            // 等待对端确认断开（可选）
            if (!tcp_->waitForDisconnected(3000))
            {
                LogError("Disconnect timeout. Aborting...");
                tcp_->abort(); // 超时后强制断开
            }
            else
            {
                LogInfo("TCP Disconnected successfully.");
            }
        }
    }

    bool TcpClient::start()
    {
        tcp_ = new QTcpSocket(this);

        // 使用5秒ping 比默认的tcp keep alive 具有更高的灵敏度检测连接的异常断开情况
        // 5秒的定时器，几乎不带来开销
        pingTimer_ = new QTimer(this);
        pingTimer_->setInterval(heartIntervalms_);
        connect(pingTimer_, &QTimer::timeout, this, &TcpClient::ping);

        if (reconnectms_ > 0)
        {
            reconnectTimer_ = new QTimer(this);
            reconnectTimer_->setInterval(reconnectms_);
            connect(reconnectTimer_, &QTimer::timeout, this, &TcpClient::reconnect);
        }
        connect(tcp_, &QTcpSocket::connected, this, &TcpClient::onConnected);
        connect(tcp_, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
        connect(tcp_, &QAbstractSocket::errorOccurred, this, &TcpClient::onError);
        connect(tcp_, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
        connectToServer();
        return true;
    }

    bool TcpClient::getConnect()
    {
        return bconnect_;
    }

    void TcpClient::connectToServer()
    {
        if (tcp_->state() != QAbstractSocket::UnconnectedState && tcp_->state() != QAbstractSocket::ClosingState)
        {
            LogError("TCP State = {}. Skip connect.", static_cast<int>(tcp_->state()));
            return;
        }
        tcp_->connectToHost(host_, port_);
    }
    void TcpClient::enablePing(bool bEnable)
    {
        if (bEnable)
        {
            pingTimer_->start();
        }
        else
        {
            pingTimer_->stop();
        }
    }
    void TcpClient::sendData(const QByteArray &data)
    {
        // LogInfo("sendData to data size: {}", data.size());
        if (QThread::currentThread() == this->thread())
        {
            sendRawData(data);
        }
        else
        {
            QMetaObject::invokeMethod(this, "sendRawData", Qt::QueuedConnection, Q_ARG(QByteArray, data));
        }
    }
    qint64 TcpClient::sendRawData(const QByteArray &data)
    {
        if (!tcp_ || tcp_->state() != QAbstractSocket::ConnectedState)
            return -1;

        qint64 bytesWritten = tcp_->write(data);
        if (bytesWritten == -1)
        {
            LogError("tcp write error, ip: {}, port: {}", host_.toStdString(), port_);
        }

        return bytesWritten;
    }
    void TcpClient::reconnect()
    {
        // 断开连接后重新连接到服务器
        LogInfo("reconnect to server ip: {}, port: {}", host_.toStdString(), port_);
        connectToServer();
    }

    void TcpClient::onConnected()
    {
        if (!bconnect_)
        {
            bconnect_ = true;
            LogInfo("Connected to ip: {}, port: {}", host_.toStdString(), port_);
        }
        // 连接成功后停止重连计时器
        if (reconnectTimer_ && reconnectTimer_->isActive())
            reconnectTimer_->stop();
    }

    void TcpClient::onDisconnected()
    {
        if (bconnect_)
        {
            bconnect_ = false;
        }
        // 断开连接后启动重连计时器
        if (reconnectTimer_ && !reconnectTimer_->isActive())
            reconnectTimer_->start();
    }

    void TcpClient::onError(QAbstractSocket::SocketError errCode)
    {
        if (bconnect_)
        {
            bconnect_ = false;
            LogError("Socket error: {}, check QAbstractSocket::SocketError", static_cast<int>(errCode));
        }
        // 发生错误后启动重连计时器
        if (reconnectTimer_ && !reconnectTimer_->isActive())
            reconnectTimer_->start();
    }

    void TcpClient::onReadyRead()
    {
        QByteArray data = tcp_->readAll();
        emit recvData(data);
    }

    void TcpClient::ping()
    {
        sendData(heartContent_);
    }
} // namespace _Kits