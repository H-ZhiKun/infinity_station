// LiveNVR.cpp
#include "LiveNvr.h"
#include <QUrlQuery>
#include <QJSValue>
#include <QJSValueIterator>
#include <QJSEngine>
#include <QCryptographicHash> // 新增MD5支持头文件

namespace _Kits
{

LiveNvr::LiveNvr() 
{
    m_httpClient = std::make_unique<_Kits::HttpClient>();
}

LiveNvr::~LiveNvr()
{
    close();
}



bool LiveNvr::init(const TIS_Info::LogInfo &config)
{
    m_loginInfo = config;

    if (!m_loginInfo.mstr_ipAddr.isEmpty())
    {
        return(doLogin());
    }

    QObject::connect(m_httpClient.get(), &HttpClient::requestFinished, this, &LiveNvr::handleChannelsConfigReply);
    QObject::connect(m_httpClient.get(), &HttpClient::requestFinished, this, &LiveNvr::handleVideoQueryResponse);
}

void LiveNvr::close()
{
    if (m_currentDownloadFile && m_currentDownloadFile->isOpen())
    {
        m_currentDownloadFile->close();
        delete m_currentDownloadFile;
        m_currentDownloadFile = nullptr;
    }
}

bool LiveNvr::checkLogin()
{
    if (m_strToken.isEmpty())
    {
        qDebug() << "[LiveNVR] Token is empty, attempting to login...";
        bool loginResult = doLogin();
        if (!loginResult)
        {
            qWarning() << "[LiveNVR] Login failed";
            //emit loginStatusChanged(false, "Login failed");
            return false;
        }
        return true;
    }

    qDebug() << "[LiveNVR] Already logged in with valid token";
    //emit loginStatusChanged(true, "Already logged in");
    return true;
}

bool LiveNvr::doLogin()
{
    m_strUrl = "http://" + m_loginInfo.mstr_ipAddr + ":" + QString::number(m_loginInfo.mus_port);
    auto m_sTempUrl = m_strUrl + "/api/login";

    QVariantMap logindata;
    logindata["username"] = m_loginInfo.mstr_userName;

    // 新增MD5密码转换
    QByteArray passwordData = m_loginInfo.mstr_password.toUtf8();
    QByteArray md5Hash = QCryptographicHash::hash(passwordData, QCryptographicHash::Md5);
    QString md5Password = QString(md5Hash.toHex());
    logindata["password"] = md5Password;

    m_httpClient->sendRequest(_Kits::HttpClient::RequestType::POST,
                            m_sTempUrl,
                            logindata,
                            {{"Content-Type", "application/x-www-form-urlencoded"}},
                            5000);

    return true;

    QObject::connect(m_httpClient.get(), &HttpClient::requestFinished, [](const QByteArray &data) { qDebug() << "Response:" << data; });
    QObject::connect(m_httpClient.get(), &HttpClient::requestError, [](const QString &error) { qDebug() << "requestError:" << error; });
}

QString LiveNvr::getCameraIPByChannel(long lChannel)
{
    return m_strUrl + "/api/v1/getchannelsconfig/" + QString::number(lChannel);
}

long LiveNvr::getChannelByCameraIP(const QString &strCamreaID)
{
    // 1. Prepare the URL and parameters
    QString url = m_strUrl + "/api/v1/getchannelsconfig";

    // 2. Prepare query parameters (data)
    QVariantMap data;
    data["enable"] = "1";

    // 3. Prepare headers
    QVariantMap headers;
    headers["Cookie"] = QString("token=%1").arg(m_strToken);

    // 4. Send the request
    m_httpClient->sendRequest(_Kits::HttpClient::RequestType::GET, url, data, headers);

    // Store the camera ID for later processing
    m_currentCameraID = strCamreaID;

    QObject::connect(m_httpClient.get(), &HttpClient::requestFinished, this, &LiveNvr::handleChannelsConfigReply);
    QObject::connect(m_httpClient.get(), &HttpClient::requestError, [](const QString &error) { qDebug() << "requestError:" << error; });

    return -1;
}

bool LiveNvr::loadInVideInfoOfOneChannel(const TIS_Info::NvrDataInfo &stInfo)
{
    // 1. 准备请求参数
    QString strUrl = m_strUrl + "/api/v1/record/querydaily";
    // 2. 格式化时间
    char str[15];
    formatDateTime(stInfo.endTime);
    std::string strEndTime(str);
    strEndTime = strEndTime.substr(0, 8);

    // 3. 准备查询参数
    QVariantMap queryParams;
    queryParams["id"] = QString::number(stInfo.lChannel);
    queryParams["period"] = QString::fromStdString(strEndTime);

    // 4. 准备请求头
    QVariantMap headers;
    headers["Cookie"] = QString("token=%1").arg(m_strToken);

    // 5. 存储当前查询信息
    m_currentVideoQueryInfo = stInfo;
    m_videoQueryTimer.start();

    // 6. 发送异步请求
    m_httpClient->sendRequest(_Kits::HttpClient::RequestType::GET, strUrl, queryParams, headers);

    QObject::connect(m_httpClient.get(), &HttpClient::requestFinished, this, &LiveNvr::handleVideoQueryResponse);
    QObject::connect(m_httpClient.get(), &HttpClient::requestError, [](const QString &error) { qDebug() << "requestError:" << error; });

    return true; // 注意：实际结果将通过信号返回
}

bool LiveNvr::downloadOneFileBy(const TIS_Info::NvrDataInfo &stInfo)
{
    // 1. 转换时间格式
    char str[15];
    formatDateTime(stInfo.beginTime);
    QString strStartTime = QString::fromLocal8Bit(str);
    formatDateTime(stInfo.endTime);
    QString strEndTime = QString::fromLocal8Bit(str);

    // 2. 拼接URL
    QString strUrl = m_strUrl + "/api/v1/record/video/download/" + QString::number(stInfo.lChannel) + "/" + strStartTime + "/" + strEndTime;

    // 3. 准备请求头
    QVariantMap headers;
    headers["Cookie"] = QString("token=%1").arg(m_strToken);

    // 4. 存储文件信息用于回调
    m_currentDownloadInfo = stInfo;
    m_downloadFile.setFileName(stInfo.strPath);

    if (!m_downloadFile.open(QIODevice::WriteOnly))
    {
        qWarning() << "Failed to open file for writing:" << stInfo.strPath;
        return false;
    }

    // 5. 发送异步下载请求
    m_httpClient->sendRequest(_Kits::HttpClient::RequestType::GET, strUrl, QVariantMap(), headers);

    QObject::connect(m_httpClient.get(), &HttpClient::requestFinished, this, &LiveNvr::handleDownloadResponse);
    QObject::connect(m_httpClient.get(), &HttpClient::requestError, [](const QString &error) { qDebug() << "requestError:" << error; });

    return true; // 注意：实际结果将通过信号返回
}

QString LiveNvr::formatDateTime(const QDateTime &datetime) const
{
    return datetime.toString("yyyyMMddhhmmss");
}

QDateTime LiveNvr::parseDateTimeString(const QString &strTime) const
{
    return QDateTime::fromString(strTime, "yyyyMMddhhmmss");
}


// 处理获取通道号槽函数
void LiveNvr::handleChannelsConfigReply(const QByteArray &responseData)
{
    // Parse JSON response
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (doc.isNull())
    {
        emit channelInfoReceived(-1);
        return;
    }

    QJsonObject root = doc.object();
    QJsonObject liveQing = root["LiveQing"].toObject();
    QJsonObject body = liveQing["Body"].toObject();

    int channelCount = body["ChannelCount"].toInt();
    if (channelCount > 0)
    {
        QJsonArray channels = body["Channels"].toArray();
        for (const QJsonValue &channelValue : channels)
        {
            QJsonObject channel = channelValue.toObject();
            if (channel["IP"].toString() == m_currentCameraID)
            {
                int foundChannel = channel["Channel"].toInt();
                emit channelInfoReceived(foundChannel);
                return;
            }
        }
    }

    emit channelInfoReceived(-1);
}
void LiveNvr::handleVideoQueryResponse(const QByteArray &responseData)
{
    // 解析JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        emit videoQueryFinished(false);
        return;
    }

    QJsonArray list = doc.object()["list"].toArray();
    time_t tNewest = 0;

    foreach (const QJsonValue &item, list)
    {
        QJsonObject record = item.toObject();
        int duration = record["duration"].toInt();

        if (duration != 1800)
        {
            time_t tStartAt = TimeStringToDatetime(record["startAt"].toString());
            if (tStartAt != -1)
            {
                tStartAt += duration;
                if (tStartAt > tNewest)
                {
                    tNewest = tStartAt;
                }
            }
        }
    }

    // 检查是否超时
    if (m_videoQueryTimer.elapsed() > 2000)
    {
        emit videoQueryFinished(false);
        return;
    }

    // 比较时间
    if (tNewest > QDateTimeToTimeT(m_currentVideoQueryInfo.endTime))
    {
        // 写入 m_vecVideoInfo（类成员变量）
        m_vecVideoInfo.push_back(m_currentVideoQueryInfo);
        emit sendDownLoadVecInfo(m_vecVideoInfo);

        emit videoQueryFinished(true);
    }
    else
    {
        // 未找到，延迟后重新查询
        QTimer::singleShot(500, this, [this]() { loadInVideInfoOfOneChannel(m_currentVideoQueryInfo); });
    }
}

void LiveNvr::handleDownloadResponse(const QByteArray &responseData)
{
    // 1. 检查文件是否可写
    if (!m_downloadFile.open(QIODevice::WriteOnly))
    {
        qWarning() << "Failed to open file for writing:" << m_currentDownloadInfo.strPath;
        return;
    }

    // 2. 写入数据
    qint64 bytesWritten = m_downloadFile.write(responseData);
    m_downloadFile.close();

    // 3. 检查是否写入成功
    if (bytesWritten != responseData.size())
    {
        qWarning() << "Failed to write all data to file:" << m_currentDownloadInfo.strPath;
        return;
    }

    // 4. 成功完成
    qDebug() << "File downloaded successfully:" << m_currentDownloadInfo.strPath;
}

time_t LiveNvr::TimeStringToDatetime(const QString& strTime) {
    // 假设输入格式为 YYYYMMDDHHMMSS
    if (strTime.length() != 14) {
        qWarning() << "Invalid time string format. Expected YYYYMMDDHHMMSS.";
        return -1; // 返回错误码
    }

    // 解析字符串
    int year = strTime.mid(0, 4).toInt();
    int month = strTime.mid(4, 2).toInt();
    int day = strTime.mid(6, 2).toInt();
    int hour = strTime.mid(8, 2).toInt();
    int minute = strTime.mid(10, 2).toInt();
    int second = strTime.mid(12, 2).toInt();

    // 构造 QDateTime
    QDate date(year, month, day);
    QTime time(hour, minute, second);
    if (!date.isValid() || !time.isValid()) {
        qWarning() << "Invalid date or time components.";
        return -1;
    }

    QDateTime dateTime(date, time, Qt::UTC); // 指定时区（或 Qt::LocalTime）
    return dateTime.toSecsSinceEpoch(); // 返回 time_t
}

time_t LiveNvr::QDateTimeToTimeT(const QDateTime& dateTime)
{
    if (!dateTime.isValid()) {
        qWarning() << "Invalid QDateTime provided";
        return -1;
    }
    return static_cast<time_t>(dateTime.toSecsSinceEpoch());
}

}


