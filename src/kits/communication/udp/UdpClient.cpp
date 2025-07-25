#include "kits/communication/udp/UdpClient.h"
#include <memory>
#include <qlogging.h>
#include <qobject.h>
#include <qthread.h>
#include <qvariant.h>
#include "kits/common/log/CRossLogger.h"
#include <QThread>
#include <vector>
#include <yaml-cpp/yaml.h>
namespace _Kits
{
    UdpClient::UdpClient(QObject *parent) : QObject(parent)
    {
    }

    UdpClient::~UdpClient()
    {
        if (m_bInit)
        {
            m_bInit = false;
            if (m_udpSocket)
            {
                if (m_udpSocket->isOpen())
                {
                    m_udpSocket->close();
                    m_udpSocket->deleteLater();
                }
                m_udpSocket = nullptr;
            }
            if (m_countTimer && m_countTimer->isActive())
            {
                m_countTimer->stop();
            }
            _Kits::LogInfo("[UdpClient] client {} exit.", m_title);
        }
    }

    bool UdpClient::start(const YAML::Node &config)
    {
        m_title = config["client_title"].as<std::string>();
        m_bindInfo.ip_ = config["ip"].as<std::string>().c_str();
        m_bindInfo.port_ = config["port"].as<uint16_t>();
        m_bindInfo.reuse_address = config["reuse_address"].as<bool>();
        m_bindInfo.ttl_ = config["ttl"].as<uint16_t>();

        m_udpSocket = new QUdpSocket(this);
        connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpClient::onReadyRead);
        connect(m_udpSocket, &QUdpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
            LogError("udp通信错误: {}", m_udpSocket->errorString().toStdString());
        });

        if (!bind(m_bindInfo.ip_, m_bindInfo.port_, m_bindInfo.reuse_address))
        {
            return false;
        }

        m_countTimer = new QTimer(this);
        connect(m_countTimer, &QTimer::timeout, this, [this]() {
            int count = m_recvPacketCount.exchange(0);
            if (count > 0)
            {
                LogInfo("[UdpClient 每分钟包统计] Received packets: {}", count);
            }
        });
        m_countTimer->start(60 * 1000); // 每分触发一次

        QList<QNetworkInterface> allIncs = QNetworkInterface::allInterfaces();
        const YAML::Node &targets = config["targets"];
        for (const auto &target : targets)
        {
            UdpTarget tar;
            tar.name_ = target["name"].as<std::string>().c_str();
            tar.type_ = target["type"].as<int>();
            tar.ip_ = target["ip"].as<std::string>().c_str();
            tar.port_ = target["port"].as<int>();
            QString ifc = target["interface"].as<std::string>().c_str();
            ifc.replace("-", ":");
            if (!ifc.isEmpty())
            {
                for (const auto &item : allIncs)
                {
                    const auto &hdAddr = item.hardwareAddress();
                    if (hdAddr == ifc)
                    {
                        tar.interface_ = item;
                        if (tar.type_ == 2)
                        {
                            joinMulticastGroup(QHostAddress(tar.ip_), tar.interface_);
                        }
                    }
                }
            }

            m_mapTargets[tar.name_] = tar;
        }
        m_bInit = true;
        return true;
    }

    void UdpClient::sendData(const QString &targetName, const QByteArray &data)
    {
        if (!m_bInit)
            return;
        auto target = m_mapTargets.find(targetName);
        if (target == m_mapTargets.end())
        {
            LogError("udp 发送失败: 没有找到目标<{}>.", targetName.toStdString());
            return;
        }
        if (QThread::currentThread() == this->thread())
        {
            sendRawData(targetName, data);
        }
        else
        {
            QMetaObject::invokeMethod(this, "sendRawData", Qt::QueuedConnection, Q_ARG(QString, targetName), Q_ARG(QByteArray, data));
        }
    }

    void UdpClient::sendRawData(const QString &targetName, const QByteArray &data)
    {
        const auto &target = m_mapTargets.at(targetName);
        qint64 bytesSent = -1;

        if (target.type_ == 3)
        {
            bytesSent = m_udpSocket->writeDatagram(data, QHostAddress::Broadcast, target.port_);
        }
        else
        {
            bytesSent = m_udpSocket->writeDatagram(data, QHostAddress(target.ip_), target.port_);
        }

        if (bytesSent > 0)
        {
            LogInfo("udp发送成功:type={},ip={},port={}, size = {}", target.type_, target.ip_.toStdString(), target.port_, bytesSent);
        }
        else
        {
            LogError("udp发送失败:type={},ip={},port={}, error = {}",
                     target.type_,
                     target.ip_.toStdString(),
                     target.port_,
                     m_udpSocket->errorString().toStdString());
        }
    }

    bool UdpClient::bind(const QString &ip, quint16 port, bool reuseAddress)
    {
        if (m_udpSocket->state() == QAbstractSocket::BoundState)
        {
            m_udpSocket->close();
        }

        QUdpSocket::BindMode bindMode =
            reuseAddress ? QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint : QUdpSocket::DefaultForPlatform;

        bool result = m_udpSocket->bind(QHostAddress(ip), port, bindMode);
        if (!result)
        {
            LogError("udp bind失败: {}", m_udpSocket->errorString().toStdString());
        }

        m_udpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 2); // 2MB接收缓冲区
        return result;
    }

    bool UdpClient::joinMulticastGroup(const QHostAddress &groupAddress, const QNetworkInterface &ifa)
    {
        bool result = m_udpSocket->joinMulticastGroup(groupAddress, ifa);
        if (!result)
        {
            LogError("udp加入组播失败: {}", m_udpSocket->errorString().toStdString());
        }
        return result;
    }

    bool UdpClient::leaveMulticastGroup(const QHostAddress &groupAddress, const QNetworkInterface &ifa)
    {
        bool result = m_udpSocket->leaveMulticastGroup(groupAddress, ifa);
        if (!result)
        {
            LogError("udp离开组播失败: {}", m_udpSocket->errorString().toStdString());
        }
        return result;
    }

    void UdpClient::setTtl(int ttl)
    {
        m_udpSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, QVariant(ttl));
    }

    void UdpClient::onReadyRead()
    {
        auto batchBuffer = std::make_shared<std::vector<QByteArray>>();
        while (m_udpSocket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(m_udpSocket->pendingDatagramSize());

            qint64 bytesRead = m_udpSocket->readDatagram(datagram.data(), datagram.size());
            if (bytesRead > 0)
            {
                batchBuffer->push_back(std::move(datagram));
                ++m_recvPacketCount;
            }
        }
        if (batchBuffer->size())
        {
            emit dataReceived(batchBuffer);
        }
    }
} // namespace _Kits