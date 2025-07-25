#include "RadarService.h"
#include "kits/thrid_devices/radar_kit/base/DeviceData.hpp"
#include "kits/thrid_devices/radar_kit/base/RadarFactory.h"

#if defined(USE_KIT_RADAR_SICK)
#include "kits/thrid_devices/radar_kit/sick/SickDeviceDeal.h"
#endif

#if defined(USE_KIT_RADAR_YANYAN)
#include "kits/thrid_devices/radar_kit/yanyan/YanYanDeviceDeal.h"
#endif

#include "kits/common/log/CRossLogger.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <filesystem>
#include <fstream>

#include "tis_global/Struct.h"

using namespace _Modules;
using namespace _Kits;
using namespace TIS_Info;

RadarService::RadarService()
{
    m_threadPool = std::make_unique<ThreadPool>(6);
}

RadarService::~RadarService()
{
}

bool RadarService::init(const YAML::Node &config)
{
    mqvec_data_send_qml.reserve(5);

    auto &radarFactory = RadarFactory::getInstance();

#if defined(USE_KIT_RADAR_SICK)
    radarFactory.registerRadar<SickDeviceDeal>("SICK");
#endif

#if defined(USE_KIT_RADAR_YANYAN)
    radarFactory.registerRadar<YanYanDeviceDeal>("YANYAN"); // 注册雷达类型YANYAN
#endif

    g_radar_record_sql.SetMaxMemorySize(250); // 250条sql记录一次

    auto str_config_path = config["radar_path"].as<std::string>();

    if (str_config_path.empty())
    {
        return false;
    }

    auto radar_config = YAML::LoadFile(std::filesystem::current_path().string() + str_config_path);

    m_config = radar_config;

    for (const auto &deviceinfo : m_config["radar_info"])
    {

        auto deviceDeal = radarFactory.createRadar(deviceinfo["type"].as<std::string>());

        if (deviceDeal == nullptr)
        {
            continue;
        }

        bool ok = connect(deviceDeal.get(), &DeviceDeal::deviceDataGet, this, &RadarService::OnDeviceDataGet);
        Q_ASSERT(ok);
        if (!ok)
            qWarning() << "信号槽连接失败";
        deviceDeal->Init(deviceinfo);

        // 先绑定设备和点云数据
        // 每个设备的角度开始，角度结束，时间戳， 以及点云数据
        mmap_device_data[deviceinfo["device_index"].as<unsigned short>()] = DeviceSurfaceData();
        mmap_device_state[deviceinfo["device_index"].as<unsigned short>()] = false;

        mmap_device_deal[deviceinfo["device_index"].as<unsigned short>()] = (std::move(deviceDeal));
    }

    for (const auto &overinfo : m_config["overrun_info"])
    {
        OverrunInfo overrun_info;

        overrun_info.line_info = {overinfo["line_start"].as<uint16_t>(), overinfo["line_end"].as<uint16_t>()};

        for (const auto &point_info : overinfo["point_info"])
        {
            overrun_info.point_info.push_back({point_info["x"].as<float>(), point_info["y"].as<float>()});
        }

        mvec_overrun_info.push_back(overrun_info);
    }

    m_skew_info.distance = m_config["skew_info"]["distance"].as<float>();
    m_skew_info.angle_1 = m_config["skew_info"]["angle_1"].as<float>(); // 筛选【angle_1，angle_2】的点，判断坐标系扶正的点云数据
    m_skew_info.angle_2 = m_config["skew_info"]["angle_2"].as<float>();

    mthread_device_data_normal = std::thread(&RadarService::_deviceDataNormal, this);
    mthread_device_data_overrun = std::thread(&RadarService::_deviceDataOverrun, this);
    m_thread_sqlRecord = std::thread(&RadarService::_sqlRecord, this);

    return true;
}
bool RadarService::start()
{
    { // 开启雷达重启线程

        m_timer_restart = new QTimer(this);
        connect(m_timer_restart, &QTimer::timeout, this, &RadarService::_OnTimerRestart);
        m_timer_restart->start(1000 * 10);
    }

    { // 开启雷达发送给Qml线程
        m_timer_send_qml = new QTimer(this);
        connect(m_timer_send_qml, &QTimer::timeout, this, &RadarService::_OnTimerDataSendQml);
        m_timer_send_qml->start(200);
    }

    emit OverinfoSendQml("开启限界系统成功");
    try
    {
        emit OverconfigSendQml(mvec_overrun_info[0].point_info);
    }
    catch (std::exception &e)
    {
        emit OverinfoSendQml("限界超限数据获取失败, 请联系研发调配置");
    }
    return true;
}
void RadarService::OnDeviceDataGet(unsigned short device_index)
{
    std::lock_guard<std::mutex> lock(m_mutex_dataget);
    auto device_data = std::move(mmap_device_deal[device_index]->GetDeviceData());
    if (device_data.mvec_data_surface.empty())
    {
        return;
    }

    // --------------------test--------------------
    // _Kits::DeviceSurfaceData device_data;
    // device_data.mus_index = device_index;
    // device_data.mvec_data_surface.reserve(2000);
    // std::generate_n(std::back_inserter(device_data.mvec_data_surface), 2000, [y = 1100.123131]() mutable {
    //     return TIS_Info::DeviceSingleData{static_cast<float>(1000.21231231), static_cast<float>(++y * 1.1)};
    // });

    // _Kits::DeviceSurfaceData device_data1;
    // device_data1.mus_index = device_index + 1;
    // device_data1.mvec_data_surface.reserve(2000);
    // std::generate_n(std::back_inserter(device_data1.mvec_data_surface), 2000, [x = 1100.123131]() mutable {
    //     return TIS_Info::DeviceSingleData{static_cast<float>(++x * 1.1), static_cast<float>(1000.21231231)};
    // });

    // mmap_device_data[device_index + 1] = std::move(device_data1);
    // --------------------test--------------------

    mmap_device_data[device_index] = std::move(device_data);
}

bool RadarService::stop()
{
    mb_stop.store(true);

    if (mthread_device_data_normal.joinable())
    {
        mthread_device_data_normal.join();
    }

    if (mthread_device_data_overrun.joinable())
    {
        mthread_device_data_overrun.join();
    }

    if (m_thread_sqlRecord.joinable())
    {
        m_thread_sqlRecord.join();
    }

    return true;
}

void RadarService::_OnTimerRestart()
{

    for (auto &device : mmap_device_deal)
    {
        if (device.second->GetDeviceState() == false)
        {
            m_threadPool->enqueue([this, device]() { device.second->ReloadDevice(); });
        }
        emit StateSendQml(QString("设备%1的状态:%2").arg(device.first).arg(device.second->GetDeviceState() ? "在线" : "掉线"));
    }
}

void RadarService::_OnTimerDataSendQml()
{
    if (mp_data_all_for_qml == nullptr || mp_data_all_for_qml->mvec_data_surface.size() == 0)
    {
        return;
    }

    auto points = std::make_shared<std::vector<TIS_Info::DeviceSingleData>>();

    // 收集点云数据
    { // 锁住mp_data_all_for_qml
        std::lock_guard<std::mutex> lock(m_mutex_data_all_for_qml);

        points->insert(points->end(), mp_data_all_for_qml->mvec_data_surface.begin(), mp_data_all_for_qml->mvec_data_surface.end());
    }

    // 发送数据到 QML
    emit DataSendQml(points);
}

void RadarService::_sqlRecord()
{
    // 在2500个点云数据的情况下，耗时1ms左右
    while (!mb_stop.load())
    {
        if (this->m_task_info.nTaskID == -1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        auto location_data = g_radar_record_sql.ReadyToWrite();

        if (location_data.line_id == -1 || location_data.id == -1)
        {
            continue;
        }

        auto obj_all = gpool_device_data_all->getObject(7600);
        auto obj_one = gpool_device_data_one->getObject(600);

        obj_all->mstr_createtime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");

        bool isChangeDevsData = false;
        float angle_offset = 0.0f;
        float origin_angle = 0.0f;
        // 保存雷达数据 全帧的数据坐标根据角度筛选 点云点

        int nAll_Size = 0;
        int nNow_Size = 0;

        for (auto &data : mmap_device_data)
        {
            nAll_Size += data.second.mvec_data_surface.size();
        }

        if (nAll_Size <= 100)
        {
            continue;
        }

        if (nAll_Size != obj_all->mvec_data_surface.size())
        {
            obj_all->mvec_data_surface.resize(nAll_Size);
        }

        for (auto &data : mmap_device_data)
        {
            if (!data.second.mvec_data_surface.empty())
            {
                try
                {
                    std::lock_guard<std::mutex> lock(m_mutex_dataget);
                    int nStart = 0;
                    for (; nNow_Size < nAll_Size && nStart < data.second.mvec_data_surface.size(); ++nNow_Size, ++nStart)
                    {
                        obj_all->mvec_data_surface[nNow_Size] = data.second.mvec_data_surface[nStart];
                    }
                }
                catch (std::exception &e)
                {
                    qDebug() << "数据异常" << e.what();
                    continue;
                }
            }
        }

        for (auto &point : obj_all->mvec_data_surface)
        {

            if (m_skew_info.angle_1 == 0 && m_skew_info.angle_2 == 0)
            {
                break; // 如果角度筛选为0，直接跳过
            }
            // 以坐标原点为基准计算角度，知道点所处位置，进而筛选
            float angle_actual = atan2(point.mf_y, point.mf_x) * 180 / M_PI; // 计算实际角度
            angle_actual = fmod(angle_actual + 360.0f, 360.0f);              // 统一到0，360 范围
            if (m_skew_info.angle_1 < m_skew_info.angle_2)                   // 普通区间
            {
                if (angle_actual > m_skew_info.angle_1 && angle_actual < m_skew_info.angle_2)
                {
                    obj_one->mvec_data_surface.push_back(point);
                }
            }
            else // 跨0 °区间
            {
                if (angle_actual > m_skew_info.angle_1 || angle_actual < m_skew_info.angle_2)
                {
                    obj_one->mvec_data_surface.push_back(point);
                }
            }
        }

        // 判断设备偏移回正
        if (obj_one->mvec_data_surface.size() > 2) // 逻辑感觉上有问题，需要修改
        {

            if (m_skew_info.distance != 0)
            {
                float height = m_skew_info.distance;
                // zzh，认为distence应该单独从配置中获取
                auto paralle_points = mmap_device_deal[obj_one->mus_index]->GetParallelData(obj_one->mvec_data_surface,
                                                                                            height); // 依据height选取雷达返回铁轨的坐标

                angle_offset = mmap_device_deal[obj_one->mus_index]->IsDeviceSkew(paralle_points, origin_angle);
                if (abs(angle_offset) > 10 && paralle_points.size() > 0) // 判断雷达返回 平行部分的数据不为空
                {
                    isChangeDevsData = true;

                    mmap_device_deal[obj_one->mus_index]->SkewDeviceData(obj_all->mvec_data_surface, angle_offset);
                }
            }
        }

        obj_all->mi_location_id = location_data.created_time;
        obj_all->mi_line_id = location_data.line_id;

        m_queue_device_data_normal.push(obj_all);
        m_queue_device_data_overrun.push(obj_all);

        std::lock_guard<std::mutex> lock(m_mutex_data_all_for_qml);
        mp_data_all_for_qml = obj_all;

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void RadarService::notifyTask(TIS_Info::TaskInfo data)
{
    if (data.isRun())
    {
        m_task_info = data;
        _Kits::LogInfo("RadarService::notifyTask(), 收到任务, id:{}, filepath:{}", data.nTaskID, data.strTaskSavePath.toStdString());
    }
    else
    {
        m_task_info.clear();
    }
}

QString RadarService::GetDirection(float angle_deg)
{
    angle_deg = std::fmod(angle_deg + 360.0, 360.0); // 确保 0-360° 范围

    // 根据角度划分方向
    if (angle_deg <= 15 || angle_deg > 345)
    { //
        return "右";
    }
    else if (angle_deg <= 75)
    { //
        return "右上";
    }
    else if (angle_deg <= 105)
    { //
        return "上";
    }
    else if (angle_deg <= 165)
    { //
        return "左上";
    }
    else if (angle_deg <= 195)
    { //
        return "左";
    }
    else if (angle_deg <= 255)
    { //
        return "左下";
    }
    else if (angle_deg <= 295)
    { //
        return "下";
    }
    else
    { // 扩展右下
        return "右下";
    }
}

void RadarService::_deviceDataNormal()
{
    while (!mb_stop.load())
    {
        if (m_task_info.nTaskID == -1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        std::shared_ptr<_Kits::DeviceSurfaceData> data;

        // 使用非阻塞方式从队列取数据
        if (m_queue_device_data_normal.try_pop(data))
        {
            if (data && !data->mvec_data_surface.empty())
            {

                m_threadPool->enqueue(
                    [](std::shared_ptr<_Kits::DeviceSurfaceData> data, TIS_Info::TaskInfo &task_info) {
                        // 记录数据到数据库
                        g_radar_record_sql.OnRecordRadar(data, data->mstr_createtime, data->mstr_createtime, task_info.nTaskID, 0);
                    },
                    data,
                    std::ref(m_task_info));
            }
        }
        else
        {
            // 队列为空时休眠，避免忙等待
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void RadarService::DataRecordSql(std::shared_ptr<_Kits::DeviceSurfaceData> data, TIS_Info::TaskInfo &task_info)
{
    g_radar_record_sql.OnRecordRadar(data, data->mstr_createtime, data->mi_location_id, task_info.nTaskID, 0);
}

void RadarService::_deviceDataOverrun()
{
    while (!mb_stop.load())
    {
        if (m_task_info.nTaskID == -1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        std::shared_ptr<_Kits::DeviceSurfaceData> data;

        if (m_queue_device_data_overrun.try_pop(data))
        {
            if (data == nullptr || data->mvec_data_surface.empty())
            {
                continue;
            }

            int index = mi_index;
            for (; index < mvec_overrun_info.size(); index++)
            {
                if (data->mi_line_id >= mvec_overrun_info[index].line_info.line_start &&
                    data->mi_line_id <= mvec_overrun_info[index].line_info.line_end)
                {
                    break;
                }
            }

            if (index >= mvec_overrun_info.size())
            {
                index = mvec_overrun_info.size() - 1;
            }

            if (index != mi_index)
            {
                emit OverconfigSendQml(mvec_overrun_info[index].point_info);
                mi_index = index;
            }

            auto vec_over_data = std::move(m_radar_overrun.isOverrun(data,
                                                                     mvec_overrun_info[index].point_info)); // 需要根据线路信息来调整超限值
            // zzh
            for (auto &vec_one : vec_over_data)
            {
                if (vec_one.empty())
                {
                    continue;
                }

                auto direction = GetDirection(vec_one.back()->mus_angle_start);

                QDateTime createTime = QDateTime::fromString(vec_one.front()->mstr_createtime, "yyyy-MM-dd HH:mm:ss.zzz");

                QDateTime updateTime = QDateTime::fromString(vec_one.back()->mstr_updatetime, "yyyy-MM-dd HH:mm:ss.zzz");
                int duration_ms = createTime.msecsTo(updateTime);

                QString logInfo = QString("超限在%1区域发生, 发生时间%2, 结束时间%3, 耗时%4ms")
                                      .arg(direction)
                                      .arg(vec_one.front()->mstr_createtime)
                                      .arg(vec_one.back()->mstr_updatetime)
                                      .arg(duration_ms);

                m_threadPool->enqueue(
                    [](std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>> vec_one,
                       TIS_Info::TaskInfo &task_info,
                       int overrun_time,
                       QString direction) {
                        // 记录数据到数据库
                        g_radar_record_sql.OnRecordOverrun(vec_one,
                                                           vec_one.front()->mstr_createtime,
                                                           vec_one.back()->mstr_updatetime,
                                                           vec_one.front()->mi_location_id,
                                                           vec_one.back()->mi_location_id,
                                                           overrun_time,
                                                           direction,
                                                           task_info.nTaskID,
                                                           vec_one.back()->mi_zSize);
                    },
                    vec_one,
                    std::ref(m_task_info),
                    duration_ms,
                    direction);

                // 在此将超限信息提出
                emit OverinfoSendQml(std::move(logInfo));
            }
        }
        else
        {
            // 队列为空时休眠，避免忙等待
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
