#include "RadarOverrun.h"
#include <QDateTime>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "tis_global/Struct.h"
#include "kits/thrid_devices/radar_kit/base/DeviceData.hpp"

using namespace _Modules;
using namespace TIS_Info;
using namespace _Kits;
std::vector<std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>>> RadarOverrun::isOverrun(
    std::shared_ptr<_Kits::DeviceSurfaceData> detect_data, std::vector<DeviceSingleData> &polygon)
{
    std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>> overrun_list_now;

    if (polygon.size() < 3)
        return std::vector<std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>>>(); // 至少需要三个点来构成一个多边形

    auto obj = gpool_device_data_one->getObject(600);

    bool in_polygon = false;

    int iCondition = 0;
    int iCondition_end = 5;

    /*
    1.超限区域检测阶段
        遍历检测点集 detect_data
        通过 isPointInPolygon 判断是否进入多边形区域
        使用对象池 DeviceSurfaceDataPool 管理 DeviceSurfaceData 对象
        通过连续 5 个外点 (iCondition_end=5) 判定超限区域结束
    */
    try
    {
        for (const auto &point : detect_data->mvec_data_surface)
        {

            if (point.mf_x == 0 && point.mf_y == 0)
            {
                continue; // 跳过无效点
            }

            if (isPointInPolygon(point, polygon))
            {
                if (!in_polygon)
                {
                    // 开始一个新的超限区域
                    if (obj == nullptr)
                    {
                        obj = gpool_device_data_one->getObject(600);
                    }
                    in_polygon = true;
                }
                obj->mvec_data_surface.push_back(point);
                obj->mi_zSize++;
                iCondition = 0; // 如果上一个点没超限，这个点超限了，就需要去除刚刚的非超限记录
            }
            else
            {
                ++iCondition;

                if (iCondition >= iCondition_end)
                {
                    // 超过一定次数，认为超限区域结束
                    if (in_polygon)
                    {
                        if (obj->mvec_data_surface.size() > 5) // 过于少的点视为无效超限
                        {
                            // 结束当前的超限区域
                            obj->mus_angle_start = calculateOffsetAngle(
                                obj->mvec_data_surface[obj->mvec_data_surface.size() / 2 - 1].mf_x,
                                obj->mvec_data_surface[obj->mvec_data_surface.size() / 2 - 1].mf_y); // 求出中点偏移角度

                            // obj->mvec_data_surface.clear(); //后不保存，只取超限角度和普通id帧号

                            overrun_list_now.push_back(obj);
                        }

                        in_polygon = false;
                    }
                    iCondition = 0;
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        qDebug() << "RadarOverrun::isOverrun error:" << e.what();
    }

    // 如果最后一个点还在超限区域内，需要将最后一个超限区域添加到列表中
    if (in_polygon && !obj->mvec_data_surface.empty())
    {
        overrun_list_now.push_back(obj);
    }

    // 超限匹配阶段
    // 对当前帧 overrun_list_now 和上一帧 m_overrun_list_before 按起始角度排序
    // 计算最小可接受角度差 idifVal
    // 通过角度匹配新旧超限区域
    //      匹配成功：继承索引和时间戳
    //      匹配失败：记录待回收索引

    std::vector<int> vec_index_no_use;

    // if (m_overrun_list_before.size() > 0)

    std::sort(overrun_list_now.begin(),
              overrun_list_now.end(),
              [](const std::shared_ptr<_Kits::DeviceSurfaceData> a, const std::shared_ptr<_Kits::DeviceSurfaceData> b) {
                  return a->mus_angle_start < b->mus_angle_start;
              });

    std::sort(m_overrun_list_before.begin(),
              m_overrun_list_before.end(),
              [](const std::shared_ptr<_Kits::DeviceSurfaceData> a, const std::shared_ptr<_Kits::DeviceSurfaceData> b) {
                  return a->mus_angle_start < b->mus_angle_start;
              });

    // 找到最小可接受偏移角度
    float idifVal = 10.0f; // 加配置

    // 匹配超限
    std::shared_ptr<_Kits::DeviceSurfaceData> matchedNowItem = nullptr;

    for (auto &beforeItem : m_overrun_list_before)
    {
        if (!beforeItem)
            continue;

        // 查找最近角度匹配项
        float minDiff = 360.0f; // 最大可能角度差

        for (auto nowItem : overrun_list_now)
        {
            if (!nowItem)
                continue;

            // 计算环形角度差
            float diff = fabs(nowItem->mus_angle_start - beforeItem->mus_angle_start);
            diff = std::min(diff, 360.0f - diff);

            if (diff < minDiff && diff <= idifVal)
            {
                // 这里要一直找出最匹配项
                minDiff = diff;
                matchedNowItem = nowItem;
            }
        }

        // 找到匹配项则更新
        if (matchedNowItem)
        {
            // 标记已匹配，避免重复处理  这是依旧持续的超限
            matchedNowItem->mb_isMatch = true;
            matchedNowItem->mus_index = beforeItem->mus_index;
            matchedNowItem->mstr_createtime = beforeItem->mstr_createtime;
            matchedNowItem->mi_zSize += beforeItem->mi_zSize; // 点云数增加

            matchedNowItem = nullptr;
        }
        else
        {
            // 已经结束的超限，需要记录index
            vec_index_no_use.push_back(beforeItem->mus_index);
        }
    }

    /*
    数据更新阶段

        清理上一帧数据 m_overrun_list_before.clear()
        处理新增/持续超限：
        新超限：分配哈希索引，记录创建时间
        持续超限：存入哈希表对应位置
        克隆当前数据到 m_overrun_list_before

    */

    // 清理上次的超限数据
    m_overrun_list_before.clear();

    // 处理新增项（未被匹配的 nowItem）
    for (auto nowItem : overrun_list_now)
    {
        if (nowItem == nullptr)
        {
            continue;
        }

        unsigned int index;

        if (!nowItem->mb_isMatch)
        { // 新超限
            index = getHashIndex();
            while (index == -1)
            {
                index = getHashIndex();
            }

            nowItem->mstr_createtime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
            nowItem->mus_index = index;
            nowItem->mi_location_id = m_radar_record_sql.ReadyToWrite().created_time;
            nowItem->mb_isMatch = false;
        }
        else
        { // 旧超限
            index = nowItem->mus_index;
            nowItem->mb_isMatch = false;
        }

        m_overrun_list_before.push_back(nowItem); // 深拷贝一次
        m_overrun_hash[index].push_back(nowItem);
    }

    std::vector<std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>>> overrun_to_sql;
    for (auto &item : vec_index_no_use)
    {
        // 单次查找并直接使用结构化绑定
        if (auto it = m_overrun_hash.find(item); it != m_overrun_hash.end())
        {
            auto &[key, vec_data] = *it; // C++17结构化绑定解引用迭代器

            vec_data.back()->mstr_updatetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
            // 转移数据所有权
            overrun_to_sql.push_back(vec_data);

            // 直接通过迭代器擦除（O(1)复杂度）
            m_overrun_hash.erase(it);

            // 加锁归还索引
            std::lock_guard<std::mutex> lock(m_hash_mutex);
            returnIndex(key); // 使用解绑后的key
        }
    }

    return overrun_to_sql;
}

/*
多边形：A(0,0) → B(2,0) → C(1,2) → 闭合到A
点：P(1,1)

步骤：
1. 检查P是否在AB、BC、CA边上（本例中不在）。
2. 从P(1,1)向右发射水平射线。
3. 射线与边CA（从C(1,2)到A(0,0)）相交：
   - 纵坐标跨越：C(2>1), A(0<1) → 满足。
   - 交点横坐标计算：x = 0 + (1-0)/(2-0)*(1-0) = 0.5 → P(1) < 0.5? 不成立（未相交）。
4. 射线与边BC（从B(2,0)到C(1,2)）相交：
   - 纵坐标跨越：B(0<1), C(2>1) → 满足。
   - 交点横坐标计算：x = 2 + (1-0)/(2-0)*(1-2) ≈ 1.5 → P(1) < 1.5 → 相交！
5. 交点数为1（奇数）→ 返回 true（点在内）。
*/
bool RadarOverrun::isPointInPolygon(const DeviceSingleData &point, const std::vector<DeviceSingleData> &polygon)
{
    int n = polygon.size();
    if (n < 3)
        return false;
    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++)
    {
        double xi = polygon[i].mf_x, yi = polygon[i].mf_y;
        double xj = polygon[j].mf_x, yj = polygon[j].mf_y;

        // 判断点是否在多边形边上
        double minX = std::min(xi, xj), maxX = std::max(xi, xj);
        double minY = std::min(yi, yj), maxY = std::max(yi, yj);
        double dx = xj - xi, dy = yj - yi;
        if (fabs(dy) > 1e-10)
        {
            double t = (point.mf_y - yi) / dy;
            if (t >= 0 && t <= 1)
            {
                double xt = xi + t * dx;
                if (fabs(point.mf_x - xt) < 1e-10) // 在边上
                    return true;
            }
        }
        else if (fabs(point.mf_y - yi) < 1e-10 && point.mf_x >= minX && point.mf_x <= maxX)
        {
            // 在水平边上
            return true;
        }

        // 射线法
        // yi + 1e-20 防止除以0
        bool intersect = ((yi > point.mf_y) != (yj > point.mf_y)) && (point.mf_x < (xj - xi) * (point.mf_y - yi) / (yj - yi + 1e-20) + xi);
        if (intersect)
            inside = !inside;
    }
    return inside;
}

double RadarOverrun::calculateOffsetAngle(double x, double y)
{
    double theta_rad = atan2(y, x);                // 计算弧度
    double theta_deg = theta_rad * (180.0 / M_PI); // 转换为角度
    theta_deg = fmod(theta_deg + 360.0, 360.0);
    return theta_deg;
}

unsigned int RadarOverrun::getHashIndex()
{
    // 修改为引用遍历
    std::lock_guard<std::mutex> lock(m_hash_mutex);
    for (auto &[key, used] : m_overrun_hash_index)
    { // C++17结构化绑定
        if (!used)
        {
            used = true; // 直接修改原数据
            return key;
        }
    }

    expandHashTable(10); // 默认扩展10个新位置

    // 再次遍历（同样需要引用）
    for (auto &[key, used] : m_overrun_hash_index)
    {
        if (!used)
        {
            used = true;
            return key;
        }
    }

    return -1; // 应改为 throw 更合理
}

void RadarOverrun::expandHashTable(int newSize)
{
    if (newSize <= 0)
        return;

    const int startIndex = m_overrun_hash_index.empty() ? 0 : (m_overrun_hash_index.begin()->first + 1); // 获取当前最大索引+1

    for (int i = 0; i < newSize; ++i)
    {
        m_overrun_hash_index[startIndex + i] = false; // 线性递增
    }
}

void RadarOverrun::returnIndex(int i)
{
    if (auto it = m_overrun_hash_index.find(i); it != m_overrun_hash_index.end())
    {
        it->second = false; // 确保操作有效性
    }
}