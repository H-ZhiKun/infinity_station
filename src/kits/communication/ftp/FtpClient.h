#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QTcpSocket>
#include <QHostAddress>
#include <atomic>
namespace _Kits
{
class FtpClient : public QObject
{
   Q_OBJECT
public:
    explicit FtpClient(QObject *parent = nullptr);
    virtual ~FtpClient() noexcept;

    bool start();
    bool getConnect();


    bool connectToServer(const QString &server, int port, const QString &username, const QString &password);
    void sendCommand(const QString &command);
    
    void sendUsername(const QString &username); //用户名
    void sendPassword(const QString &password);//密码
    bool setTypeI();
    bool setPassiveMod();
    void downloadFile(const QString &url, const QString &filePath);
    bool uploadFile(const QString &url, const QString &filePath);
    bool parsePasvResponse(const QByteArray &response, QString &ip, uint16_t &port);
signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpSocket *m_cmdSocket;
    QString ip;
    uint16_t port_;
    std::atomic_bool bconnect_ = false;
    const int FTP_CONTROL_MSG_MAX = 1460;

};


}