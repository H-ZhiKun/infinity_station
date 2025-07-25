#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QVariant>
namespace _Kits
{
    // 需要在主线程中创建HttpClient对象。controller构造函数中或同步链接方式函数中，module的init或者start接口
    // 如果需要链接复用，应该复用HttpClient对象指针。
    // 调用示例在下面。
    class HttpClient : public QObject
    {
        Q_OBJECT

      public:
        enum class RequestType
        {
            GET,
            POST,
            PUT,
            DELETE,
            PATCH
        };

        explicit HttpClient(QObject *parent = nullptr);
        virtual ~HttpClient() noexcept;

        // 发送通用请求
        void sendRequest(RequestType type,
                         const QString &url,
                         const QVariantMap &data = QVariantMap(),
                         const QVariantMap &headers = {},
                         int timeout = 30000);

        // 取消所有请求
        void cancelAllRequests();

        // 上传文件
        void uploadFile(const QString &url, const QString &filePath, const QVariantMap &formData = {}, const QVariantMap &headers = {});

      signals:
        // 请求完成信号，返回响应数据
        void requestFinished(const QByteArray &data);
        // 请求错误信号
        void requestError(const QString &error);
        // 上传进度
        void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
        // 下载进度
        void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
      private slots:
        // 主线程中执行
        void sendRawData(RequestType type, const QString &url, const QVariantMap &data, const QVariantMap &headers, int timeout);
        void doUploadFile(const QString &url, const QString &filePath, const QVariantMap &formData, const QVariantMap &headers);

      private:
        QNetworkReply *preparePostRequest(QNetworkRequest &request, const QVariant &data);
        QNetworkReply *preparePutRequest(QNetworkRequest &request, const QVariant &data);
        QNetworkReply *preparePatchRequest(QNetworkRequest &request, const QVariant &data);
        void handleResponse(QNetworkReply *reply);

        QNetworkAccessManager *m_manager;
        QList<QNetworkReply *> m_currentReplies;
    };
} // namespace _Kits

// 示例 1：发送 JSON 类型 POST 请求
// auto client = new _Kits::HttpClient(this);

// QVariantMap jsonData;
// jsonData["username"] = "admin";
// jsonData["password"] = "123456";

// client->sendRequest(
//     _Kits::HttpClient::RequestType::POST,
//     "https://example.com/api/login",
//     QJsonObject::fromVariantMap(jsonData),
//     {{"User-Agent", "MyHttpClient/1.0"}},
//     5000);

// connect(client, &HttpClient::requestFinished, [](const QByteArray &data) {
//     qDebug() << "Response:" << data;
// });

// connect(client, &HttpClient::requestError, [](const QString &msg) {
//     qWarning() << "Request failed:" << msg;
// });

// 示例 2：上传文件（带额外字段）
// auto client = new _Kits::HttpClient(this);

// QMap<QString, QString> formData;
// formData["token"] = "abc123";
// formData["user_id"] = "42";

// client->uploadFile(
//     "https://example.com/api/upload",
//     "C:/Users/xxx/Desktop/test.jpg",
//     formData,
//     {{"Authorization", "Bearer xyz"}});

// connect(client, &HttpClient::requestFinished, [](const QByteArray &data) {
//     qDebug() << "Upload response:" << data;
// });

// connect(client, &HttpClient::requestError, [](const QString &msg) {
//     qWarning() << "Upload failed:" << msg;
// });
