#pragma once
#include "SqlInsert.h"
#include "SqlSelect.h"
#include "SqlTypes.h"
#include "SqlUpdate.h"
#include "kits/common/database/connections/DatabaseConnections.h"
#include "kits/common/database/orm/TableStructs.h"
#include <qcontainerfwd.h>
#include <vector>

namespace _Kits
{

    template <typename DBDriver>
    struct SqlIdentifierWrapper;

    template <>
    struct SqlIdentifierWrapper<PostgreTag>
    {
        static QString quote(const QString &identifier)
        {
            return "\"" + identifier + "\"";
        }
    };

    template <>
    struct SqlIdentifierWrapper<SqliteTag>
    {
        static QString quote(const QString &identifier)
        {
            return "`" + identifier + "`";
        }
    };

    template <typename DBDriver = PostgreTag>
    class CppBatis
    {
      public:
        static QVariantList execSql(const QString &dbName, const QString &sql)
        {
            auto db = DatabaseConnections<DBDriver>::getConnection(dbName);
            if (!db.isOpen() || !db.isValid())
            {
                return {};
            }
            QSqlQuery query(db);

            if (!query.prepare(sql))
            {
                qDebug() << "SQL准备失败: " << query.lastError().text();
                qDebug() << sql;
                return {};
            }

            if (!query.exec())
            {
                qDebug() << "SQL执行失败: " << query.lastError().text();
                return {};
            }

            QVariantList resultList;

            // 检查是否有结果集（即是否为SELECT查询）
            if (query.isSelect())
            {
                while (query.next())
                {
                    QVariantMap recordMap;
                    QSqlRecord record = query.record();
                    for (int i = 0; i < record.count(); ++i)
                    {
                        recordMap.insert(record.fieldName(i), record.value(i));
                    }
                    resultList.append(recordMap);
                }
            }
            else
            {
                // 非查询操作，返回受影响的行数
                QVariantMap result;
                result.insert("rowsAffected", query.numRowsAffected());
                resultList.append(result);
            }

            return resultList;
        }
        // 单条插入
        static int insert(const QString &dbName, const QString &tableName, const QVariantMap &mapData)
        {
            auto sql = buildInsertStatement(tableName, mapData);
            std::unordered_map<QString, QVariant> mapBindValues;
            for (const auto &[key, value] : mapData.asKeyValueRange())
            {
                if (key != "id")
                {
                    mapBindValues[":" + key] = std::move(value);
                }
            }
            return execSingle(dbName, sql, mapBindValues);
        }
        // 批量插入插入
        static int insert(const QString &dbName,
                          const QString &tableName,
                          const std::vector<QVariantMap> &vMapData,
                          const std::unordered_map<QString, QSql::ParamType> &bindTypes)
        {
            auto sql = buildInsertStatement(tableName, vMapData.front());
            std::unordered_map<QString, QVariantList> mapBindValues;
            for (const auto &object : vMapData)
            {
                for (const auto &[key, value] : object.asKeyValueRange())
                {
                    if (key != "id")
                    {
                        mapBindValues[":" + key].append(std::move(value));
                    }
                }
            }
            return execBatch(dbName, sql, mapBindValues, bindTypes);
        }

      private:
        static QString buildInsertStatement(const QString &tableName, const QVariantMap &map)
        {
            QStringList fields, placeholders;
            for (const auto &key : map.keys())
            {
                if (key != "id")
                {
                    fields << SqlIdentifierWrapper<DBDriver>::quote(key);
                    placeholders << ":" + key;
                }
            }

            return QString("INSERT INTO %1 (%2) VALUES (%3)")
                .arg(SqlIdentifierWrapper<DBDriver>::quote(tableName))
                .arg(fields.join(", "))
                .arg(placeholders.join(", "));
        }

        static bool execSingle(const QString &dbName, const QString &sql, const std::unordered_map<QString, QVariant> &mapValues)
        {
            auto db = DatabaseConnections<DBDriver>::getConnection(dbName);
            if (!db.isOpen() || !db.isValid())
            {
                return false;
            }
            QSqlQuery query(db);
            if (!query.prepare(sql))
            {
                return false;
            }
            for (const auto &[key, value] : mapValues)
            {
                query.bindValue(key, value);
            }
            return query.exec();
        }

        static bool execBatch(const QString &dbName,
                              const QString &sql,
                              const std::unordered_map<QString, QVariantList> &mapValues,
                              const std::unordered_map<QString, QSql::ParamType> &bindTypes)
        {
            auto db = DatabaseConnections<DBDriver>::getConnection(dbName);
            if (!db.isOpen() || !db.isValid())
            {
                return false;
            }
            QSqlQuery query(db);
            if (!query.prepare(sql))
            {
                return false;
            }
            for (const auto &[key, values] : mapValues)
            {
                QSql::ParamType paramType = QSql::In;
                auto it = bindTypes.find(key);
                if (it != bindTypes.end())
                {
                    paramType = it->second;
                }
                query.bindValue(key, values, paramType);
            }

            db.transaction(); // 开始事务
            auto success = query.execBatch();
            if (!success)
            {
                db.rollback(); // 失败回滚
                return false;
            }
            success = db.commit();
            return success; // 成功提交
        }
    };

} // namespace _Kits
