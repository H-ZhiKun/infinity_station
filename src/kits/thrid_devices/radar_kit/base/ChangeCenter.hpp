#ifndef _CHANGE_CENTER_HPP_
#define _CHANGE_CENTER_HPP_

#include "ChangeCenterBase.h"

namespace _Modules
{

    class ChangeCenter : public _Modules::ChangeCenterBase
    {

      public:
        ChangeCenter() = default;

        virtual ~ChangeCenter() = default;

        virtual void ChangeCenterPoint(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                       const _Kits::ChangeCenterData &change_center_data) override final
        {
            for (auto &point : device_singledata)
            {

                // 使用 QVector2D 的正确方法访问和修改坐标
                point.mf_x = (point.mf_x - change_center_data.mf_change_x);
                point.mf_y = (point.mf_y - change_center_data.mf_change_y);
            }
        }
    };

} // namespace _Modules

#endif