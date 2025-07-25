#ifndef _GET_ALL_SICK_DEVICE_DATA_HPP_
#define _GET_ALL_SICK_DEVICE_DATA_HPP_

#include "kits/thrid_devices/radar_kit/base/DeviceData.hpp"
#include "kits/thrid_devices/radar_kit/base/GetAllDeviceDataBase.h"
#include "sick_radar/SICKLidarSensor.h"
#include "sick_radar/SICKLidarSensorData.h"

#include <vector>

namespace _Modules
{

    class GetAllSickDeviceData : public GetAllDeviceDataBase
    {
      public:
        GetAllSickDeviceData() = default;
        ~GetAllSickDeviceData() = default;

        void LoadAllDevice() override final;

        void ReloadAllDevice() override final;

        _Kits::DeviceSurfaceData GetAllDeviceData() override final;

      private:
        std::vector<SICK::SICKLidarSensor> mvec_sick_device;
        std::vector<std::vector<SICK::Point3D>> mvec_singleProfile;
        std::vector<std::vector<TIS_Info::DeviceSingleData>> mvec_oringin_data;

      private:
        void GetSingleDeviceData(_Kits::DeviceSurfaceData &device_surface_data,
                                 SICK::SICKLidarSensor &sick_device,
                                 std::vector<SICK::Point3D> &single_data,
                                 const _Kits::ChangeCenterData &change_center_data);

        void ChangeToNormalData(_Kits::DeviceSurfaceData &device_surface_data,
                                const std::vector<SICK::Point3D> &vec_sick_data,
                                const _Kits::ChangeCenterData &change_center_data);

        void SkewDeviceData(std::vector<TIS_Info::DeviceSingleData> &device_singledata, float angle_degrees);
    };

} // namespace _Modules

#endif