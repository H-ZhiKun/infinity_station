#include "FtpClient.h"
#include "kits/common/log/CRossLogger.h"
#include <QRegularExpression>
#include <QFile>
namespace _Kits
{  

FtpClient::FtpClient(QObject *parent)
{


}
FtpClient::~FtpClient() noexcept
{
   

}
bool FtpClient::start()
{
    m_cmdSocket = new QTcpSocket(this);
    connect(m_cmdSocket, &QTcpSocket::connected, this, &FtpClient::onConnected);
    connect(m_cmdSocket, &QTcpSocket::disconnected, this, &FtpClient::onDisconnected);
    connect(m_cmdSocket, &QTcpSocket::readyRead, this, &FtpClient::onReadyRead);

    return true;

}
bool FtpClient::connectToServer(const QString &server, int port, const QString &username, const QString &password)
{

    m_cmdSocket->connectToHost(server, port);
    if(m_cmdSocket->waitForConnected())
    {
        if(m_cmdSocket->waitForReadyRead())
        {
            QString response = m_cmdSocket->readAll();
            return response.startsWith("220");
        }
    }
    return false;

}
void FtpClient::sendCommand(const QString &command)
{
    m_cmdSocket->write((command + "\r\n").toUtf8());
    m_cmdSocket->waitForBytesWritten();
}
void FtpClient::onConnected()
{
    if(m_cmdSocket->waitForReadyRead(3000))
    {
        QByteArray response = m_cmdSocket->readAll();
        if(response.startsWith("220"))
        {
            bconnect_ = true;
           // emit connected(); // Emit a signal to indicate successful connection
        }
        else
        {
            //logError("Unexpected response from server: {}", response.toStdString());
            bconnect_ = false;
            disconnect(); // Disconnect if the response is not as expected
        }
      
    }

}
 void FtpClient::onDisconnected()
 {
    if (bconnect_)
    {
        bconnect_ = false;
    }
    //判断是否启动重连
 }

 void FtpClient::onReadyRead()
 {
    QByteArray response = m_cmdSocket->readAll();

 }
 void FtpClient::sendUsername(const QString &username)
 {
    QString cmd = QString("USER %1\r\n").arg(username);
    QByteArray cmdBytes = cmd.toUtf8();
    qint64 bytesWritten = m_cmdSocket->write(cmdBytes);
     if (bytesWritten == -1 || bytesWritten == 0) {
        int errCode = m_cmdSocket->error();
        qWarning() << "发送 USER 命令失败，错误码:" << errCode
                   << m_cmdSocket->errorString();
        //return errCloseCmdSocket();
    }

    // 确保数据被发送出去
    if (!m_cmdSocket->waitForBytesWritten(3000)) {
        qWarning() << "waitForBytesWritten 超时";
        //return errCloseCmdSocket();
    }

    // 接收服务器响应（预期 331）
    //return recvAllInOne(331);


 }
 void FtpClient::sendPassword(const QString &password)
 {
    QString cmd = QString("PASS %1\r\n").arg(password);
    QByteArray cmdBytes = cmd.toUtf8();
    qint64 bytesWritten = m_cmdSocket->write(cmdBytes);
    if (bytesWritten == -1 || bytesWritten == 0)
    {
        int errCode = m_cmdSocket->error();
        qWarning() << "发送 PASS 命令失败，错误码:" << errCode
                   << m_cmdSocket->errorString();
      
    }

}

bool FtpClient::setTypeI()
{
    QString cmd = QString("TYPE I\r\n");
    m_cmdSocket->write(cmd.toUtf8());

}
 bool FtpClient::setPassiveMod()
 {
    QString cmd = QString("PASV\r\n");
    qint64 bytesSent= m_cmdSocket->write(cmd.toUtf8());
    if (bytesSent <= 0) 
    {
        qDebug() << "send(PASV) failed, socket error:" << m_cmdSocket->errorString();
        return false;
    }
    if (!m_cmdSocket->waitForReadyRead(3000)) 
    {
        qDebug() << "No response from FTP server for PASV command.";
        return false;
    }
    QByteArray response = m_cmdSocket->readAll();
    qDebug() << "PASV response:" << response;

    if (!parsePasvResponse(response, ip, port_))
    {
        return false;
    }
    return true;


 }
bool FtpClient::parsePasvResponse(const QByteArray &response, QString &ip, uint16_t &port)
{
    QRegularExpression regex(R"(\((\d+,\d+,\d+,\d+,\d+,\d+)\))");
    QRegularExpressionMatch match = regex.match(QString::fromUtf8(response));
       if (!match.hasMatch()) {
        qDebug() << "Invalid PASV response format.";
        return false;
    }
     QStringList parts = match.captured(1).split(",");
    if (parts.size() != 6) {
        qDebug() << "Malformed PASV address.";
        return false;
    }
    bool ok = true;
    int n0 = parts[0].toInt(&ok); if (!ok) return false;
    int n1 = parts[1].toInt(&ok); if (!ok) return false;
    int n2 = parts[2].toInt(&ok); if (!ok) return false;
    int n3 = parts[3].toInt(&ok); if (!ok) return false;
    int n4 = parts[4].toInt(&ok); if (!ok) return false;
    int n5 = parts[5].toInt(&ok); if (!ok) return false;

    ip = QString("%1.%2.%3.%4").arg(n0).arg(n1).arg(n2).arg(n3);
    port = static_cast<quint16>(n4 * 256 + n5);

    if (port == 0 || ip.isEmpty()) {
        qDebug() << "Parsed invalid IP or port.";
        return false;
    }

    return true;



}
bool FtpClient::uploadFile(const QString &url, const QString &filePath)
{

    //设置二进制模式
    

    QString cmd = QString("STOR %s\r\n").arg(url);
    qint64 bytesSent = m_cmdSocket->write(cmd.toUtf8());
    if(bytesSent < 0)
    {
        qDebug() << "send(RETR) failed, socket error:" << m_cmdSocket->errorString();
        return false;
    }
    if(!m_cmdSocket->waitForReadyRead(3000))
    {
        qDebug() << "No response from FTP server for RETR command.";
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"打开文件失败";
        return false;
    }
    while (!file.atEnd())
    {
        QByteArray data = file.read(FTP_CONTROL_MSG_MAX);
        qint64 bytesWritten = m_cmdSocket->write(data);
        if (bytesWritten != data.size())
        {
            qDebug() << "send(STOR) failed, socket error:" << m_cmdSocket->errorString();
            file.close();
            return false;
        }
    }

    if (!m_cmdSocket->waitForBytesWritten(3000)) 
    {
        qWarning() << "等待写入超时:" << m_cmdSocket->errorString();
        file.close();
        m_cmdSocket->disconnectFromHost();
        delete m_cmdSocket;
        m_cmdSocket = nullptr;
        return false;
    }
    file.close();

    return true;


}
}// Implementation of FtpClient methods

