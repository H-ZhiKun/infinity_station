#pragma once

#include "kits/common/database/CppBatis.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/radar_kit/base/DeviceData.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <json/json.h>
#include <QDateTime>

#include <any>

namespace _Modules
{

    class OfflineSqlOp
    {

      public:
        OfflineSqlOp() {};

        ~OfflineSqlOp() {};

        // 根据时间戳获取点云数据
        std::vector<_Kits::_Orm::radar_data> GetDataByTime(QDateTime &time, int range);
        // 根据时间戳范围获取雷达点云数据
        std::vector<_Kits::_Orm::radar_data> GetDataByTimeRange(QDateTime &start, QDateTime &end, int range);
        // 获取前后时间戳
        std::pair<QString, QString> GetTimeRange(int taskId);
        // 获取超限所有信息
        std::vector<_Kits::_Orm::radar_over_data> GetOverrunDataByTask(int taskId);
        // 根据id获取定位信息
        std::vector<_Kits::_Orm::location_data> GetLocationDataById(int locationId);
        // 获取任务表
        std::vector<_Kits::_Orm::task_data> GetTaskTable();
        // 根据时间戳获得定位信息
        std::vector<_Kits::_Orm::location_data> GetLocationDataByTime(QDateTime &time, int range);
        // 根据时间戳获得超限信息
        std::vector<_Kits::_Orm::radar_over_data> GetOverrunDataByTime(QDateTime &time, int range);
    };

} // namespace _Modules