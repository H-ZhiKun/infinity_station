#include "OfflineSqlOp.h"


using namespace _Kits;
using namespace _Modules;
using namespace _Kits::_Orm;


// 获取前后时间戳
std::pair<QString, QString> OfflineSqlOp::GetTimeRange(int taskId)
{
    QString str_start_time;
    QString str_end_time;


    if (_Kits::SqlSelect<radar_data>().select(QStringList()) // 开始时间
        .where(QString("task_id"), OperatorComparison::Equal, QVariant(taskId))
        .orderBy(QString("id"))
        .paginate(1, 1)
        .exec())
    {
        auto results = _Kits::SqlSelect<radar_data>().getResults();
        str_start_time = results.begin()->created_time;
    }

    if (_Kits::SqlSelect<radar_data>().select(QStringList()) // 结束时间
    .where(QString("task_id"), OperatorComparison::Equal, QVariant(taskId))
    .orderBy(QString("id"), false)
    .paginate(1, 1)
    .exec())
    {
        auto results = _Kits::SqlSelect<radar_data>().getResults();
        str_end_time = results.begin()->created_time;
    }

    return {str_start_time, str_end_time};
}


// 获取超限所有信息
std::vector<radar_over_data> OfflineSqlOp::GetOverrunDataByTask(int taskId)
{
    if (_Kits::SqlSelect<radar_over_data>().select(QStringList())
        .where(QString("task_id"), OperatorComparison::Equal, QVariant(taskId))
        .exec())
    {
        return _Kits::SqlSelect<radar_over_data>().getResults();
    }

    return {};
}


// 根据id获取定位信息
std::vector<location_data> OfflineSqlOp::GetLocationDataById(int locationId)
{
    if (_Kits::SqlSelect<location_data>().select(QStringList())
        .where(QString("id"), OperatorComparison::Equal, QVariant(locationId))
        .paginate(1, 1)
        .exec())
    {
        return _Kits::SqlSelect<location_data>().getResults();
    }
    
    return {};
}

std::vector<radar_data> OfflineSqlOp::GetDataByTime(QDateTime &time, int range)
{
    if (_Kits::SqlSelect<radar_data>().select(QStringList())
        .where(QString("created_time"), OperatorComparison::GreaterEqual, QVariant(time.toString("yyyy-MM-dd HH:mm:ss.zzz")))
        .paginate(1, range)
        .exec())
    {
        return _Kits::SqlSelect<radar_data>().getResults();
    }
    
    
    return {};
}
std::vector<radar_data> GetDataByTimeRange(QDateTime &start, QDateTime &end, int range)
{
    if (_Kits::SqlSelect<radar_data>().select(QStringList())
        .where(QString("created_time"), OperatorComparison::GreaterEqual, QVariant(start.toString("yyyy-MM-dd HH:mm:ss.zzz")))
        .where(QString("created_time"), OperatorComparison::LessEqual, QVariant(end.toString("yyyy-MM-dd HH:mm:ss.zzz")))
        .paginate(1, range)
        .exec())
    {
        return _Kits::SqlSelect<radar_data>().getResults();
    }

    return {};
}

std::vector<task_data> OfflineSqlOp::GetTaskTable()
{
    if (_Kits::SqlSelect<task_data>().select(QStringList())
        .exec())
    {
        return _Kits::SqlSelect<task_data>().getResults();
    }

    return {};
}
std::vector<location_data> OfflineSqlOp::GetLocationDataByTime(QDateTime &time, int range)
{
    if (_Kits::SqlSelect<location_data>().select(QStringList())
        .where(QString("created_time"), OperatorComparison::GreaterEqual, QVariant(time.toString("yyyy-MM-dd HH:mm:ss.zzz")))
        .paginate(1, range)
        .exec())
    {
        return _Kits::SqlSelect<location_data>().getResults();
    }

    return {};
}
std::vector<radar_over_data> OfflineSqlOp::GetOverrunDataByTime(QDateTime &time, int range)
{
    if (_Kits::SqlSelect<radar_over_data>().select(QStringList())
        .where(QString("created_time"), OperatorComparison::GreaterEqual, QVariant(time.toString("yyyy-MM-dd HH:mm:ss.zzz")))
        .paginate(1, range)
        .exec())
    {
        return _Kits::SqlSelect<radar_over_data>().getResults();
    }

    return {};
}