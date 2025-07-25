// 1.获取需要的数据，定位等

// 2.存储数据到特定文件中，路径由接口传入

// 3.判断文件大小，如果够多就将数据一次性写入数据库并删除文件

#include "RadarRecord.h"
#include "kits/common/database/CppBatis.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <QDateTime>
#include <iostream>
#include <memory>
#include <string>
#include "kits/common/serialization/serialize_jsoncpp.h"

using namespace _Modules;
using namespace std;
using namespace _Kits;
using namespace _Kits::_Orm;
using namespace TIS_Info;

RadarRecord::RadarRecord(size_t maxMemorySize)
    : m_maxMemorySize(maxMemorySize), m_currentMemorySize(0), m_jsonPool(std::make_shared<_Kits::ObjectPool<Json::Value>>(1000))
{
}

RadarRecord::~RadarRecord()
{
    WriteDataToDatabase();
}

void RadarRecord::OnRecordRadar(
    std::shared_ptr<_Kits::DeviceSurfaceData> data, QString create_time, QString location_id, int task_id, int pointCount)
{

    try
    {
        auto jsVal = CreateJsonObjectForNormal(data->mvec_data_surface);

        WriteBatchToVector(*jsVal, RecordType::NORMAL, create_time, "", 0, location_id, 0, "", task_id, pointCount);

        CheckMemorySizeAndWriteToDatabase();
    }
    catch (const std::exception &e)
    {
        qDebug() << "Database error: " << e.what();
    }
    catch (...)
    {
        qDebug() << "Unknown error occurred while writing radar data.";
    }
}
void RadarRecord::WriteBatchToVector(Json::Value &jsObj,
                                     RecordType type,
                                     QString create_time,
                                     QString update_time,
                                     QString location_id_end,
                                     QString location_id_start,
                                     int overrun_time,
                                     QString direction,
                                     int task_id,
                                     int pointCount)
{
    radar_data sqlObj_normal;
    radar_over_data sqlObj_over;
    static int nCount = -1;
    switch (type)
    {
    case RecordType::NORMAL:
        sqlObj_normal.created_time = create_time;
        sqlObj_normal.location_time = location_id_start;
        sqlObj_normal.points = std::move(QString::fromStdString(jsonToString(jsObj)));

        if (++nCount == 0)
        {
            qDebug() << sqlObj_normal.points;
        }
        sqlObj_normal.task_id = task_id;

        m_buffer.normal_active.push(std::move(sqlObj_normal));
        m_currentMemorySize.fetch_add(1, std::memory_order_relaxed);
        break;

    case RecordType::OVERRUN:
        sqlObj_over.created_time = create_time;
        sqlObj_over.updated_time = update_time;
        sqlObj_over.location_time_start = location_id_start;
        sqlObj_over.location_time_end = location_id_end;
        sqlObj_over.points = QString::fromStdString(jsonToString(jsObj));
        sqlObj_over.direction = direction;
        sqlObj_over.overrun_time = overrun_time;
        sqlObj_over.task_id = task_id;
        sqlObj_over.point_count = nCount;

        m_buffer.over_active.push(std::move(sqlObj_over));
        m_currentMemorySize.fetch_add(1, std::memory_order_relaxed);
        break;
    }
}

void RadarRecord::CheckMemorySizeAndWriteToDatabase()
{
    if (m_currentMemorySize.load() < m_maxMemorySize)
        return;

    m_currentMemorySize.store(0);

    // 无锁批量转移数据
    std::vector<radar_data> normalBatch;
    radar_data normalItem;
    while (m_buffer.normal_active.try_pop(normalItem))
    {
        normalBatch.push_back(std::move(normalItem));
    }

    std::vector<radar_over_data> overBatch;
    radar_over_data overItem;
    while (m_buffer.over_active.try_pop(overItem))
    {
        overBatch.push_back(std::move(overItem));
    }

    // 短时锁交换缓冲区
    {
        std::lock_guard lock(m_buffer.swap_mutex);
        std::swap(m_buffer.normal_ready, normalBatch);
        std::swap(m_buffer.over_ready, overBatch);
    }

    // 异步写入数据库
    if (!m_buffer.normal_ready.empty() || !m_buffer.over_ready.empty())
    {
        WriteDataToDatabase();
    }
}

void RadarRecord::WriteDataToDatabase()
{
    if (!m_buffer.normal_ready.empty())
    {
        ConcurrentPool::runTask([this] {
            std::vector<radar_data> batch;
            {
                std::lock_guard lock(m_buffer.swap_mutex);
                batch = std::move(m_buffer.normal_ready);
            }
            _Kits::SqlInsert<radar_data>().insert(batch).exec();
        });
    }

    if (!m_buffer.over_ready.empty())
    {
        ConcurrentPool::runTask([this] {
            std::vector<radar_over_data> batch;
            {
                std::lock_guard lock(m_buffer.swap_mutex);
                batch = std::move(m_buffer.over_ready);
            }
            _Kits::SqlInsert<radar_over_data>().insert(batch).exec();
        });
    }
}

void RadarRecord::OnRecordOverrun(std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>> &overrunList,
                                  QString create_time,
                                  QString update_time,
                                  QString location_id_start,
                                  QString location_id_end,
                                  int overrun_time,
                                  QString direction,
                                  int task_id,
                                  int pointCount)
{

    static int nCount = -1;

    if (++nCount % 250 == 0)
    {
        LogDebug("RadarRecord::OnRecordOverrun() 开始写入数据 {}", nCount);
    }

    auto jsval = CreateJsonObjectForOver(overrunList);

    if (nCount % 250 == 0)
    {
        LogDebug("RadarRecord::OnRecordOverrun() 结束写入数据 {}", nCount);
    }

    WriteBatchToVector(jsval,
                       RecordType::OVERRUN,
                       create_time,
                       update_time,
                       location_id_start,
                       location_id_end,
                       overrun_time,
                       direction,
                       task_id,
                       pointCount);

    CheckMemorySizeAndWriteToDatabase();
}

Json::Value RadarRecord::CreateJsonObjectForOver(const std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>> &vec_data)
{
    Json::Value jsonObject; // 最终返回的JSON对象
    int index = 0;

    for (auto &dev_data : vec_data)
    {
        Json::Value pointArray;
        auto &pointsJson = pointArray["points"];
        pointArray["angle"] = dev_data->mus_angle_start;

        auto results = GetCurrentId();
        if (results.empty())
        {
            pointArray["id"] = 0;
        }
        else
        {
            pointArray["id"] = results.begin()->id;
        }

        for (auto &point : dev_data->mvec_data_surface)
        {
            Json::Value pointObject;
            pointObject["x"] = point.mf_x;
            pointObject["y"] = point.mf_y;
            pointsJson.append(pointObject);
        }

        // 将当前设备的JSON对象添加到最终的JSON对象中，使用索引作为键
        jsonObject[std::to_string(index)] = pointArray;
        ++index; // 索引递增
    }

    return jsonObject;
}

std::shared_ptr<Json::Value> RadarRecord::CreateJsonObjectForNormal(const std::vector<DeviceSingleData> &data)
{
    // 从对象池获取预分配的 JSON 对象
    auto jsonPtr = m_jsonPool->getObject();
    Json::Value &jsonObject = *jsonPtr;

    Json::Value &pointsArray = jsonObject["points"];

    // 预分配内存减少动态分配开销
    const size_t numPoints = data.size();
    pointsArray.resize(static_cast<int>(numPoints));

    // 使用 TBB 的 parallel_for 进行并行处理
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numPoints), [&](const tbb::blocked_range<size_t> &r) {
        for (size_t i = r.begin(); i < r.end(); ++i)
        {
            // 直接操作预分配的位置
            Json::Value &pointObject = pointsArray[static_cast<int>(i)];
            pointObject["x"] = data[i].mf_x;
            pointObject["y"] = data[i].mf_y;
        }
    });

    return jsonPtr;
}

_Kits::_Orm::location_data RadarRecord::ReadyToWrite()
{
    auto sql = _Kits::SqlSelect<location_data>().select(QStringList("*")).orderBy("id", false).paginate(1, 1);

    if (sql.exec())
    {
        auto res = sql.getResults();
        if (res.size() > 0)
        {
            return res[0];
        }
        else
        {
            return location_data();
        }
    }
    else
    {
        return location_data();
    }
}

std::vector<radar_data> RadarRecord::GetCurrentId()
{
    auto sql = _Kits::SqlSelect<radar_data>().select(QStringList("id")).orderBy("id", false).paginate(1, 1);

    if (sql.exec())
    {
        return sql.getResults();
    }

    return std::vector<radar_data>();
}
