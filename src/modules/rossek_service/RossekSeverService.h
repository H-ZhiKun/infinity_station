#pragma once

#include "Tis_global/Struct.h"
#include "kits/common/factory/ModuleRegister.h"
#include "kits/common/module_base/ModuleBase.h"
#include "kits/communication/tcp_server/TcpServer.h"
#include "kits/communication/tcp_server/TcpPrivateClient.h"
#include <QImage>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QPair>
#include <QQueue>
#include <QString>
#include <memory>
#include <thread>

using namespace _Kits;
namespace _Modules
{
    enum class OperationType : uint16_t
    {
        StoreImage = 0x0001,
        // 在此添加更多操作类型
        Unknown = 0xFFFF
    };

    class RossekSeverService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(RossekSeverService)
      public:
        explicit RossekSeverService(QObject *parent = nullptr);
        virtual ~RossekSeverService() noexcept;
        RossekSeverService(const RossekSeverService &) = delete;
        RossekSeverService &operator=(const RossekSeverService &) = delete;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        /**********************
         * @brief tcp服务端模块接收到保存图片信息给其他模块转发
         * @param msg  数据
         * @return 空
         ************************ */
        // void imageReceived(const QImage& image, float timestamp);
        void imageReceived(const QVariant &);
        void initinfosend(const QVariantMap &);
        /**********************
         * @brief tcp服务端模块接收到错误信息给其他模块转发
         * @param msg  数据
         * @return 空
         ************************ */
        void error(const QVariant &); // const QString& message

        void taskChange(const QString &);
        void sendinitinfoTopage(const QVariant &);

      private slots:
        void handleNewData(QTcpSocket *client, const QByteArray &data);
        void processImageData(const QByteArray &data, const int num);
        void notifyTask(TIS_Info::TaskInfo);
        void getinitinfo(const QVariant &); // 界面通过此函数获取所有相机的信息

      private:
        OperationType parseOperationType(const QByteArray &data);
        qint64 parseTimestamp(const QByteArray &data);
        int parseImageSize(const QByteArray &data);
        QImage parseImageData(const QByteArray &data, int offset, int size);
        // 获取图像队列中的一张图片
        bool getNextImage(QImage &image, qint64 &timestamp, const int num);
        // 获取队列中的图片数量
        int getImageCount(const int num) const;
        void run();

      private:
        QVariantMap initinfo;
        std::unique_ptr<_Kits::TcpPrivateClient> m_tcpClient;
        quint16 m_port;
        quint16 client_count;
        std::atomic<bool> m_runThreadFlag = false;
        std::thread m_runThread;
        // 图像队列
        QVector<QQueue<QPair<QImage, qint64>>> m_imageQueue;
        QMap<QString, int> m_clientip;
        QMap<int, QString> m_iclientip;
        mutable QMutex m_queueMutex;
        static constexpr int MAX_QUEUE_SIZE = 100;

        // 数据包格式常量
        static constexpr int OPTYPE_SIZE = 2;
        static constexpr int TIMESTAMP_SIZE = 8;
        static constexpr int IMG_SIZE_SIZE = 4;
        static constexpr int HEADER_SIZE = OPTYPE_SIZE + TIMESTAMP_SIZE + IMG_SIZE_SIZE;
        static constexpr int m_sendRate = 1000;
    };
} // namespace _Modules