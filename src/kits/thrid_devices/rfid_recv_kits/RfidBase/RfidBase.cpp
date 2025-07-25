#include "RfidBase.h"
#include <QSqlDatabase> // 数据库连接
#include <QSqlQuery>    // SQL查询执行
#include <QSqlError>    // 数据库错误处理
#include <QDateTime>    // 时间戳生成
#include <QDir>         // 目录操作
#include <QFileInfo>    // 文件路径处理
#include <QDebug>       // 调试输出
#include <chrono>

bool _Kits::RfidBase::saveNaturalData(const QString &save_naturalData_path,
                                      const std::vector<uint8_t> &naturalData,
                                      const QString &parsed_data)
{
    // 确保数据库文件目录存在
    QFileInfo dbPathInfo(save_naturalData_path);
    QDir dbDir = dbPathInfo.absoluteDir();
    if (!dbDir.exists())
    {
        if (!dbDir.mkpath("."))
        {
            qWarning() << "Failed to create database directory:" << dbDir.path();
            return false;
        }
    }

    // 打开SQLite数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(save_naturalData_path);

    if (!db.open())
    {
        qWarning() << "Failed to open database:" << db.lastError().text();
        return false;
    }

    // 创建表（如果不存在）
    QSqlQuery createTableQuery(db);
    if (!createTableQuery.exec("CREATE TABLE IF NOT EXISTS natural_rfid_data ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "timestamp TEXT NOT NULL, "
                               "natural_data BLOB NOT NULL,"
                               "parsed_data TEXT)"))
    {
        qWarning() << "Failed to create table:" << createTableQuery.lastError().text();
        db.close();
        return false;
    }

    // 准备插入语句
    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO natural_rfid_data (timestamp, natural_data, parsed_data) "
                        "VALUES (:timestamp, :natural_data, :parsed_data)");

    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 计算从 1970/1/1 到现在的持续时间（以毫秒为单位）
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    // 获取毫秒级时间戳
    long long timestamp = duration.count();
    insertQuery.bindValue(":timestamp", QString::fromStdString(std::to_string(timestamp)));

    QByteArray blobData(reinterpret_cast<const char *>(naturalData.data()), naturalData.size());
    insertQuery.bindValue(":natural_data", blobData);
    insertQuery.bindValue(":parsed_data", parsed_data); // 绑定解析后的数据

    // 执行插入
    if (!insertQuery.exec())
    {
        qWarning() << "Failed to insert data:" << insertQuery.lastError().text();
        db.close();
        return false;
    }

    db.close();
    return true;
}
