#include "OfflineRadarService.h"

using namespace _Kits;
using namespace _Modules;
using namespace _Kits::_Orm;
OfflineRadarService::OfflineRadarService()
    : m_overStatistic_(std::make_unique<OfflineDateStatistic>()),
      m_sqlOp_(std::make_unique<OfflineSqlOp>())
{
}

OfflineRadarService::~OfflineRadarService() noexcept
{
}

bool OfflineRadarService::init(const YAML::Node &config)
{

    // 与controller交互的信号槽函数
    /*
    流程:
        1.前端通知，查询任务表
        2.等待前端选择任务，返回任务id
        3.选择任务后进行分析
            3.1.查询普通点云数据表中的任务信息
            3.2.查询本次所有超限信息
            3.3.根据前端返回的时间戳查询前后时间戳的点云数据并缓存
            3.4.查询线路信息表 line_data


        前后端交互没做,先拟两个slot函数,一个接收任务id，一个接收启动指令
        //点云json数据解析没做    用于发送点云给qml，具体看在线里面的实现

        service功能没怎么做,就是流程逻辑
    */

    

    return true;
}
bool OfflineRadarService::start()
{
    //查询任务信息表
    if(!SearchTaskList())
    {
        return false;
    }
    return true;
}
bool OfflineRadarService::stop()
{
   
    return true;
}

bool OfflineRadarService::StartSearch(int id)
{
    // 先查询一次任务表，缓存到内存中
    // SearchTaskList();
    // 获取时间戳
    if (m_sqlOp_ == nullptr)
    {
        qDebug() << "Database operation object (m_sqlOp_) is not initialized.";
        return false;
    }

    auto m_taskTimeRange_ = m_sqlOp_->GetTimeRange(id);
    // 查询一次雷达数据，缓存至内存中
    QDateTime startTime = QDateTime::fromString(m_taskTimeRange_.first, "yyyy-MM-dd HH:mm:ss.zzz");
    QDateTime endTime = QDateTime::fromString(m_taskTimeRange_.second, "yyyy-MM-dd HH:mm:ss.zzz");

    auto radar_data = m_sqlOp_->GetDataByTime(startTime, 50);
    if (radar_data.empty())
    {
        qDebug() << "No radar data found for task ID:" << id;
        return false;
    }

    for (auto &data : radar_data)
    {
        // 雷达数据加入缓存
        m_radarData_.emplace_back(std::make_unique<_Kits::_Orm::radar_data>(data));
    }
    // 查询超限数据存到缓存  m_overData_
    auto over_data = m_sqlOp_->GetOverrunDataByTask(id);
    for (auto &data : over_data)
    {
        // 雷达超限数据 加入缓存
        m_overData_.emplace_back(std::make_unique<_Kits::_Orm::radar_over_data>(data));
    }

    return !m_radarData_.empty() && !m_overData_.empty();

    
}

bool OfflineRadarService::StopSearch()
{
    if (m_radarData_.size() > 0)
    {
        m_radarData_.clear();
    }
    if (m_overData_.size() > 0)
    {
        m_overData_.clear();
    }
    if (m_taskData_.size() > 0)
    {
        m_taskData_.clear();
    }

    return true;
}

bool OfflineRadarService::SearchCondition()
{

    return true;
}

bool OfflineRadarService::SearchTaskList()
{
    //所有的任务表查询到此
    m_taskData_ = std::move(m_sqlOp_->GetTaskTable());
    if (m_taskData_.empty())
    {
        qDebug() << "No task data found.";
        return false;
    }
    emit taskTableSendQml(QVariant::fromValue(_Kits::toJson(m_taskData_)));

    return true;
}

void OfflineRadarService::OnreceiveTimeLine(const QVariant &data)
{
    // 接受qml发送的时间戳,查询数据库,将数据库数据发送qml 包含战区station_name，杆号polename，公里标，速度，里程，线段类型，是否有倾线， 侵限点数量，

    if (data.canConvert<long long>())
    {
        QDateTime time = QDateTime::fromMSecsSinceEpoch(data.toLongLong());
        auto dataLocation = m_sqlOp_->GetLocationDataByTime(time, 10);
        emit localtionDataSendQml(QVariant::fromValue(_Kits::toJson(dataLocation)));

        auto dataOverRun = m_sqlOp_->GetOverrunDataByTime(time, 10);
        if (m_overStatistic_ != nullptr && m_overStatistic_->overInfoStatistic(dataOverRun))
        {
            auto overRunData = m_overStatistic_->getResults();
            // 将处理过的超限数据转成json 发送qml
            emit overInfoSendQml(QVariant::fromValue(_Kits::toJson(overRunData)));
        }
    }
}
void OfflineRadarService::OnreceiveTaskStart(const QVariant &data)
{
    // 接受qml发送的任务选择指令,查询数据库,将缓存中数据处理发送qml
    if (!data.canConvert<int>())
    {
        qDebug() << "Invalid data type received. Expected int.";
        return;
    }

    if (!m_sqlOp_)
    {
        qDebug() << "Database operation object (m_sqlOp_) is not initialized.";
        return;
    }

    if (data.canConvert<int>())
    {

        int taskId = data.toInt();
        if (StartSearch(taskId))
        {
            // 定位数据
            auto locationData = m_sqlOp_->GetLocationDataById(taskId);
            if (locationData.empty())
            {
                qDebug() << "No location data found for task ID:" << taskId;
                return;
            }
            emit localtionDataSendQml(QVariant::fromValue(_Kits::toJson(locationData)));
            emit radarDataSendQml(QVariant::fromValue(_Kits::toJson(m_radarData_)));
            StopSearch();
        }
    }
}

Json::Value ConvertToJson(const std::vector<_Kits::_Orm::task_data> &task_data_)
{
    Json::Value jsonArray = Json::arrayValue; // 初始化一个空的 JSON 数组
    for (const auto &data : task_data_)
    {
        Json::Value jsonObj;
        jsonObj["id"] = data.id;
        jsonObj["task_name"] = data.task_name.toStdString();
        jsonObj["line_name"] = data.line_name.toStdString();
        jsonObj["line_dir"] = data.line_dir;
        jsonObj["direction"] = data.direction;
        jsonObj["start_station"] = data.start_station.toStdString();
        jsonObj["end_station"] = data.end_station.toStdString();
        jsonObj["start_pole"] = data.start_pole.toStdString();
        jsonObj["created_time"] = data.created_time.toStdString();
        jsonObj["updated_time"] = data.updated_time.toStdString();

        jsonArray.append(jsonObj); // 将 location_data 对象添加到 JSON 数组
    }
    return jsonArray;
}
Json::Value OfflineRadarService::ConvertToJson(const std::vector<OfflineDateStatistic::OverInfoOffline> &results)
{
    Json::Value jsonArray = Json::arrayValue; // 初始化一个空的 JSON 数组
    for (const auto &data : results)
    {
        Json::Value jsonObj;
        jsonObj["id"] = data.mi_id;
        jsonObj["station_name"] = data.mstr_station.toStdString();
        jsonObj["pole_name"] = data.mstr_pole_name.toStdString();
        jsonObj["kilometer"] = data.mf_kilometer;
        jsonObj["line_type"] = data.mstr_linetype.toStdString();
        jsonObj["point_count"] = data.mi_point_count;
        jsonObj["overrun_time"] = data.mi_overrun_time;
        jsonObj["direction"] = data.mstr_direction.toStdString();
        jsonObj["created_time"] = data.mdt_create_time.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();

        jsonArray.append(jsonObj); // 将 location_data 对象添加到 JSON 数组
    }
    return jsonArray;
}

Json::Value OfflineRadarService::ConvertToJson(const std::vector<_Kits::_Orm::location_data> &results)
{
    Json::Value jsonArray = Json::arrayValue; // 初始化一个空的 JSON 数组
    for (const auto &data : results)
    {
        Json::Value jsonObj;
        jsonObj["id"] = data.id;
        jsonObj["station_name"] = data.station_name.toStdString();
        jsonObj["maoduan_name"] = data.maoduan_name.toStdString();
        jsonObj["pole_name"] = data.pole_name.toStdString();
        jsonObj["train_move_dis"] = data.kilo_meter;
        jsonObj["speed"] = data.speed;
        jsonObj["created_time"] = data.created_time.toStdString();
        jsonObj["updated_time"] = data.updated_time.toStdString();

        jsonArray.append(jsonObj); // 将 location_data 对象添加到 JSON 数组
    }
    return jsonArray;
}
Json::Value OfflineRadarService::ConvertToJson(const std::deque<std::unique_ptr<_Kits::_Orm::radar_data>> &results)
{

    Json::Value jsonArray = Json::arrayValue; // 初始化一个空的 JSON 数组

    for (const auto &dataPtr : results)
    {
        if (dataPtr)
        {
            Json::Value jsonObj;
            jsonObj["id"] = dataPtr->id;
            jsonObj["location_id"] = dataPtr->location_id;
            jsonObj["task_id"] = dataPtr->task_id;

            // 将 "points" 字段转换为 JSON 数组
            jsonObj["points"] = ParsePointsToJsonArray(dataPtr->points);

            jsonObj["created_time"] = dataPtr->created_time.toStdString();
            jsonObj["updated_time"] = dataPtr->updated_time.toStdString();

            jsonArray.append(jsonObj); // 将 radar_data 对象添加到 JSON 数组
        }
    }

    return jsonArray;
}

Json::Value OfflineRadarService::ParsePointsToJsonArray(const QString &pointsStr)
{

    Json::Value jsonArray = Json::arrayValue; // 初始化一个空的 JSON 数组

    // 使用空格分隔每个坐标对 "x,y"
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(pointsStr.toUtf8(), &parseError);

    // 确保 JSON 是一个数组
    if (!jsonDoc.isArray())
    {
        qWarning() << "Points JSONB is not an array.";
        return jsonArray; // 返回空数组
    }

    // 遍历 QJsonArray 并转换为 Json::Value
    QJsonArray jsonArrayQt = jsonDoc.array();
    for (const QJsonValue &value : jsonArrayQt)
    {
        if (value.isObject())
        {
            QJsonObject obj = value.toObject();
            Json::Value jsonObj;
            jsonObj["x"] = obj.value("x").toDouble();
            jsonObj["y"] = obj.value("y").toDouble();
            jsonArray.append(jsonObj);
        }
    }

    return jsonArray;
}
bool OfflineRadarService::readRadarCsv(const QString &filePath, std::vector<_Kits::_Orm::radar_over_data> &results)
{

    return true;
}

QStringList OfflineRadarService::ParseCsvLine(const QString &line)
{
    QStringList fields;
    QString current;
    bool inQuotes = false;

    for (QChar ch : line)
    {
        if (ch == '"')
        {
            inQuotes = !inQuotes;
        }
        else if (ch == ',' && !inQuotes)
        {
            fields.append(current.trimmed());
            current.clear();
        }
        else
        {
            current.append(ch);
        }
    }
    fields.append(current.trimmed());
    return fields;
}
bool OfflineRadarService::StartSearch(const QString &taskIdPath)
{

    return false;
}

void OfflineRadarService::OnreceiveTaskCSV(const QVariant &data)
{
    // 接收qml QStinrg 路径  返回json数据
    if (data.canConvert<QString>())
    {
        std::vector<_Kits::_Orm::radar_over_data> m_overData_;
        if (readRadarCsv(data.toString(), m_overData_))
        {
            // 解析超限数据
            if (m_overStatistic_->overInfoStatistic(m_overData_))
            {
                // 发送解析结果给 以json格式发送QML
                emit overInfoSendQml(QVariant::fromValue(m_overStatistic_->getResults()));
            }
            else
            {
                qWarning() << "Failed to parse overrun data.";
            }
        }
        else
        {
            qWarning() << "Failed to read CSV file:" << data.toString();
        }
    }
}
