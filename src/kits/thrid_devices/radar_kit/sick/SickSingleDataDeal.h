#ifndef _SICK_SINGEL_DATA_DEAL_H_
#define _SICK_SINGEL_DATA_DEAL_H_

#include "SickSingleData.h"
#include <vector>
#include <algorithm>

namespace _Modules {
    // 对多个设备包含的数据进行合并
    class SickSingelDataDeal
    {
    public:
        SickSingelDataDeal();
        ~SickSingelDataDeal();

        static SingleSurfaceData mergeData(std::vector<SICK::Point3D>& vec_oringin_data)
        {
            return deweightData(vec_oringin_data);
        }

    private:
        static SingleSurfaceData deweightData(std::vector<SICK::Point3D>& vec_oringin_data) {
            // 对points进行排序
            std::sort(vec_oringin_data.begin(), vec_oringin_data.end());
            // 使用std::unique去重
            auto last = std::unique(vec_oringin_data.begin(), vec_oringin_data.end());
            // 删除多余的数据
            vec_oringin_data.erase(last, vec_oringin_data.end());

            return SingleSurfaceData(std::move(vec_oringin_data));
        }
    };
}

#endif