#pragma once

#include "kits/common/read_csv/ReadCSVFile.h"
#include "sqloi.hpp"
#include <QObject>
#include <qobject.h>
#include <yaml-cpp/yaml.h>

namespace _Kits
{
    class csv_to_sql_service : public QObject
    {
        Q_OBJECT

      public:
        csv_to_sql_service();
        virtual ~csv_to_sql_service() noexcept; // 显式声明noexcept

        bool init(const YAML::Node &config);
        bool start();
        bool stop();

      private:
        ReadCSVFile m_csv_reader_;

        sqloi m_sql_op_;
        // 读取出的sql表数据
        QSet<QString> m_sqlTables_;

        std::atomic_bool _mb_isstop;

        std::unordered_map<std::string, std::string> mvec_csvPath_tableName;

      private:
        bool readCSVFile(const std::string &filePath, const QString &tableName, QList<QStringList> &csvData);

        bool saveSqlData();

        template <typename T>
        QList<T> exchangeData(const std::string &tablename, const QList<QStringList> &data);

        template <typename T>
        void processTableData(const std::string &tableName, const QList<QStringList> &csvData);
    };

} // namespace _Kits