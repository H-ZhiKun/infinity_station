#include "TestDatabase.h"
#include "kits/common/database/CppBatis.h"
#include "kits/common/database/connections/DatabaseConnections.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/common/log/CRossLogger.h"
#include <cstddef>
#include <json/value.h>
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qobjectdefs.h>
#include <vector>
#include "service/AppFramework.h"
#include "tis_global/Function.h"

namespace _Controllers
{
    using namespace _Kits::_Orm;
    void TestDatabase::testCURD(TIS_Info::QmlCommunication::ForQmlSignals ForQmlSignals, const QVariant &)
    {
        // insert();
        // select();
    }
    void TestDatabase::testSqlite()
    {
        // "testdb")); 目前没有对sqlite支持orm,所以请使用CppBatis中的普通接口
        // static int count = 0;
        // count++;
        // QString dbName = QString("test_%1.db").arg(count);
        // _Kits::CppBatis<_Kits::SqliteTag>::execSql(dbName,
        //                                            "CREATE TABLE IF NOT EXISTS natural_data ("
        //                                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        //                                            "timestamp TEXT NOT NULL)");

        // std::vector<QVariantMap> vMapData;
        // for (size_t i = 0; i < 5; i++)
        // {
        //     QVariantMap map;
        //     map["timestamp"] = QDateTime::currentDateTime().toString();
        //     vMapData.push_back(map);
        //     // 测试单挑插入
        //     _Kits::CppBatis<_Kits::SqliteTag>::insert(dbName, "natural_data", map);
        // }
        // // 测试批量插入
        // _Kits::CppBatis<_Kits::SqliteTag>::insert(dbName, "natural_data", vMapData);
    }
    void TestDatabase::insert()
    {
        // // 插入
        // std::vector<radar_data> lvObj;
        // int batchSize = 10;
        // lvObj.reserve(batchSize);
        // for (int i = 0; i < batchSize; i++)
        // {
        //     radar_data data;
        //     Json::Value jsRoot;
        //     Json::Value jsdata;
        //     jsdata["x"] = i;
        //     jsdata["y"] = i;
        //     jsRoot["points"].append(jsdata);
        //     data.location_id = i;
        //     data.points = jsRoot.toStyledString().c_str();
        //     lvObj.emplace_back(std::move(data));
        // }
        // _Kits::LogDebug("sql insert begin");
        // _Kits::SqlInsert<radar_data> insert;
        // insert.insert(lvObj).exec();
        // int nums = insert.getNumAffected();
        // _Kits::LogDebug("sql insert end {}", nums);
    }
    void TestDatabase::select()
    {

        // 获取第1页，每页1条记录，按时间倒序，也就是获取最新的一条记录
        auto selector = _Kits::SqlSelect<radar_data>();
        selector.select({"id", "points"})
            .where("id", _Kits::OperatorComparison::LessThan, 1000)
            .orderBy("id", false)
            .paginate(1, 10)
            .exec();

        // 处理查询结果
        auto datas = selector.getResults();
        for (const auto &data : datas)
        {
            qDebug() << "id:" << data.id << data.points;
        }
    }
} // namespace _Controllers