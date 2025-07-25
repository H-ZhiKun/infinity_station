#ifndef RADARRECORD_H
#define RADARRECORD_H

#include "kits/common/database/CppBatis.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/thrid_devices/radar_kit/base/DeviceData.hpp"
#include <QDateTime>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <memory>
#include <string>
#include <vector>

#ifndef Q_MOC_RUN
#ifdef emit
#undef emit
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#define emit
#else
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#endif // emit
#endif // Q_MOC_RUN

namespace _Modules
{
    class RadarRecord
    {
      public:
        enum class RecordType
        {
            NORMAL,
            OVERRUN
        };

      public:
        RadarRecord(size_t maxMemorySize = 250);
        ~RadarRecord();
        void OnRecordRadar(
            const std::shared_ptr<_Kits::DeviceSurfaceData> data, QString create_time, QString location_id, int task_id, int nCount);
        void OnRecordOverrun(std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>> &overrunList,
                             QString create_time,
                             QString update_time,
                             QString location_id_start,
                             QString location_id_end,
                             int overrun_time,
                             QString direction,
                             int task_id,
                             int pointCount);
        void SetMaxMemorySize(size_t maxMemorySize)
        {
            m_maxMemorySize = maxMemorySize;
        }

        _Kits::_Orm::location_data ReadyToWrite();

      private:
        // 双缓冲结构体
        struct DoubleBuffer
        {
            tbb::concurrent_queue<_Kits::_Orm::radar_data> normal_active;    // 无锁队列
            tbb::concurrent_queue<_Kits::_Orm::radar_over_data> over_active; // 无锁队列

            std::vector<_Kits::_Orm::radar_data> normal_ready;    // 准备写入的缓冲区
            std::vector<_Kits::_Orm::radar_over_data> over_ready; // 准备写入的缓冲区
            std::mutex swap_mutex;                                // 只用于缓冲区交换
        };

        DoubleBuffer m_buffer;

        void WriteBatchToVector(Json::Value &jsObj,
                                RecordType type,
                                QString create_time,
                                QString update_time,
                                QString location_id_end,
                                QString location_id_start,
                                int overrun_time,
                                QString direction,
                                int task_id,
                                int pointCount);
        void CheckMemorySizeAndWriteToDatabase();

        void WriteDataToDatabase();

        Json::Value CreateJsonObjectForOver(const std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>> &vec_data);
        std::shared_ptr<Json::Value> CreateJsonObjectForNormal(const std::vector<TIS_Info::DeviceSingleData> &data);

        std::shared_ptr<_Kits::ObjectPool<Json::Value>> m_jsonPool; // Json对象池

        std::vector<_Kits::_Orm::radar_data> GetCurrentId();

        std::vector<_Kits::_Orm::radar_over_data> m_over_dataVector; // 写入数据库的容器
        std::vector<_Kits::_Orm::radar_data> m_normal_dataVector;    // 写入数据库的容器
        std::atomic<size_t> m_maxMemorySize;                         // 最大sql条数，单位为条
        std::atomic<size_t> m_currentMemorySize{0};                  // 当前sql条数，单位为条

        std::mutex m_mutex_for_data;
    };
} // namespace _Modules

#endif // RADARRECORD_H