#include "TcpServer.h"
#include "kits/common/log/CRossLogger.h"
#include <memory>

namespace _Kits
{
    TcpServer::TcpServer(QObject *parent) : QObject(parent)
    {
    }

    TcpServer::~TcpServer() noexcept
    {
        if (m_tcpServer)
        {
            m_tcpServer->close();
            m_tcpServer->deleteLater();
            m_tcpServer = nullptr;
        }
        m_clients.clear();
    }

    bool TcpServer::start(const YAML::Node &config)
    {
        m_listenIp = QString::fromStdString(config["ip"].as<std::string>());
        m_listenPort = config["port"].as<uint16_t>();
        m_maxConnections = config["max_connections"].as<int>();

        m_tcpServer = new QTcpServer(this);
        connect(m_tcpServer, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);

        return bind(m_listenIp, m_listenPort);
    }

    bool TcpServer::bind(const QString &ip, quint16 port)
    {
        QHostAddress address(ip);
        bool ok = m_tcpServer->listen(address, port);
        if (!ok)
        {
            LogError("tcp server bind失败: {}", m_tcpServer->errorString().toStdString());
        }
        else
        {
            LogInfo("tcp server 启动成功: ip={}, port={}", ip.toStdString(), port);
        }
        return ok;
    }

    void TcpServer::onNewConnection()
    {
        while (m_tcpServer->hasPendingConnections())
        {
            QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
            if (!clientSocket)
                continue;

            if (m_clients.size() >= m_maxConnections)
            {
                LogWarn("tcp连接数超限, 拒绝新连接");
                clientSocket->close();
                clientSocket->deleteLater();
                continue;
            }

            qintptr socketId = clientSocket->socketDescriptor();
            QString ip = clientSocket->peerAddress().toString();
            quint16 port = clientSocket->peerPort();

            connect(clientSocket, &QTcpSocket::disconnected, this, &TcpServer::onDisconnected);

            auto client = std::make_shared<TcpPrivateClient>(ip, clientSocket, socketId, this);

            LogInfo("tcp新连接: id={}, ip={}", socketId, ip.toStdString());
            emit newConnection(client);
            m_clients[socketId] = client;
        }
    }

    void TcpServer::onDisconnected()
    {
        QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
        if (!clientSocket)
            return;
        qintptr socketId = clientSocket->socketDescriptor();
        LogInfo("tcp断开连接: id={}", socketId);
        emit clientDisconnected(socketId);
        m_clients.erase(socketId);
    }

} // namespace _Kits
