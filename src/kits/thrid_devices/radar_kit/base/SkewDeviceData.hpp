#ifndef _SKEW_DEVICE_DATA_HPP_
#define _SKEW_DEVICE_DATA_HPP_

#include "DeviceData.hpp"
#include <qvectornd.h>
#include "tis_global/Struct.h"

namespace _Modules
{
    //  雷达数据倾斜处理
    class SkewDeviceData
    {
        // protected:
        //     static constexpr double M_PI = 3.141592653;

      public:
        SkewDeviceData() = default;
        virtual ~SkewDeviceData() = default;

        virtual void OperateSkewData(std::vector<TIS_Info::DeviceSingleData> &device_singledata, float &angle_degrees)
        {
        }
        virtual float IsDeviceSkew(std::vector<TIS_Info::DeviceSingleData> &device_singledata, float &angle_radians) = 0;

        virtual std::vector<TIS_Info::DeviceSingleData> GetParallelData(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                                                        float &height) = 0;
    };
} // namespace _Modules

#endif