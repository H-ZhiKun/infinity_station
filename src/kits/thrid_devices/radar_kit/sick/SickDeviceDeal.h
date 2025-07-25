#ifndef _SICK_DEVICE_DEAL_H_
#define _SICK_DEVICE_DEAL_H_

#include "kits/thrid_devices/radar_kit/base/DeviceData.hpp"
#include "kits/thrid_devices/radar_kit/base/DeviceDeal.h"
#include "SickSingleData.h"
#include "sick_radar/SICKLidarSensor.h"
#include "sick_radar/SICKLidarSensorData.h"
#include "yaml-cpp/yaml.h"
#include <functional>
#include <queue>
#include <qvectornd.h>
#include <string>
#include <vector>

namespace _Modules
{

    class SickDeviceDeal : public DeviceDeal
    {
        Q_OBJECT

      public:
        SickDeviceDeal();

        SickDeviceDeal(const YAML::Node &config);
        SickDeviceDeal(const SickDeviceDeal &) = delete;
        SickDeviceDeal &operator=(const SickDeviceDeal &) = delete;

        ~SickDeviceDeal();

      public:
        // int DeviceNum() final;

        _Kits::Err LastErrorCode() override;

        _Kits::DeviceSurfaceData GetDeviceData() override;

        void ReloadDevice() override;

        // 改变中心坐标系
        void ChangeCenterPoint(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData,
                               const _Kits::ChangeCenterData &change_center_data) override;

        // 倾斜设备数据
        void SkewDeviceData(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees) override;
        // 判断设备是否扶正 返回需要扶正的角度
        virtual float IsDeviceSkew(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees) override;
        virtual std::vector<TIS_Info::DeviceSingleData> GetParallelData(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                                                        float &height) override;

        // 登陆设备
        void LoginDevice() override;

        bool Init(const YAML::Node &) override;

        bool StartDetect() override;
        bool StopDetect() override;

        void CompensateData() override;

      private:
        int mi_device_num;
        SICK::SICKLidarSensor m_sick_device;

        SICK::CommonError mcommon_errorcode;
        // std::vector<SICK::Point3D> mvec_oringin_data;
        // //不能只有一个vector，因为取数据和处理数据并不一定是线性的
        // std::queue<std::vector<DeviceSingleData>> mque_vecoringin_data;
        // //线性队列，主要是为了做缓存区

        std::vector<SICK::Point3D> mvec_one_origin_point;
        std::vector<TIS_Info::DeviceSingleData> mvec_one_point;

        std::thread m_thread_getData;
        std::atomic_bool mb_getData;
        QTimer *m_timer_getData;

        unsigned int m_echoFlag;

      private:
        void GetDataThread();

      private slots:
        void OnDataTimerTimeout();
    };
} // namespace _Modules

#endif