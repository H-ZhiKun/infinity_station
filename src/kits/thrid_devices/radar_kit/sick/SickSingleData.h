#ifndef _SICK_SINGEL_DATA_H_
#define _SICK_SINGEL_DATA_H_

#include "sick_radar/SICKLidarSensorData.h"
#include <vector>


namespace _Modules
{
    // 断面数据
    struct SingleSurfaceData
    {
        std::vector<SICK::Point3D> mvec_deweight_surfacepoints;
        // std::vector<SICK::Point3D> mvec_origin_tooalpoints;

        SingleSurfaceData() = default;

        SingleSurfaceData(const SingleSurfaceData &p) : mvec_deweight_surfacepoints(p.mvec_deweight_surfacepoints)
        {
        }
        SingleSurfaceData(SingleSurfaceData &&p) noexcept : mvec_deweight_surfacepoints(std::move(p.mvec_deweight_surfacepoints))
        {
        }

        SingleSurfaceData(std::vector<SICK::Point3D> &&p) noexcept : mvec_deweight_surfacepoints(std::move(p))
        {
        }

        //        SingleSurfaceData(const SingleSurfaceData& p)
        //            : mvec_deweight_surfacepoints(p.mvec_deweight_surfacepoints),
        //              mvec_origin_tooalpoints(p.mvec_origin_tooalpoints) {}
        //        SingleSurfaceData(SingleSurfaceData&& p) noexcept
        //            : mvec_deweight_surfacepoints(std::move(p.mvec_deweight_surfacepoints)),
        //              mvec_origin_tooalpoints(std::move(p.mvec_origin_tooalpoints)) {}
    };

    // 超限数据
    struct SingleSurfaceOverData
    {
      public:
        static inline std::vector<SICK::Point3D> mvec_oneoverlevel_surfacepoints = {};
        static inline std::vector<SICK::Point3D> mvec_twooverlevel_surfacepoints = {};
        static inline std::vector<SICK::Point3D> mvec_threeoverlevel_surfacepoints = {};
    };

} // namespace _Modules

#endif