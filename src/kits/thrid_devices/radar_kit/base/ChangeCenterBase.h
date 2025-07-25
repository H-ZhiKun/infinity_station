#ifndef _CHANGE_CENTER_BASE_H_
#define _CHANGE_CENTER_BASE_H_

#include "DeviceData.hpp"
#include <vector>
#include "tis_global/Struct.h"

namespace _Modules
{

    class ChangeCenterBase
    {
      public:
        ChangeCenterBase() = default;
        virtual ~ChangeCenterBase() = default;

        virtual void ChangeCenterPoint(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                       const _Kits::ChangeCenterData &change_center_data)
        {
            // pass
        }
    };

} // namespace _Modules

#endif