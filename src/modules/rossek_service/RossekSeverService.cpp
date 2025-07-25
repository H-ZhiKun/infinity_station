#include "RossekSeverService.h"
#include "kits/common/log/CRossLogger.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <QDataStream>
#include <QDebug>
#include <QMutexLocker>

using namespace _Kits;
namespace _Modules
{
    RossekSeverService::RossekSeverService(QObject *parent)
    {
    }

    RossekSeverService::~RossekSeverService() noexcept
    {
    }

    bool RossekSeverService::init(const YAML::Node &config)
    {
        m_tcpServer = std::make_unique<_Kits::TcpServer>();
        m_port = config["sever_port"].as<int>();
        client_count = config["client_count"].as<int>();
        m_imageQueue.resize(client_count);
        QStringList iplist;
        for (int i = 0; i < client_count; ++i)
        {
            QString ipconfig = QString("ip_%1").arg(i);
            QString nameconfig = QString("name_%1").arg(i);
            std::string cipconfig = ipconfig.toStdString();
            QString ip = QString().fromStdString(config[cipconfig].as<std::string>());
            std::string cnameconfig = nameconfig.toStdString();
            QString name = QString().fromStdString(config[cnameconfig].as<std::string>());
            initinfo[name] = false;
            initinfo[ip] = name;
            iplist << ip;
            m_clientip.insert(ip, i);
            m_iclientip.insert(i, ip);
        }
        initinfo["iplist"] = iplist;
        connect(m_tcpServer.get(), &_Kits::TcpServer::dataReceived, this, &RossekSeverService::handleNewData);
        if (!m_tcpServer)
        {
            emit error("TCP服务器未初始化");
            return false;
        }

        m_tcpServer->setPort(m_port);
        if (!m_tcpServer->startServer())
        {
            emit error(QString("无法启动TCP服务器，端口: %1").arg(m_port));
            return false;
        }
        m_runThreadFlag.store(true);
        m_runThread = std::thread(&RossekSeverService::run, this);
        return true;
    }
    bool RossekSeverService::start()
    {
        emit initinfosend(initinfo);
        return true;
    }
    bool RossekSeverService::stop()
    {
        m_runThreadFlag.store(false);
        if (m_runThread.joinable())
        {
            m_runThread.join();
        }
        if (m_tcpServer)
        {
            return m_tcpServer->stopServer();
        }

        return true;
    }
    void RossekSeverService::getinitinfo(const QVariant &)
    {
        emit sendinitinfoTopage(1);
    }

    void RossekSeverService::handleNewData(QTcpSocket *client, const QByteArray &data)
    {
        if (data.size() < HEADER_SIZE)
        {
            emit error("接收到的数据包大小不正确");
            return;
        }

        int num = m_clientip.value(client->peerAddress().toString());

        OperationType opType = parseOperationType(data);

        switch (opType)
        {
        case OperationType::StoreImage:
            processImageData(data, num);
            break;
        default:
            emit error("未知的操作类型");
            break;
        }
    }

    void RossekSeverService::processImageData(const QByteArray &data, const int num)
    {
        qint64 timestamp = parseTimestamp(data);
        int imageSize = parseImageSize(data);

        // 验证数据包完整性
        if (data.size() < HEADER_SIZE + imageSize)
        {
            emit error("图像数据不完整");
            return;
        }

        // 解析图像数据
        QImage image = parseImageData(data, HEADER_SIZE, imageSize);
        if (image.isNull())
        {
            emit error("图像解析失败");
            return;
        }
        image.save("./textpicture.jpg", "JPG", 100);
        // 添加到队列
        {
            QMutexLocker locker(&m_queueMutex);
            while (m_imageQueue[num].size() >= MAX_QUEUE_SIZE)
            {
                m_imageQueue[num].dequeue();
            }
            m_imageQueue[num].enqueue(qMakePair(image, timestamp));
        }
    }

    void RossekSeverService::notifyTask(TIS_Info::TaskInfo taskInfo)
    {
        if (taskInfo.nTaskstate == 1)
        {
            emit taskChange("");
            return;
        }

        emit taskChange(taskInfo.strTaskSavePath);
    }

    OperationType RossekSeverService::parseOperationType(const QByteArray &data)
    {
        uint16_t opType;
        QDataStream stream(data.left(OPTYPE_SIZE));
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> opType;
        return static_cast<OperationType>(opType);
    }

    qint64 RossekSeverService::parseTimestamp(const QByteArray &data)
    {
        qint64 timestamp;
        QDataStream stream(data.mid(OPTYPE_SIZE, TIMESTAMP_SIZE));
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> timestamp;
        return timestamp;
    }

    int RossekSeverService::parseImageSize(const QByteArray &data)
    {
        int size;
        QDataStream stream(data.mid(OPTYPE_SIZE + TIMESTAMP_SIZE, IMG_SIZE_SIZE));
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> size;
        return size;
    }

    QImage RossekSeverService::parseImageData(const QByteArray &data, int offset, int size)
    {
        QImage image;
        if (!image.loadFromData(data.mid(offset, size), "JPEG"))
        {
            return QImage();
        }
        return image;
    }

    bool RossekSeverService::getNextImage(QImage &image, qint64 &timestamp, const int num)
    {
        QMutexLocker locker(&m_queueMutex);
        if (m_imageQueue[num].isEmpty())
        {
            return false;
        }

        auto pair = m_imageQueue[num].dequeue();
        image = pair.first;
        timestamp = pair.second;
        return true;
    }

    int RossekSeverService::getImageCount(const int num) const
    {
        QMutexLocker locker(&m_queueMutex);
        return m_imageQueue[num].size();
    }

    void RossekSeverService::run()
    {
        while (m_runThreadFlag)
        {
            QVariantMap PicAndTimeData;
            QImage picdata;
            qint64 timestamp;
            for (int i = 0; i < client_count; ++i)
            {
                if (getNextImage(picdata, timestamp, i))
                {
                    PicAndTimeData["picturedata"] = picdata;
                    PicAndTimeData["timestamp"] = timestamp;
                    PicAndTimeData["camip"] = m_iclientip.value(i);
                    emit imageReceived(PicAndTimeData);
                }
            }
            // std::this_thread::sleep_for(std::chrono::milliseconds(m_sendRate));
        }
    }

} // namespace _Modules