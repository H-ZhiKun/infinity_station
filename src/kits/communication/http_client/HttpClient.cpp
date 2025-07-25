#include "HttpClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QMimeDatabase>
#include <QHttpMultiPart>
#include <qfileinfo.h>
#include <QThread>
namespace _Kits
{
    HttpClient::HttpClient(QObject *parent) : QObject(parent), m_manager(new QNetworkAccessManager(this))
    {
    }

    HttpClient::~HttpClient() noexcept
    {
        cancelAllRequests();
        m_manager->deleteLater();
    }

    void HttpClient::sendRequest(RequestType type, const QString &url, const QVariantMap &data, const QVariantMap &headers, int timeout)
    {
        if (QThread::currentThread() == this->thread())
        {
            sendRawData(type, url, data, headers, timeout);
        }
        else
        {
            QMetaObject::invokeMethod(this,
                                      "sendRawData",
                                      Qt::QueuedConnection,
                                      Q_ARG(RequestType, type),
                                      Q_ARG(QString, url),
                                      Q_ARG(QVariantMap, data),
                                      Q_ARG(QVariantMap, headers),
                                      Q_ARG(int, timeout));
        }
    }

    void HttpClient::uploadFile(const QString &url, const QString &filePath, const QVariantMap &formData, const QVariantMap &headers)
    {
        if (QThread::currentThread() == this->thread())
        {
            doUploadFile(url, filePath, formData, headers);
        }
        else
        {
            QMetaObject::invokeMethod(this,
                                      "doUploadFile",
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, url),
                                      Q_ARG(QString, filePath),
                                      Q_ARG(QVariantMap, formData),
                                      Q_ARG(QVariantMap, headers));
        }
    }

    void HttpClient::sendRawData(RequestType type, const QString &url, const QVariantMap &data, const QVariantMap &headers, int timeout)
    {
        QNetworkRequest request;
        request.setUrl(QUrl(url));
        request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
        request.setTransferTimeout(timeout);

        // 设置请求头
        for (auto it = headers.constBegin(); it != headers.constEnd(); ++it)
        {
            request.setRawHeader(it.key().toUtf8(), it.value().toString().toUtf8());
        }

        QNetworkReply *reply = nullptr;

        switch (type)
        {
        case RequestType::GET:
            reply = m_manager->get(request);
            break;
        case RequestType::POST:
            reply = preparePostRequest(request, data);
            break;
        case RequestType::PUT:
            reply = preparePutRequest(request, data);
            break;
        case RequestType::DELETE:
            reply = m_manager->deleteResource(request);
            break;
        case RequestType::PATCH:
            reply = preparePatchRequest(request, data);
            break;
        }

        if (reply)
        {
            connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleResponse(reply); });

            connect(reply, &QNetworkReply::uploadProgress, this, [this](qint64 bytesSent, qint64 bytesTotal) {
                emit uploadProgress(bytesSent, bytesTotal);
            });

            connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesReceived, qint64 bytesTotal) {
                emit downloadProgress(bytesReceived, bytesTotal);
            });

            m_currentReplies.append(reply);
        }
    }

    void HttpClient::cancelAllRequests()
    {
        for (auto reply : m_currentReplies)
        {
            reply->abort();
            reply->deleteLater();
        }
        m_currentReplies.clear();
    }

    void HttpClient::doUploadFile(const QString &url, const QString &filePath, const QVariantMap &formData, const QVariantMap &headers)
    {
        QFile *file = new QFile(filePath);
        QString fileName = QFileInfo(filePath).fileName();
        if (!file->open(QIODevice::ReadOnly))
        {
            emit requestError(tr("Failed to open file"));
            file->deleteLater();
            return;
        }

        QMimeDatabase mimeDb;
        QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        // 添加文件部分
        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType.name());
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileName));
        filePart.setBodyDevice(file);
        file->setParent(multiPart);
        multiPart->append(filePart);

        // 添加表单数据
        for (auto it = formData.constBegin(); it != formData.constEnd(); ++it)
        {
            QHttpPart textPart;
            textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(it.key()));
            textPart.setBody(it.value().toString().toUtf8());
            multiPart->append(textPart);
        }

        QNetworkRequest request;
        request.setUrl(QUrl(url));

        // 设置请求头
        for (auto it = headers.constBegin(); it != headers.constEnd(); ++it)
        {
            request.setRawHeader(it.key().toUtf8(), it.value().toString().toUtf8());
        }

        QNetworkReply *reply = m_manager->post(request, multiPart);
        multiPart->setParent(reply);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleResponse(reply); });

        m_currentReplies.append(reply);
    }

    QNetworkReply *HttpClient::preparePostRequest(QNetworkRequest &request, const QVariant &data)
    {
        if (data.canConvert<QJsonObject>())
        {
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            QByteArray jsonData = QJsonDocument(data.toJsonObject()).toJson();
            return m_manager->post(request, jsonData);
        }
        else if (data.canConvert<QByteArray>())
        {
            return m_manager->post(request, data.toByteArray());
        }
        else if (data.canConvert<QString>())
        {
            return m_manager->post(request, data.toString().toUtf8());
        }
        else
        {
            return m_manager->post(request, QByteArray());
        }
    }

    QNetworkReply *HttpClient::preparePutRequest(QNetworkRequest &request, const QVariant &data)
    {
        if (data.canConvert<QJsonObject>())
        {
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            QByteArray jsonData = QJsonDocument(data.toJsonObject()).toJson();
            return m_manager->put(request, jsonData);
        }
        else if (data.canConvert<QByteArray>())
        {
            return m_manager->put(request, data.toByteArray());
        }
        else if (data.canConvert<QString>())
        {
            return m_manager->put(request, data.toString().toUtf8());
        }
        else
        {
            return m_manager->put(request, QByteArray());
        }
    }

    QNetworkReply *HttpClient::preparePatchRequest(QNetworkRequest &request, const QVariant &data)
    {
        if (data.canConvert<QJsonObject>())
        {
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            QByteArray jsonData = QJsonDocument(data.toJsonObject()).toJson();
            return m_manager->sendCustomRequest(request, "PATCH", jsonData);
        }
        else if (data.canConvert<QByteArray>())
        {
            return m_manager->sendCustomRequest(request, "PATCH", data.toByteArray());
        }
        else if (data.canConvert<QString>())
        {
            return m_manager->sendCustomRequest(request, "PATCH", data.toString().toUtf8());
        }
        else
        {
            return m_manager->sendCustomRequest(request, "PATCH", QByteArray());
        }
    }

    void HttpClient::handleResponse(QNetworkReply *reply)
    {
        m_currentReplies.removeAll(reply);

        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray responseData = reply->readAll();
            emit requestFinished(responseData);
        }
        else
        {
            emit requestError(reply->errorString());
        }

        reply->deleteLater();
    }
} // namespace _Kits