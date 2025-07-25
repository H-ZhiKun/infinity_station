#pragma once

#include <QString>
#include "kits/common/database/orm/TableStructs.h"
#include <vector>
#include "OfflineSqlOp.h"
#include "OfflineDateStatistic.h"

namespace _Modules
{

    class OfflineDateStatistic
    {
      public:
        enum WhereType
        {
            WhereType_None = 0,
            WhereType_Station,
            WhereType_PoleName,
            WhereType_Kilometer,
            WhereType_LineType,
            WhereType_PointCount,
            WhereType_OverrunTime,
            WhereType_Direction
        };

        enum Condition
        {
            EQUAL = 0,
            NOT_EQUAL = 1,
            LESS_THAN = 2,
            LESS_THAN_EQUAL = 3,
            GREATER_THAN = 4,
            GREATER_THAN_EQUAL = 5,
        };

        enum OrderType
        {
            OrderType_Id,
            OrderType_Kilometer,
            OrderType_OverrunTime,
            OrderType_PointCount,
            OrderType_CreateTime
        };

        enum OrderCondition
        {
            NONE = -1,
            ASC,
            DESC
        };

        struct OverInfoOffline
        {
            int mi_id = 0;
            QString mstr_station; // 站区
            QString mstr_pole_name;
            float mf_kilometer;
            QString mstr_linetype;
            int mi_point_count = 0;
            int mi_overrun_time = 0;
            QString mstr_direction;
            QDateTime mdt_create_time;

            bool empty()
            {
                return mi_id == 0 || mstr_station.isEmpty() || mstr_pole_name.isEmpty() || mf_kilometer == 0 || mstr_linetype.isEmpty() ||
                       mi_point_count == 0 || mi_overrun_time == 0 || mstr_direction.isEmpty() || mdt_create_time.isNull();
            }
        };

        OfflineDateStatistic();

        ~OfflineDateStatistic();

        // 处理超限数据并根据站区分类    直接存储到内存
        bool overInfoStatistic(std::vector<_Kits::_Orm::radar_over_data> &);

        // 获取处理后的结果
        std::vector<OverInfoOffline> getResults();

        // 条件筛选
        template <typename T>
        const OfflineDateStatistic &where(WhereType &&type, Condition &&op, T &&value);

        // 排序
        const OfflineDateStatistic &order(OrderCondition o_condition, OrderType order);

        // 普通的查询只用这个
        bool excute();

        // 重置结果
        bool reset();

      private:
        // 内部缓存区，存储所有超限分析后结果，在考虑是否需要开放
        std::vector<OverInfoOffline> mvec_over_info_offline;
        // 内部缓存区，条件筛选用
        std::vector<OverInfoOffline> mvec_over_info_offline_filter;

        OfflineSqlOp m_sqlop;

        std::pair<OrderCondition, OrderType> m_order_type;

        struct ConditionEntry
        {
            WhereType type;
            Condition op;
            std::variant<int, float, QString> value;

            bool evaluate(const OverInfoOffline &info) const
            {
                switch (type)
                {
                case WhereType_Station:
                    return compare(info.mstr_station, std::get<QString>(value));
                case WhereType_Kilometer:
                    return compare(info.mf_kilometer, std::get<float>(value));
                case WhereType_OverrunTime:
                    return compare(info.mi_overrun_time, std::get<int>(value));
                // 其他类型处理...
                default:
                    return false;
                }
            }

            template <typename T>
            bool compare(const T &field, const T &val) const
            {
                switch (op)
                {
                case EQUAL:
                    return field == val;
                case NOT_EQUAL:
                    return field != val;
                case LESS_THAN:
                    return field < val;
                case LESS_THAN_EQUAL:
                    return field <= val;
                case GREATER_THAN:
                    return field > val;
                case GREATER_THAN_EQUAL:
                    return field >= val;
                default:
                    return false;
                }
            }
        };

        std::vector<ConditionEntry> m_conditionStack; // 条件存储栈
    };

} // namespace _Modules