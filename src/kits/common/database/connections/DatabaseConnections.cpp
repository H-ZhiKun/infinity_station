#include "DatabaseConnections.h"
#include "kits/common/log/CRossLogger.h"
#include <QSqlQuery>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

namespace _Kits
{

    template <typename Tag>
    bool DatabaseConnections<Tag>::start(const DatabaseConfig &config)
    {
        std::lock_guard lock(m_mutex);
        m_config = config;
        return true;
    }

    template <typename Tag>
    void DatabaseConnections<Tag>::stop()
    {
        std::lock_guard lock(m_mutex);
        for (const auto &name : m_connectionNames)
            QSqlDatabase::removeDatabase(name);
        m_connectionNames.clear();
    }

    template <typename Tag>
    QSqlDatabase DatabaseConnections<Tag>::getConnection(const QString &dbName)
    {
        if constexpr (is_postgre<Tag>::value)
            return getConnectionImpl(m_config.dbName);
        else if constexpr (is_sqlite<Tag>::value)
            return getConnectionImpl(m_config.dbPath + "/" + dbName);
        else
            return {};
    }

    template <typename Tag>
    QSqlDatabase DatabaseConnections<Tag>::getConnectionImpl(const QString &dbName)
    {
        QString connName = makeConnectionName(dbName);
        {
            std::lock_guard lock(m_mutex);
            if (m_connectionNames.count(connName))
            {
                QSqlDatabase db = QSqlDatabase::database(connName);
                if (!db.isOpen())
                {
                    m_connectionNames.erase(connName);
                    QSqlDatabase::removeDatabase(connName);
                }
                return db;
            }
        }

        QSqlDatabase db = QSqlDatabase::addDatabase(m_config.driver, connName);
        db.setHostName(m_config.host);
        db.setPort(m_config.port);
        db.setDatabaseName(dbName);
        db.setUserName(m_config.user);
        db.setPassword(m_config.password);

        if (!db.open())
        {
            LogError("[数据库连接] 打开失败: {}", db.lastError().text().toStdString());
            QSqlDatabase::removeDatabase(connName);
            return {};
        }

        std::lock_guard lock(m_mutex);
        m_connectionNames.insert(connName);
        return db;
    }

    template <typename Tag>
    QString DatabaseConnections<Tag>::makeConnectionName(const QString &dbName)
    {
        return QString("conn_%1_%2").arg(dbName).arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    }

    template <typename Tag>
    bool DatabaseConnections<Tag>::createDatabase(const QString &dbName)
    {
        return createDatabaseImpl(dbName);
    }

    template <typename Tag>
    bool DatabaseConnections<Tag>::initSchema()
    {
        return initSchemaImpl();
    }

    template <typename Tag>
    template <typename T>
    std::enable_if_t<is_postgre<T>::value, bool> DatabaseConnections<Tag>::createDatabaseImpl(const QString &dbName)
    {
        Q_UNUSED(dbName)
        QString tempConnName = "pg_init_" + QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(m_config.driver, tempConnName);
            db.setHostName(m_config.host);
            db.setPort(m_config.port);
            db.setDatabaseName("postgres");
            db.setUserName(m_config.user);
            db.setPassword(m_config.password);

            if (!db.open())
            {
                LogError("[PostgreSQL] 数据库打开失败: {}", db.lastError().text().toStdString());
                return false;
            }

            QSqlQuery query(db);
            query.prepare("SELECT 1 FROM pg_database WHERE datname = :dbName");
            query.bindValue(":dbName", m_config.dbName);

            if (!query.exec())
            {
                LogError("[PostgreSQL] 数据库查询失败: {}", db.lastError().text().toStdString());
                return false;
            }

            if (!query.next())
            {
                if (!query.exec(QString("CREATE DATABASE \"%1\"").arg(m_config.dbName)))
                {
                    LogError("[PostgreSQL] 创建失败: {}", db.lastError().text().toStdString());
                    return false;
                }
                LogInfo("[PostgreSQL] 创建成功: {}", m_config.dbName.toStdString());
            }
        }
        QSqlDatabase::removeDatabase(tempConnName);
        return true;
    }

    template <typename Tag>
    template <typename T>
    std::enable_if_t<!is_postgre<T>::value, bool> DatabaseConnections<Tag>::createDatabaseImpl(const QString &)
    {
        return false;
    }

    template <typename Tag>
    template <typename T>
    std::enable_if_t<is_postgre<T>::value, bool> DatabaseConnections<Tag>::initSchemaImpl()
    {
        QFile sqlFile("./config/pginit.sql");
        if (!sqlFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            LogError("[SQL脚本]: 无法打开SQL脚本文件");
            return false;
        }

        QTextStream stream(&sqlFile);
        QString script = stream.readAll();
        sqlFile.close();

        if (script.isEmpty())
        {
            LogError("[SQL脚本]: 脚本为空");
            return false;
        }

        QRegularExpression commentRegex(R"((--[^\n]*|/\*.*?\*/))", QRegularExpression::DotMatchesEverythingOption);
        script.remove(commentRegex);
        QStringList statements = script.split(";", Qt::SkipEmptyParts);

        QSqlDatabase db = getConnection();
        QSqlQuery query(db);
        for (const QString &stmt : statements)
        {
            QString trimmed = stmt.trimmed();
            if (!trimmed.isEmpty() && !query.exec(trimmed))
            {
                LogError("[SQL]: 执行失败 = {}, 错误 = {}", trimmed.toStdString(), query.lastError().text().toStdString());
            }
        }

        LogInfo("[SQL脚本]: 执行完毕");
        return true;
    }

    template <typename Tag>
    template <typename T>
    std::enable_if_t<!is_postgre<T>::value, bool> DatabaseConnections<Tag>::initSchemaImpl()
    {
        return false;
    }

    // 显式实例化
    template class DatabaseConnections<PostgreTag>;
    template class DatabaseConnections<SqliteTag>;

} // namespace _Kits
