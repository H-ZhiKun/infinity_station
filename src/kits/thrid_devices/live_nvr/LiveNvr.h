// LiveNVR.h
#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QFile>
#include <QTimer>
#include <QCryptographicHash>
#include <yaml-cpp/yaml.h>
#include <string>
#include "tis_global/Struct.h"
#include "kits/communication/http_client/HttpClient.h"
#include <QElapsedTimer>

namespace _Kits
{
class LiveNvr : public QObject
{
    Q_OBJECT

  public:
    explicit LiveNvr();
    ~LiveNvr();

    bool init(const TIS_Info::LogInfo &config);
    void close();

    bool checkLogin();
    QString getCameraIPByChannel(long lChannel);
    long getChannelByCameraIP(const QString &strCamreaID);

    bool loadInVideInfoOfOneChannel(const TIS_Info::NvrDataInfo &stInfo);
    bool downloadOneFileBy(const TIS_Info::NvrDataInfo &stInfo);
    time_t TimeStringToDatetime(const QString& strTime);
    time_t QDateTimeToTimeT(const QDateTime& dateTime);

  signals:
    void channelInfoReceived(int);
    void videoQueryFinished(bool success);
    void sendDownLoadVecInfo(const QVector<TIS_Info::NvrDataInfo>&);

  private slots:
    void handleChannelsConfigReply(const QByteArray &responseData);
    void handleVideoQueryResponse(const QByteArray &responseData);
    void handleDownloadResponse(const QByteArray &responseData);

  private:
    bool doLogin();
    QString formatDateTime(const QDateTime &datetime) const;
    QDateTime parseDateTimeString(const QString &strTime) const;
    std::unique_ptr<_Kits::HttpClient> m_httpClient = nullptr;
    TIS_Info::LogInfo m_loginInfo;
    QString m_currentCameraID;
    QString m_struSleepTime_Video;
    QString m_strUrl;
    QString m_strToken;
    QFile *m_currentDownloadFile;
    long lFindedChannel;                           // 通道号
    QVector<TIS_Info::NvrDataInfo> m_vecVideoInfo; // VideoInfovector
    TIS_Info::NvrDataInfo m_currentVideoQueryInfo; // VideoInfo
    TIS_Info::NvrDataInfo m_currentDownloadInfo;
    QElapsedTimer m_videoQueryTimer;
    QFile m_downloadFile;

};
}