#include "CsvToSqlService.h"
#include "kits/common/log/CRossLogger.h"
#include <cmath>
#include "kits/common/system_config/SystemConfig.h"

using namespace _Kits;
using namespace _Kits::_Orm;
_Kits::csv_to_sql_service::csv_to_sql_service()
{
    _mb_isstop.store(false);
}

_Kits::csv_to_sql_service::~csv_to_sql_service() noexcept
{
}

bool _Kits::csv_to_sql_service::init(const YAML::Node &config)
{
    m_sqlTables_.insert(_Kits::_Orm::line_data::tableName());

    auto str_config_path = config["csv_path"].as<std::string>();

    if (str_config_path.empty())
    {
        return false;
    }

    // 加载配置文件
    auto &sysConfig = _Kits::SystemConfig::instance();
    auto loadFilePath = sysConfig.configVersionPath() + str_config_path;
    YAML::Node radar_config = _Kits::SystemConfig::instance().loadYamlFile(loadFilePath);

    if (_mb_isstop.load())
    {
        return false;
    }

    for (auto node : radar_config["csv_info"])
    {
        auto filePath = sysConfig.configVersionPath() + node["csv_path"].as<std::string>();
        auto tableName = node["table_name"].as<std::string>();

        if (!m_sqlTables_.contains(QString::fromStdString(tableName)))
        {
            qWarning() << "Table not exists:" << QString::fromStdString(tableName);
            continue;
        }

        mvec_csvPath_tableName[filePath] = tableName;
    }

    return true;
}

bool _Kits::csv_to_sql_service::start()
{
    for (const auto &[csvpath, table] : mvec_csvPath_tableName)
    {
        QList<QStringList> csvDataRef;

        if (!readCSVFile(csvpath, QString::fromStdString(table), csvDataRef))
        {
            LogInfo("Read CSV file failed: " + csvpath);
            continue;
        }

        if (table == _Kits::_Orm::line_data::tableName())
        {
            processTableData<_Kits::_Orm::line_data>(table, csvDataRef);
        }
    }

    return true;
}

bool _Kits::csv_to_sql_service::stop()
{
    _mb_isstop.store(true);
    return true;
}

bool csv_to_sql_service::readCSVFile(const std::string &filePath, const QString &tableName, QList<QStringList> &csvData)
{
    m_csv_reader_.readFile(filePath, csvData);

    if (csvData.isEmpty())
    {
        return false;
    }

    return true;
}

bool csv_to_sql_service::saveSqlData()
{

    return true;
}

template <typename T>
QList<T> csv_to_sql_service::exchangeData(const std::string &tablename, const QList<QStringList> &data)
{
    QList<T> result;

    if (!m_sqlTables_.contains(QString::fromStdString(tablename)))
    {
        qWarning() << "Table not exists:" << QString::fromStdString(tablename);
        return result;
    }

    try
    {
        for (const auto &row : data)
        {
            if (row.isEmpty())
                continue;

            if (row.size() < 16)
            {
                LogError("Row size is too small to process line_data:{} ", QString::number(row.size()).toStdString());
                break;
            }

            if constexpr (std::is_same_v<T, line_data>)
            {
                static int id = -1;

                QString kilo = row[7];
                if (kilo == "公里标")
                {
                    continue;
                }
                QString tagid = row[14];
                int station_id = row[15].isEmpty() ? -1 : row[15].toInt();

                float span = row[5].isEmpty() ? -1.0f : std::ceil(row[5].toFloat() * 10.0f) / 10.0f;

                QString structure = row[6].isEmpty() ? "" : row[6];

                int line_dir = row[10] == "上行" ? 0 : row[10] == "下行" ? 1 : -1;

                bool valid = true;
                if (row[0].isEmpty())
                {
                    LogError("line_name is empty, in the {} ", id);
                    valid = false;
                }
                if (row[1].isEmpty())
                {
                    LogError("station_name is empty, in the {} ", id);
                    valid = false;
                }
                if (row[3].isEmpty())
                {
                    LogError("maoduan_name is empty, in the {} ", id);
                    valid = false;
                }
                if (kilo.isEmpty())
                {
                    LogError("kilo_meter is empty, in the {} ", id);
                    valid = false;
                }
                if (row[4].isEmpty())
                {
                    LogError("pole_name is empty, in the {} ", id);
                    valid = false;
                }
                if (line_dir == -1)
                {
                    LogError("line_dir is invalid, in the {} ", id);
                    valid = false;
                }

                if (!valid)
                {
                    continue;
                }

                result.append({++id,           // auto increment id
                               tagid,          // tag_id
                               row[0],         // station_name
                               row[1],         // line_name
                               row[3],         // maoduan_name (锚段名)
                               row[4],         // pole_name
                               kilo.toFloat(), // kilo_meter (公里标)
                               line_dir,       // line_dir
                               span,           // span (跨距)
                               structure,      // structure (结构)
                               station_id,     // station_id (站区ID)
                               QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"),
                               QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")});
            }
            else
            {
                return false;
            }
        }
    }
    catch (const std::exception &e)
    {
        qCritical() << "Data conversion failed:" << e.what();
        return QList<T>();
    }

    return result;
}

template <typename T>
void _Kits::csv_to_sql_service::processTableData(const std::string &tableName, const QList<QStringList> &csvData)
{
    if constexpr (std::is_same_v<T, void>)
    {
        // 终止条件
        qWarning() << "Unsupported table type:" << QString::fromStdString(tableName);
    }
    else
    {
        if (tableName == T::tableName().toStdString())
        {
            auto data = exchangeData<T>(tableName, csvData);
            if (!data.isEmpty())
            {
                data.removeAt(0);
                m_sql_op_.insertSqlData<T>(data);
            }
        }
        else
        {
            return;
        }
    }
}
