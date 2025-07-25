#include "PreviousTaskCtrl.h"
#include "kits/common/database/CppBatis.h"
#include "kits/common/system_config/SystemConfig.h"
#include <QByteArray>
#include <QDateTime>
#include <QTextStream>
#include <filesystem>
#include <qfile.h>
#include <QDir>
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;
using namespace _Kits::_Orm;
using namespace _Modules;

PreviousTaskCtrl::PreviousTaskCtrl(/* args */)
{
}

PreviousTaskCtrl::~PreviousTaskCtrl()
{
}

void PreviousTaskCtrl::initConfigInfo(const YAML::Node &config)
{
    auto configpath = config["config_path"].as<std::string>();
    // 加载配置文件
    auto &sysConfig = _Kits::SystemConfig::instance();
    auto loadFilePath = sysConfig.configVersionPath() + configpath;
    YAML::Node config_node = _Kits::SystemConfig::instance().loadYamlFile(loadFilePath);

    m_SavePath = config_node["savepath"].as<std::string>("d:/gwkj/");
    auto stationFilePath = config_node["stationfilepath"].as<std::string>();
    auto lineDataFilePath = config_node["linedatafilepath"].as<std::string>();
    auto onlineTaskFilePaht = config_node["onlinetaskfilepath"].as<std::string>();
    if (0 != readStationInfo(stationFilePath))
    {
        return;
    }
    if (0 != readLineDataInfo(lineDataFilePath))
    {
        return;
    }
    if (0 != readOnlineTask(onlineTaskFilePaht))
    {
        return;
    }
}
int PreviousTaskCtrl::readStationInfo(const std::string &filePath)
{
    ReadCSVFile readlinecsv;
    QList<QStringList> stationdatalist;
    if (0 != readlinecsv.readFile(filePath, stationdatalist))
    {
        return -1;
    }

    for (auto &it : stationdatalist)
    {
        StationInfo struStation;
        struStation.nStationID = it.at(0).toInt();                  //  站id
        struStation.strStationName = it.at(1).toStdString();        //  站名
        struStation.strEndStationName_UP = it.at(2).toStdString();  //  上行终点站
        struStation.strEndStationName_DN = it.at(3).toStdString();  //  下行终点站
        struStation.strPole_UP = it.at(4).toStdString();            //  上行杆号
        struStation.strPole_DN = it.at(5).toStdString();            //  下行杆号
        struStation.strNextStationName_UP = it.at(6).toStdString(); //  上行下一站
        struStation.strNextStationName_DN = it.at(7).toStdString(); //  下行下一站
        struStation.strPreStationName_UP = it.at(8).toStdString();  //  上行上一站
        struStation.strPreStationName_DN = it.at(9).toStdString();  //  下行上一站

        m_stationInfoList.insert(std::make_pair(struStation.nStationID,
                                                struStation)); //   存入map集合
    }

    return 1;
}
int PreviousTaskCtrl::readLineDataInfo(const std::string &filePath)
{
    ReadCSVFile readlinecsv;
    QList<QStringList> stationdatalist;
    std::filesystem::path dirPath = filePath;
    auto filename = dirPath.filename();
    if (0 != readlinecsv.readFile(filePath, stationdatalist))
    {
        return -1;
    }
    std::list<LineDataBaseInfo> lineDataList;
    for (auto &it : stationdatalist)
    {
        LineDataBaseInfo linedatabase;
        linedatabase.strLineName = it.at(0).toStdString();
        linedatabase.strSationName = it.at(1).toStdString();
        linedatabase.strTunnelName = it.at(2).toStdString();
        linedatabase.strMaoduanName = it.at(3).toStdString();
        linedatabase.strPoleName = it.at(4).toStdString();
        linedatabase.fPoleDistance = it.at(5).toFloat();

        linedatabase.nStructure = 10;
        auto linesignit = lineSignList.find(it.at(6).toStdString());
        if (linesignit != lineSignList.end())
        {
            linedatabase.nStructure = linesignit->second;
        }

        linedatabase.fTrainKilometer = it.at(7).toFloat();
        linedatabase.fZig = it.at(8).toFloat();
        linedatabase.fHei = it.at(9).toFloat();

        auto strlineDir = it.at(10).toStdString();
        if ("上行" == strlineDir)
        {
            linedatabase.nLineDir = 0;
        }
        else
        {
            linedatabase.nLineDir = 1;
        }

        linedatabase.nLineType = -1;
        auto lineTypeit = lineTypeList.find(it.at(11).toStdString());
        if (lineTypeit != lineTypeList.end())
        {
            linedatabase.nLineType = lineTypeit->second;
        }

        linedatabase.fGpsLongitude = it.at(12).toFloat();
        linedatabase.fGpsLatitude = it.at(13).toFloat();
        lineDataList.emplace_back(linedatabase);
        // linedatabase.nStationOnlineId = 0;
        // linedatabase.iNextStationOnlineId = 0;
        // linedatabase.bOnlineStopPole = false;
    }
    m_lineDataList.insert(std::make_pair(filename.string(), lineDataList));
    return 1;
}
int PreviousTaskCtrl::readOnlineTask(const std::string &filePath)
{
    YAML::Node config = YAML::LoadFile(std::filesystem::current_path().string() + filePath);
    int i = 0;
    for (auto portinfo : config["OnlineTask"])
    {
        OnlineTask onlinetask;
        onlinetask.strSubWayNum = portinfo["OnlineSubwayNum"].as<std::string>();
        onlinetask.strLineName = portinfo["OnlineLineName"].as<std::string>();
        onlinetask.strDirection = portinfo["OnlineDirection"].as<std::string>();
        onlinetask.strStartStation = portinfo["OnlineStartStation"].as<std::string>();
        onlinetask.strEndStation = portinfo["OnlineEndStation"].as<std::string>();
        onlinetask.strStartPole = portinfo["OnlineStartPole"].as<std::string>();
        onlinetask.bOnlineTask = portinfo["IsOnlineTask"].as<bool>();
        onlinetask.nLineDir = portinfo["linedir"].as<int>();
        m_arrOnlineTask[i] = onlinetask;
        i++;
    }
    return 0;
}
int PreviousTaskCtrl::createTask(const QVariant &data, TIS_Info::TaskInfo &taskData)
{
    auto mapdata = data.toMap();
    auto tasktype = mapdata.value("taskType", 0).toInt();
    auto taskstate = mapdata.value("taskState", 1).toInt();
    switch (taskstate)
    {
    case 0: {
        if (1 == tasktype) // 创建在线任务
        {
            return createOnlineTask(data, taskData);
        }
        // 手动创建任务
        return createTestTaskData(data, taskData);
    }
    break;
    case 1: {
        // taskvariant["taskstate"] = 0;
        stopTask();
        taskData = m_currentTask;
        return 0;
    }
    default:
        break;
    }

    return 0;
}

int PreviousTaskCtrl::createOnlineTask(const QVariant &data, TIS_Info::TaskInfo &taskData)
{
    if (m_bStartTaskFlag.load())
    {
        return -1;
    }
    auto mapdata = data.toMap();
    m_currentTask.nLineDir = mapdata.value("lineDirection", -1).toString() == "下行" ? 0 : 1;
    m_currentTask.strLineName = mapdata.value("lineName", "").toString();
    m_currentTask.strStartStation = mapdata.value("startStation", "").toString();
    m_currentTask.strStartPole = mapdata.value("startPole", "").toString();
    m_currentTask.strEndStation = mapdata.value("endStation", "").toString();
    m_currentTask.strSubWayNum = mapdata.value("trainNumber", "").toString();
    m_currentTask.nDirection = mapdata.value("runDirection", "").toInt();
    m_currentTask.dKilometer = mapdata.value("kilometer", "").toDouble();
    m_currentTask.strCreateTime = m_taskData.created_time;
    m_currentTask.nTaskstate = 0;
    m_bStartTaskFlag.store(true);

    if (0 != startTask())
    {
        return -2;
    }

    taskData = m_currentTask;

    return 0;
}
int PreviousTaskCtrl::createTestTaskData(const QVariant &data, TIS_Info::TaskInfo &taskData)
{

    auto mapdata = data.toMap();
    m_currentTask.nLineDir = mapdata.value("lineDirection", -1).toString() == "上行" ? 0 : 1;
    m_currentTask.strLineName = mapdata.value("lineName", "").toString();
    m_currentTask.strStartStation = mapdata.value("startStation", "").toString();
    m_currentTask.strStartPole = mapdata.value("startPole", "").toString();
    m_currentTask.strEndStation = mapdata.value("endStation", "").toString();
    m_currentTask.strSubWayNum = mapdata.value("trainNumber", "").toString();
    m_currentTask.nDirection = mapdata.value("runDirection", "").toInt();
    m_currentTask.dKilometer = mapdata.value("kilometer", "").toDouble();
    m_currentTask.nTaskstate = 0;
    m_currentTask.strCreateTime = m_taskData.created_time;
    m_bStartTaskFlag.store(true);

    if (0 != startTask())
    {
        return -1;
    }

    taskData = m_currentTask;

    return 0;
}

int PreviousTaskCtrl::startTask()
{
    QDateTime datatime = QDateTime::currentDateTime();
    QString strCreateTime = datatime.toString("yyyyMMddhhmmss");
    m_currentTask.strCreateTime = strCreateTime;
    m_currentTask.strTaskName = m_currentTask.strSubWayNum + "_" + m_currentTask.strCreateTime + "_" + m_currentTask.strLineName + "_" +
                                m_currentTask.strStartStation + "_" + m_currentTask.strEndStation;

    m_taskData.task_name = m_currentTask.strTaskName;
    m_taskData.line_name = m_currentTask.strLineName;
    m_taskData.direction = m_currentTask.nDirection;
    m_taskData.start_station = m_currentTask.strStartStation;
    m_taskData.start_pole = m_currentTask.strStartPole;
    m_taskData.end_station = m_currentTask.strEndStation;
    m_taskData.created_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    m_taskData.updated_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    m_taskData.line_dir = m_currentTask.nLineDir;

    createTaskDataBase();
    QString folderPath = QString::fromStdString(m_SavePath) + "/" + datatime.toString("yyyyMMdd") + "/" + m_currentTask.strTaskName;
    // std::filesystem::path dirPath = folderPath;

    QDir dir(folderPath);
    if (!dir.exists())
    {
        if (dir.mkpath("."))
        { // mkpath会递归创建所有不存在的目录
            qDebug() << "文件夹创建成功:" << folderPath;
        }
        else
        {
            qDebug() << "文件夹创建失败! " << folderPath;
            return -1;
        }
    }

    m_currentTask.strTaskSavePath = folderPath;
    m_currentTask.nTaskID = _Modules::sqlio::searchIdByTaskName(m_currentTask.strTaskName);

    return 0;
}
void PreviousTaskCtrl::stopTask()
{

    if (!_Modules::sqlio::updataTaskTime(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"), m_currentTask.nTaskID))
    {
        LogError("更新任务结束时间失败。");
    }
    m_currentTask.clear();
    m_bStartTaskFlag.store(false);
}
int PreviousTaskCtrl::insertLineData()
{

    if (_Kits::SqlInsert<_Kits::_Orm::task_data>().insert(m_taskData).exec() == true)
    {
        return 0;
    }
    else
    {
        qDebug() << "数据库插入失败。";
        _Kits::LogError("数据库插入失败。");
        return -1;
    }
}

int PreviousTaskCtrl::createTaskDataBase()
{
    insertLineData();

    return 0;
}
