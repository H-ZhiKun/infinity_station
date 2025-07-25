#include "DatabaseManager.h"
#include "kits/common/database/connections/DatabaseConnections.h"
#include "kits/common/log/CRossLogger.h"
#include <QFileInfo>
namespace _Modules
{
    using namespace _Kits;

    // 2 完成模块类型
    // 注册宏实现，如果将来要制作动态库时避免发生错误。
    DatabaseManager::DatabaseManager(QObject *parent) : ModuleBase(parent)
    {
    }

    DatabaseManager::~DatabaseManager() noexcept
    {
    }

    bool DatabaseManager::init(const YAML::Node &initConfig)
    {
        if (initConfig["postgresql"])
        {
            auto config = initConfig["postgresql"];
            DatabaseConfig dbConfig;
            dbConfig.driver = QString::fromStdString(config["driver"].as<std::string>());
            dbConfig.host = QString::fromStdString(config["host"].as<std::string>());
            dbConfig.port = config["port"].as<quint16>();
            dbConfig.dbName = QString::fromStdString(config["db_name"].as<std::string>());
            dbConfig.user = QString::fromStdString(config["user"].as<std::string>());
            dbConfig.password = QString("~postgres@");
            DatabaseConnections<PostgreTag>::start(dbConfig);
            if (DatabaseConnections<PostgreTag>::createDatabase(dbConfig.dbName))
            {
                DatabaseConnections<PostgreTag>::initSchema();
                LogInfo("[postgresql]: 数据库配置成功.");
                m_rdbms.push_back(dbConfig.driver);
                mp_csvToSqlService = std::make_unique<_Kits::csv_to_sql_service>();
                mp_csvToSqlService->init(config);
            }
        }

        if (initConfig["sqlite"])
        {
            auto config = initConfig["sqlite"];
            DatabaseConfig dbConfig;
            dbConfig.driver = QString::fromStdString(config["driver"].as<std::string>());
            dbConfig.dbPath = QString::fromStdString(config["db_path"].as<std::string>());
            DatabaseConnections<SqliteTag>::start(dbConfig);
            LogInfo("[sqlite]: 数据库配置成功.");
            namespace fs = std::filesystem;
            fs::path path(dbConfig.dbPath.toStdString());
            if (!fs::exists(path))
            {
                std::error_code ec;
                if (!fs::create_directories(path, ec))
                {
                    LogError("SQLite数据库路径不存在，且创建失败: {}", dbConfig.dbPath.toStdString());
                    return false;
                }
            }
            m_rdbms.push_back(dbConfig.driver);
        }

        return true;
    }

    bool DatabaseManager::start()
    {
        for (const auto &item : m_rdbms)
        {
            if (item == "QPSQL")
            {
                emit initStatus(TIS_Info::QmlCommunication::ForQmlSignals::database_state, QVariant::fromValue(true));
                mp_csvToSqlService->start();
            }
        }
        return true;
    }
    bool DatabaseManager::stop()
    {
        for (const auto &item : m_rdbms)
        {
            if (item == "QPSQL")
            {
                DatabaseConnections<PostgreTag>::stop();
            }
            if (item == "QSQLITE")
            {
                DatabaseConnections<SqliteTag>::stop();
            }
        }
        return true;
    }

} // namespace _Modules
