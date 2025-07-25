#ifndef _GET_ALL_DEVICE_DATA_BASE_H_
#define _GET_ALL_DEVICE_DATA_BASE_H_

#include "DeviceData.hpp"
#include "SkewDeviceData.hpp"
#include "ChangeCenterBase.h"
#include "tis_global/Struct.h"

namespace _Modules
{

    class GetAllDeviceDataBase
    {
      public:
        GetAllDeviceDataBase() = default;
        virtual _Kits::DeviceSurfaceData GetAllDeviceData()
        {
            return _Kits::DeviceSurfaceData();
        }
        virtual ~GetAllDeviceDataBase() = default;
        virtual void LoadAllDevice()
        {
        }
        virtual void ReloadAllDevice()
        {
        }
        _Kits::Err GetLastErr()
        {
            return m_err_code;
        }

      protected:
        _Kits::Err m_err_code;
        _Modules::SkewDeviceData *m_skew_data;
        _Modules::ChangeCenterBase *m_change_center;

        // 改变中心坐标系
      protected:
        inline void ChangeCenterPoint(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                      const _Kits::ChangeCenterData &change_center_data)
        {
            m_change_center->ChangeCenterPoint(device_singledata, change_center_data);
        }

        inline void SkewDeviceData(std::vector<TIS_Info::DeviceSingleData> &device_singledata, float angle_degrees)
        {
            m_skew_data->OperateSkewData(device_singledata, angle_degrees);
        }
    };

} // namespace _Modules

#endif