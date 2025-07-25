#include "SickDeviceDeal.h"
#include "kits/thrid_devices/radar_kit/base/ChangeCenter.hpp"
#include "kits/thrid_devices/radar_kit/base/SkewData.hpp"
#include "kits/common/log/CRossLogger.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <qtmetamacros.h>
#include <qvectornd.h>
#include <string>
#include <unordered_set>
#include <utility>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <Qdebug>

using namespace _Modules;
using namespace _Kits;
using namespace TIS_Info;
SickDeviceDeal::SickDeviceDeal() : mb_getData(false)
{
    // 角度与设备ID需要注册到一个映射中
    // this.ReadYamlConfig(str_configpath);
    // 后续看看在哪实现，是全局单例的配置还是各取各
    m_skew_data = std::make_unique<SkewData>();
    m_change_center = std::make_unique<ChangeCenter>();

    mcommon_errorcode.code = SICK::CommonError::ErrorCode::NO_ANYERROR;
    mcommon_errorcode.prompt = "no error";

    m_timer_getData = new QTimer(this);
    connect(m_timer_getData, &QTimer::timeout, this, &SickDeviceDeal::OnDataTimerTimeout);
}

SickDeviceDeal::~SickDeviceDeal()
{
    // 清理资源
    m_sick_device.setSendDataPermanently(false);
    // m_sick_device.stopMeasurement();
    mb_getData = false;
}

_Kits::Err SickDeviceDeal::LastErrorCode()
{
    return Err((int)mcommon_errorcode.code, mcommon_errorcode.prompt);
}

void SickDeviceDeal::LoginDevice()
{

    if (m_sick_device.isRunningOK() && mb_device_state)
    {
        // pass
    }
    else if (m_sick_device.init(mcommon_errorcode, m_device_config.mstr_ip))
    {
        _Kits::LogInfo("sick device init success:{}", m_device_config.mstr_ip);

        mb_device_state = true;
        mb_getData = true;

        StartDetect();
    }
    else
    {
        mb_device_state = false;
        mb_getData = false;

        LogError("sick device init fail:" + m_device_config.mstr_ip + ", errcode:" + std::to_string((int)mcommon_errorcode.code) +
                 ", errdis:" + mcommon_errorcode.prompt);
    }
}
void SickDeviceDeal::ReloadDevice()
{
    try
    {
        if (m_sick_device.isRunningOK() && mb_device_state)
        {
            // pass
        }
        else
        {
            m_sick_device.rebootDevice();
            if (m_sick_device.init(mcommon_errorcode, m_device_config.mstr_ip))
            {
                _Kits::LogInfo("sick device init success:{}", m_device_config.mstr_ip);

                mb_device_state = true;
                mb_getData = true;

                StartDetect();
            }
            else
            {
                mb_device_state = false;
                mb_getData = false;

                LogError("sick device init fail:" + m_device_config.mstr_ip + ", errcode:" + std::to_string((int)mcommon_errorcode.code) +
                         ", errdis:" + mcommon_errorcode.prompt);
            }
        }
    }
    catch (std::exception &e)
    {
        LogError("sick device init fail:" + m_device_config.mstr_ip + ", err:" + e.what());
    }
    catch (...)
    {
        LogError("sick device init fail:" + m_device_config.mstr_ip + ", err:unknow");
    }
}

_Kits::DeviceSurfaceData SickDeviceDeal::GetDeviceData()
{
    mvec_one_origin_point.clear();

    // _Kits::DeviceSurfaceData device_data;
    // device_data.mus_index = m_device_config.mus_device_index;
    // device_data.mvec_data_surface.reserve(2000);
    // std::generate_n(std::back_inserter(device_data.mvec_data_surface), 1000, [y = 1100.123131]() mutable {
    //     return TIS_Info::DeviceSingleData{static_cast<float>(1000.21231231), static_cast<float>(++y * 1.1)};
    // });

    // ChangeCenterPoint(device_data.mvec_data_surface, m_device_config.m_change_center_data);

    // SkewDeviceData(device_data.mvec_data_surface, m_device_config.m_skew_angle);

    // return device_data;

    auto retVal = m_sick_device.getSingleProfileBlock(mvec_one_origin_point, mcommon_errorcode);

    std::string str;

    // 若开启All
    // echo后，还想获取最后一次回波的数据，则调用getSingleProfileLastEcho
    if (m_echoFlag == 1)
    {
        int retVal2 = m_sick_device.getSingleProfileLastEchoBlock(mvec_one_origin_point, mcommon_errorcode); // 阻塞模式

        if (retVal2 == 5 || retVal2 == -1) // 不知道为什么会返回5，先这样处理
        {
            mb_device_state = false;
            mb_getData = false;
        }
        else
        {
            return DeviceSurfaceData();
        }
    }
    else if (retVal == 5 || retVal == -1) // 不知道为什么会返回5，先这样处理
    {
        mb_device_state = false;
        mb_getData = false;
        return DeviceSurfaceData();
    }

    std::vector<DeviceSingleData> mvec_one_point;
    for (const auto &point : mvec_one_origin_point)
    {
        if (point.X == 0 && point.Y == 0) // 不需要这种点
        {
            continue;
        }

        DeviceSingleData single_data;
        single_data.mf_x = point.X;
        single_data.mf_y = point.Y;

        mvec_one_point.push_back(std::move(single_data));
    }

    ChangeCenterPoint(mvec_one_point, m_device_config.m_change_center_data);

    SkewDeviceData(mvec_one_point, m_device_config.m_skew_angle);

    auto deviceSurfaceData = DeviceSurfaceData(
        std::move(mvec_one_point), m_device_config.mus_device_index, m_device_config.mus_angle_start, m_device_config.mus_angle_end, 0);

    return deviceSurfaceData;
}

void SickDeviceDeal::ChangeCenterPoint(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData,
                                       const ChangeCenterData &change_center_data)
{
    if (DeviceSurfaceData.empty())
    {
        return;
    }
    m_change_center->ChangeCenterPoint(DeviceSurfaceData, change_center_data); // change 和 skew 可以合并
}

void SickDeviceDeal::SkewDeviceData(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees)
{
    if (DeviceSurfaceData.empty())
    {
        return;
    }
    m_skew_data->OperateSkewData(DeviceSurfaceData, angle_degrees);
}

float SickDeviceDeal::IsDeviceSkew(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees)
{
    if (DeviceSurfaceData.empty())
    {
        return angle_degrees;
    }
    return m_skew_data->IsDeviceSkew(DeviceSurfaceData, angle_degrees);
}

std::vector<TIS_Info::DeviceSingleData> SickDeviceDeal::GetParallelData(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                                                        float &height)
{
    if (!device_singledata.empty())
    {
        return m_skew_data->GetParallelData(device_singledata, height);
    }
    return std::vector<TIS_Info::DeviceSingleData>();
}
bool SickDeviceDeal::Init(const YAML::Node &config)
{
    m_device_config = DeviceConfig(
        config["ip"].as<std::string>(),
        config["type"].as<std::string>(),
        config["port"].as<unsigned short>(),
        config["device_index"].as<unsigned short>(),
        config["angle_start"].as<unsigned short>(),
        config["angle_end"].as<unsigned short>(),
        ChangeCenterData(config["change_center_data"]["change_x"].as<float>(), config["change_center_data"]["change_y"].as<float>()),
        config["skew_angle"].as<float>());

    // 角度与设备ID需要注册到一个映射中
    m_skew_data = std::make_unique<SkewData>();
    m_change_center = std::make_unique<ChangeCenter>();

    mcommon_errorcode.code = SICK::CommonError::ErrorCode::NO_ANYERROR;
    mcommon_errorcode.prompt = "no error";

    return true;
}

bool SickDeviceDeal::StartDetect()
{
    if (!mb_device_state)
    {
        return false;
    }

    int ret = m_sick_device.startMeasurement();
    if (ret != 1)
    {
        LogError("Failed to start measurement. errcode:" + std::to_string(ret));
        // return false;
    }

    ret = m_sick_device.setSendDataPermanently(false);
    if (ret != 1)
    {
        LogError("Failed to setSendDataPermanently. errcode:" + std::to_string(ret));
        // return false;
    }

    m_echoFlag = 2; // 2:Last echo
    ret = m_sick_device.setEchoFilter(m_echoFlag);
    if (ret != 1)
    {
        LogError("Failed to getEchoFilter. errcode:" + std::to_string(ret));
        // return false;
    }

    ret = m_sick_device.setSendDataPermanently(true);
    if (ret != 1)
    {
        LogError("Failed to setSendDataPermanently. errcode:" + std::to_string(ret));
        // return false;
    }

    return ret == 1;
}

bool SickDeviceDeal::StopDetect()
{
    if (!mb_device_state)
    {
        return false;
    }

    int ret = m_sick_device.stopMeasurement();

    return ret == 1;
}

void SickDeviceDeal::GetDataThread()
{
    m_timer_getData->start(15); // 设置定时器为50ms
}

void SickDeviceDeal::OnDataTimerTimeout()
{
    // if (mb_getData)
    if (true)
    {
        emit deviceDataGet(m_device_config.mus_device_index);
    }
}

void SickDeviceDeal::CompensateData()
{
    // pass
}