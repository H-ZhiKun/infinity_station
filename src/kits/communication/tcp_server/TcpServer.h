#pragma once
#include <QTcpServer>
#include <memory>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "TcpPrivateClient.h"
namespace _Kits
{
    class TcpServer : public QObject
    {
        Q_OBJECT
      public:
        explicit TcpServer(QObject *parent = nullptr);
        virtual ~TcpServer() noexcept;
        bool start(const YAML::Node &config);
      signals:
        void newConnection(std::shared_ptr<TcpPrivateClient>); // controller中同步绑定，获取目标业务客户端
        void clientDisconnected(qintptr socketId);             // controller中同步绑定，删除目标业务客户端

      private slots:
        void onNewConnection();
        void onDisconnected();

      private:
        bool bind(const QString &ip, quint16 port);

        QTcpServer *m_tcpServer = nullptr;
        std::unordered_map<qintptr, std::shared_ptr<TcpPrivateClient>> m_clients;

        QString m_listenIp;
        quint16 m_listenPort = 0;
        int m_maxConnections = 1000;
    };
} // namespace _Kits
