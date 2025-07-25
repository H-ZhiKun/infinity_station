#pragma once

#include <QSqlDatabase>
#include <QString>
#include <QSqlError>
#include <unordered_set>
#include <mutex>
#include <type_traits>

namespace _Kits
{
    struct PostgreTag
    {
    };
    struct SqliteTag
    {
    };

    template <typename T>
    struct is_postgre : std::false_type
    {
    };
    template <>
    struct is_postgre<PostgreTag> : std::true_type
    {
    };

    template <typename T>
    struct is_sqlite : std::false_type
    {
    };
    template <>
    struct is_sqlite<SqliteTag> : std::true_type
    {
    };

    struct DatabaseConfig
    {
        QString driver;
        QString host;
        quint16 port = 0;
        QString dbPath;
        QString dbName;
        QString user;
        QString password;
    };

    template <typename Tag>
    class DatabaseConnections
    {
      public:
        static bool start(const DatabaseConfig &config);
        static void stop();
        static QSqlDatabase getConnection(const QString &dbName = "");
        static bool createDatabase(const QString &dbName);
        static bool initSchema();

      private:
        DatabaseConnections() = default;
        static QSqlDatabase getConnectionImpl(const QString &dbName);
        static QString makeConnectionName(const QString &dbName);

        template <typename T = Tag>
        static std::enable_if_t<is_postgre<T>::value, bool> createDatabaseImpl(const QString &dbName);

        template <typename T = Tag>
        static std::enable_if_t<!is_postgre<T>::value, bool> createDatabaseImpl(const QString &dbName);

        template <typename T = Tag>
        static std::enable_if_t<is_postgre<T>::value, bool> initSchemaImpl();

        template <typename T = Tag>
        static std::enable_if_t<!is_postgre<T>::value, bool> initSchemaImpl();

      private:
        inline static DatabaseConfig m_config;
        inline static std::unordered_set<QString> m_connectionNames;
        inline static std::mutex m_mutex;
    };
} // namespace _Kits
