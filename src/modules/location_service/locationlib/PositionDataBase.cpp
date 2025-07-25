#include "PositionDataBase.h"
#include "kits/common/database/CppBatis.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/common/log/CRossLogger.h"

#include <QTime>
using namespace _Kits::_Orm;
using namespace _Kits;
bool PositionDataBase::DataBaseInsert(QVariant data)
{

    std::vector<location_data> lvObj;
    lvObj.reserve(1);
    location_data data1;
    data1.station_name = data.toMap().value("stationName", QString()).toString();
    data1.pole_name = data.toMap().value("poleName", QString()).toString();
    data1.maoduan_name = data.toMap().value("maoDuanName", QString()).toString();
    data1.kilo_meter = data.toMap().value("kiloMeter", -1.0f).toDouble();
    data1.speed = data.toMap().value("speed", -1.0f).toDouble();
    data1.location_type = data.toMap().value("positionType", 0).toInt();
    data1.task_id = data.toMap().value("taskId", -1).toInt();
    data1.move_dis = data.toMap().value("moveDis", -1.0f).toDouble();
    data1.line_id = data.toMap().value("lineId", -1).toInt();
    lvObj.emplace_back(std::move(data1));

    _Kits::SqlInsert<location_data> insert;
    insert.insert(lvObj).exec();
    int nums = insert.getNumAffected();
    return false;
}

bool PositionDataBase::DataBaseInsert(const TIS_Info::PositionData &vecData)
{
    auto dbData = convertToDBData(vecData);
    return DataBaseInsert(dbData);
}

_Kits::_Orm::location_data PositionDataBase::convertToDBData(const TIS_Info::PositionData &data)
{
    _Kits::_Orm::location_data dbData;
    QString createtime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    dbData.task_id = data.taskId;
    dbData.station_name = data.stationName;
    dbData.pole_name = data.poleName;
    dbData.maoduan_name = data.maoduanName;
    dbData.line_id = data.lineId;
    dbData.location_type = data.positionType;
    dbData.kilo_meter = data.kiloMeter;
    dbData.move_dis = data.moveDistance;
    dbData.speed = data.speed;
    dbData.created_time = createtime;
    return dbData;
}
bool PositionDataBase::DataBaseInsert(const _Kits::_Orm::location_data &vecData)
{

    if (m_InsertData.size() > 50) // 后续改为动态调整todo
    {
        m_InsertData.push_back(vecData);
        DataBaseInsert(m_InsertData);
        m_InsertData.clear();
    }
    else
    {
        m_InsertData.push_back(vecData);
    }
    return true;
}

bool PositionDataBase::DataBaseInsert(const std::vector<_Kits::_Orm::location_data> &vecData)
{
    if (!vecData.empty())
    {
        _Kits::SqlInsert<_Kits::_Orm::location_data> insert;

        insert.insert(vecData).exec();

        int nums = insert.getNumAffected();
        // _Kits::LogDebug("insert location data end nums={}", nums);
        return nums > 0;
    }
    return false;
}

int PositionDataBase::DataBaseQuery(std::string stationName, std::string poleName, int direction, double &dis)
{
    auto selector = _Kits::SqlSelect<line_data>();
    selector.select({"station_name", "pole_name", "line_dir", "kilo_meter"})
        .where(QString("station_name"), OperatorComparison::Equal, QString(stationName.c_str()))
        .where(QString("pole_name"), OperatorComparison::Equal, QString(poleName.c_str()))
        .where("line_dir", OperatorComparison::Equal, direction)
        .paginate(1, 5)
        .exec();

    // 处理查询结果
    auto datas = selector.getResults();
    if (datas.size() != 1)
    {
        qDebug() << "line_data error";
        return -1;
    }
    for (const auto &data : datas)
    {
        dis = data.kilo_meter;
        return 0;
        // qDebug() << "position id:" << data.id;
    }
    return -1;
}

int PositionDataBase::DataBaseQuery(std::string poleName, int direction, double &dis)
{
    auto selector = _Kits::SqlSelect<line_data>();
    selector.select({"station_name", "pole_name", "line_dir", "kilo_meter"})
        .where(QString("pole_name"), OperatorComparison::Equal, QString(poleName.c_str()))
        .where("line_dir", OperatorComparison::Equal, direction)
        .paginate(1, 5)
        .exec();

    // 处理查询结果
    auto datas = selector.getResults();
    if (datas.size() != 1)
    {
        qDebug() << "line_data error";
        return -1;
    }
    for (const auto &data : datas)
    {
        dis = data.kilo_meter;
        return 0;
    }
    return -1;
}

int PositionDataBase::DataBaseQuery(QString rfidId, int direction, double &dis)
{
    auto selector = _Kits::SqlSelect<line_data>();
    selector.select({"tag_id", "kilo_meter"})
        .where(QString("tag_id"), OperatorComparison::Equal, rfidId)
        .where(QString("line_dir"), OperatorComparison::Equal, direction)
        .paginate(1, 5)
        .exec();

    // 处理查询结果
    auto datas = selector.getResults();
    if (datas.size() != 1)
    {
        qDebug() << "line_data error";
        return -1;
    }
    for (const auto &data : datas)
    {
        dis = data.kilo_meter;
        return 0;
    }
    return -1;
}

int PositionDataBase::DataBaseQueryMaoDuan(double curDis, int direction, double &dis)
{

    auto selector = _Kits::SqlSelect<line_data>();
    selector.select({"station_name", "kilo_meter", "pole_name", "maoduan_name", "structure"})
        .where("line_dir", OperatorComparison::Equal, direction)
        .where("structure", OperatorComparison::Equal, "锚段关节")
        .orderBy("kilo_meter", false)
        .exec();

    // 处理查询结果
    auto datas = selector.getResults();
    if (datas.size() == 0)
    {
        qDebug() << "line_data range error";
        return -1;
    }

    double minDistance = std::numeric_limits<double>::max();
    double closestKiloMeter = 0.0;

    for (const auto &data : datas)
    {
        double diff = std::fabs(data.kilo_meter - curDis);
        if (diff < minDistance)
        {
            closestKiloMeter = data.kilo_meter;
            minDistance = diff;
        }
    }

    dis = closestKiloMeter;

    return 0;
}
