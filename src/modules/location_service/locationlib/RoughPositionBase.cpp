#include "RoughPositionBase.h"
#include "kits/common/database/CppBatis.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;
using namespace _Kits::_Orm;
RoughPositionBase::RoughPositionBase() : PositionBaseInterface()
{
}

RoughPositionBase::~RoughPositionBase()
{
}

int RoughPositionBase::init(const YAML::Node &config)
{
    // 初始化资源
    // m_refreshRate = config["refresh_rate"].as<int>();

    return 0;
}

int RoughPositionBase::start()
{
    return 0;
}

int RoughPositionBase::stop()
{
    return 0;
}

std::vector<line_data> select_kilometer_less(double dis,int dir)
{
    auto selector = _Kits::SqlSelect<line_data>();
    selector.select({"station_name", "kilo_meter", "pole_name", "maoduan_name", "id" ,"span"})
        .where("kilo_meter", OperatorComparison::LessThan, dis)
        .where("line_dir", OperatorComparison::Equal, dir)
        .orderBy("kilo_meter", false)
        .paginate(1, 1)
        .exec();

    return selector.getResults();
}

std::vector<line_data> select_kilometer_great(double dis,int dir)
{
    auto selector = _Kits::SqlSelect<line_data>();

    selector.select({"station_name", "kilo_meter", "pole_name", "maoduan_name", "id","span"})
        .where("kilo_meter", OperatorComparison::GreaterThan, dis)
        .where("line_dir", OperatorComparison::Equal, dir)
        .orderBy("kilo_meter", false)
        .paginate(1, 1)
        .exec();

    // 处理查询结果
    return selector.getResults();
}

bool RoughPositionBase::selectLineData(int stationId, TIS_Info::PositionData &posInfo)//todo 战区ID还没有
{
    auto selector = _Kits::SqlSelect<line_data>();
    selector.select({"station_name", "kilo_meter", "pole_name", "maoduan_name", "id"})
        .where("id", OperatorComparison::Equal, stationId)
        .where("line_dir", OperatorComparison::Equal, m_taskInfo.nLineDir)
        .paginate(1, 4)
        .exec();
    auto results = selector.getResults();
    if (results.size() == 1)
    {
        posInfo.stationName = results[0].station_name;
        posInfo.maoduanName = results[0].maoduan_name;
        posInfo.poleName = results[0].pole_name;
        posInfo.lineId = results[0].id;
        posInfo.kiloMeter = results[0].kilo_meter;
        return true;
    }

    return false;
}
bool RoughPositionBase::selectLineData(double dis, TIS_Info::PositionData &posInfo)
{

    dis = dis + 0.001; //todo 数据库有问题,数据库会查到前一个杆

    auto lessData = select_kilometer_less(dis,m_taskInfo.nLineDir);
    auto greatData =select_kilometer_great(dis,m_taskInfo.nLineDir);
    bool useGreat = false;
    if (greatData.size() == 1 && lessData.size() == 0) // 第一个
    {
        useGreat = true;
    }
    else if (greatData.size() == 1 && lessData.size() == 1) // 在中间
    {
        auto subData = std::abs(dis - lessData[0].kilo_meter);
        auto subData2 = std::abs(greatData[0].kilo_meter - dis);
        if (subData < subData2)
        {
            useGreat = false;
        }
        else
        {
            useGreat = true;
        }
    }
    else
    {
        return false;
    }

    if (useGreat)
    {
        posInfo.stationName = greatData[0].station_name;
        posInfo.maoduanName = greatData[0].maoduan_name;
        posInfo.poleName = greatData[0].pole_name;
        posInfo.lineId = greatData[0].id;
        posInfo.kiloMeter = dis;
        posInfo.span=greatData[0].span;
    }
    else
    {
        posInfo.stationName = lessData[0].station_name;
        posInfo.maoduanName = lessData[0].maoduan_name;
        posInfo.poleName = lessData[0].pole_name;
        posInfo.lineId = lessData[0].id;
        posInfo.kiloMeter = dis;
        posInfo.span=lessData[0].span;
    }
    return true;
}

bool RoughPositionBase::isSpan(const QString &poleName)
{
    if (m_curPositionInfo.poleName != poleName)
    {
        m_curPositionInfo.poleName = poleName;
        return true;
    }
    else
    {
        return false;
    }
} 

TIS_Info::PositionData RoughPositionBase::getPositionData(double dis, double speed)
{
    TIS_Info::PositionData posInfo;
    if (m_taskInfo.isRun())
    {

        double disBase = m_taskInfo.dKilometer;
        bool ret = selectLineData(dis + disBase, posInfo);
        if (ret)
        {
            if (isSpan(posInfo.poleName))
            {
                posInfo.positionType = TIS_Info::LocationCommunication::PositionType::PT_DIST;
            }
            else
            {
                posInfo.positionType = TIS_Info::LocationCommunication::PositionType::PT_LINE;
            }
            m_curPositionInfo = posInfo;
            posInfo.speed = speed;
            posInfo.taskId = m_taskInfo.nTaskID;
        }
    }
    return posInfo;
}

TIS_Info::PositionData RoughPositionBase::getPositionData(int stationId)
{
    TIS_Info::PositionData posInfo;
    bool ret = selectLineData(stationId, posInfo);
    if (ret)
    {
        return posInfo;
    }
    return posInfo;
}
void RoughPositionBase::setTaskInfo(TIS_Info::TaskInfo taskInfo)
{
    m_taskInfo = taskInfo;
}

bool RoughPositionBase::isEffectPT(int *data)
{
    *data = m_curPositionInfo.positionType;
    return m_curPositionInfo.positionType >= 2;
}