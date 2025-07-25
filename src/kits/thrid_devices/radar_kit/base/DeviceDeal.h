#ifndef _DEVICE_DEAL_H_
#define _DEVICE_DEAL_H_

#include "ChangeCenterBase.h"
#include "DeviceData.hpp"
#include "SkewDeviceData.hpp"
#include <QObject>
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qvectornd.h>
#include <vector>
#include <yaml-cpp/node/node.h>
#include "tis_global/Struct.h"

namespace _Modules
{

    class DeviceDeal : public QObject
    {
        Q_OBJECT

      protected:
        struct DeviceConfig
        {
          public:
            DeviceConfig() = default;
            DeviceConfig(const std::string &str_ip,
                         const std::string &str_name,
                         const unsigned short &us_port,
                         const unsigned short &us_device_index,
                         const unsigned short &us_angle_start,
                         const unsigned short &us_angle_end,
                         const _Kits::ChangeCenterData &change_center_data,
                         const float &skew_angle)
                : mstr_ip(str_ip), mstr_name(str_name), mus_port(us_port), mus_device_index(us_device_index),
                  mus_angle_start(us_angle_start), mus_angle_end(us_angle_end), m_change_center_data(change_center_data),
                  m_skew_angle(skew_angle) {};

            unsigned short mus_port;
            unsigned short mus_device_index;
            unsigned short mus_angle_start;
            unsigned short mus_angle_end;
            std::string mstr_ip;
            std::string mstr_name;
            _Kits::ChangeCenterData m_change_center_data;
            float m_skew_angle;

          private:
            const constexpr static unsigned short _ANGLE_MAX = 360;
            const constexpr static unsigned short _ANGEL_MIN = 0;
        };

      public:
        DeviceDeal(QObject *parent = nullptr) : QObject(parent)
        {
        }

        DeviceDeal(const YAML::Node &);
        virtual ~DeviceDeal() = default;

        // virtual int DeviceNum() = 0;

        virtual bool Init(const YAML::Node &) = 0;

        virtual _Kits::Err LastErrorCode() = 0;

        // 已经完成去重
        virtual _Kits::DeviceSurfaceData GetDeviceData() = 0; // 统一使用DeviceSurfaceData

        // 重新登录设备
        virtual void ReloadDevice() = 0;

        // 改变中心坐标系
        virtual void ChangeCenterPoint(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData,
                                       const _Kits::ChangeCenterData &change_center_data) = 0;

        // 倾斜设备数据
        virtual void SkewDeviceData(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees) = 0;
        // 判断设备是否扶正
        virtual float IsDeviceSkew(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees) = 0;
        // 获取平行于铁轨的 一系列雷达返回坐标数据以height作为限制依据
        virtual std::vector<TIS_Info::DeviceSingleData> GetParallelData(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                                                        float &height) = 0;

        // 补偿数据
        virtual void CompensateData() = 0;

        // 登陆设备
        virtual void LoginDevice() = 0;

        virtual bool GetDeviceState()
        {
            return mb_device_state;
        }

        virtual int GetDeviceIndex()
        {
            return m_device_config.mus_device_index;
        }

        virtual bool StartDetect() = 0;
        virtual bool StopDetect() = 0;

        DeviceConfig &GetDeviceConfig()
        {
            return m_device_config;
        }
      signals:
        void deviceDataGet(unsigned short device_index); // 准备接受数据了，通知外部主动获取，因为sick并没有数据回调

      public slots:

      protected:
        _Kits::Err m_err_code;
        std::unique_ptr<_Modules::SkewDeviceData> m_skew_data;
        std::unique_ptr<_Modules::ChangeCenterBase> m_change_center;

        bool mb_device_state = false;

        DeviceConfig m_device_config;
    };

} // namespace _Modules

#endif