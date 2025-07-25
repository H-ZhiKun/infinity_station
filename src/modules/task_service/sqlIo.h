#pragma once

#include <vector>
#include <string>
#include "kits/common/database/CppBatis.h"
#include "json/json.h"

namespace _Modules
{

    class sqlio
    {
      public:
        sqlio();
        ~sqlio();

        /// @brief 找到线路名称并输出
        /// @param line_name 输出的线路名称表
        /// @return
        bool selectlinename(std::vector<std::string> &line_name);

        /// @brief 该函数会根据线路名称查询数据库中的线路数据，并将结果存储在line_data中
        /// @param line_name 由Qml选择的线路名称
        /// @param line_data 线路数据Json格式
        /// @return
        bool selectlineByLineName(const QString &line_name, std::vector<_Kits::_Orm::line_data> &lineData);

        bool selectStationByLineName(const QString &line_name, const int &line_dir, std::vector<_Kits::_Orm::line_data> &stationData);

        bool selectlineByStationNameLinedir(const QString &station_name,
                                            const int &line_dir,
                                            std::vector<_Kits::_Orm::line_data> &lineData);

        /// @brief 该函数通过任务名查找任务ID
        /// @param task_name 任务名
        /// @return
        static int searchIdByTaskName(const QString &task_name);

        /// @brief 该函数会更新某任务的updatetime字段
        /// @param task_time 结束时间
        /// @param task_id 任务ID
        /// @return
        static bool updataTaskTime(const QString &task_time, int task_id);
    };

} // namespace _Modules