#include "OfflineDateStatistic.h"

using namespace _Modules;

// 构造函数（初始化成员变量）
OfflineDateStatistic::OfflineDateStatistic() 
    : mvec_over_info_offline(),
      mvec_over_info_offline_filter(),
      m_sqlop(),
      m_order_type(std::make_pair(static_cast<OrderCondition>(-1), static_cast<OrderType>(0))),
      m_conditionStack() 
{

}

// 析构函数（需与头文件声明保持一致）
OfflineDateStatistic::~OfflineDateStatistic()
{

}

// 核心数据处理实现
bool OfflineDateStatistic::overInfoStatistic(std::vector<_Kits::_Orm::radar_over_data>& input) {
    // 清空旧数据
    mvec_over_info_offline.clear();
    mvec_over_info_offline_filter.clear();
    
    // 转换数据结构
    for (const auto& item : input) {
        OverInfoOffline info;
        info.mi_id = item.id;  // 假设radar_over_data包含id字段

        auto location_info = m_sqlop.GetLocationDataById(item.location_id_start);

        if (!location_info.empty()) {
            info.mstr_station = location_info.begin()->station_name;
            info.mstr_pole_name = location_info.begin()->pole_name;
            info.mf_kilometer = location_info.begin()->kilo_meter;
            info.mstr_linetype = "";
        }
        //判断倾线点数量
        auto calculatePointCount = [](const QString& points) -> int {
            int count = 0;
            if (!points.isEmpty()) {
                QString searchKey = "\"x\":";
                int pos = 0;
                while ((pos = points.indexOf(searchKey, pos)) != -1) {
                    count++;
                    pos += searchKey.length();
                }
            }
            return count;
        };
        info.mi_point_count =calculatePointCount(item.points);
        info.mi_overrun_time = item.overrun_time;
        info.mstr_direction = item.direction;
        info.mdt_create_time = QDateTime::fromString(
            item.created_time, 
            "yyyy-MM-dd HH:mm:ss.zzz"
        );

        mvec_over_info_offline.push_back(info);
        mvec_over_info_offline_filter.push_back(info);
    }
    
    return !mvec_over_info_offline.empty() && !mvec_over_info_offline_filter.empty();
}

// 结果获取
std::vector<OfflineDateStatistic::OverInfoOffline> OfflineDateStatistic::getResults() {
    
    return mvec_over_info_offline_filter; 
}


// 条件筛选
template<typename T>
const OfflineDateStatistic& OfflineDateStatistic::where(WhereType &&type, Condition &&op, T &&value) {
    if(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, QString>)
        m_conditionStack.push_back({type, op, value});
    return *this;
}


// 排序设置
const OfflineDateStatistic& OfflineDateStatistic::order(OrderCondition o_condition, OrderType order) {
    m_order_type = std::make_pair(o_condition, order);
    // 实际应解析order并执行排序，这里返回对象本身以支持链式调用
    return *this;
}

bool OfflineDateStatistic::excute() {
    if (m_order_type.second == -1 && m_conditionStack.empty())
    {
        return true;
    }

    // Where
    for (const auto& cond : m_conditionStack) {
        std::vector<OverInfoOffline> temp;
        
        std::copy_if(mvec_over_info_offline_filter.begin(), mvec_over_info_offline_filter.end(),
            std::back_inserter(temp),
            [&cond](const OverInfoOffline& info) {
                return cond.evaluate(info);
            });

        mvec_over_info_offline_filter = std::move(temp);
    }
    
    // Order
    if (m_order_type.first != OrderCondition::NONE) {
        auto comparator = [this](const OverInfoOffline& a, const OverInfoOffline& b) {
            const auto& [condition, type] = m_order_type;
            
            // 统一比较函数
            auto compare = [&]() -> bool {
                switch(condition) {
                    case OrderType::OrderType_Id:
                        return a.mi_id < b.mi_id;
                    case OrderType::OrderType_Kilometer:
                        return a.mf_kilometer < b.mf_kilometer;
                    case OrderType::OrderType_PointCount:
                        return a.mi_point_count < b.mi_point_count;
                    case OrderType::OrderType_OverrunTime:
                        return a.mi_overrun_time < b.mi_overrun_time;
                    case OrderType::OrderType_CreateTime:
                        return a.mdt_create_time < b.mdt_create_time;
                    default: 
                        return false;
                }
            };

            // 根据排序类型反转结果
            return (type == OrderCondition::ASC) ? compare() : !compare();
        };

        // 执行排序
        std::sort(mvec_over_info_offline_filter.begin(),
                 mvec_over_info_offline_filter.end(),
                 comparator);
    }
    

    return true;
}

