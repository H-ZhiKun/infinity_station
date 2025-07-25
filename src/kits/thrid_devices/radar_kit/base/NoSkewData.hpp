#ifndef _NO_SKEW_DATA_HPP_
#define _NO_SKEW_DATA_HPP_

#include "SkewDeviceData.hpp"

namespace _Modules{
    class NoSkewData : public SkewDeviceData
    {
    public:
        
        public:
        NoSkewData() = default;
        void OperateSkewData(std::vector<DeviceSingleData> &device_singledata, float &angle_degrees) override final
        {
            //pass
        }
        NoSkewData(const NoSkewData&) = delete;
        virtual ~NoSkewData() = default;
    };
}

#endif