#ifndef _RADAR_OVERRUN_H_
#define _RADAR_OVERRUN_H_

#include <QVector2D>
#include <vector>
#include "kits/thrid_devices/radar_kit/base/DeviceData.hpp"
#include "RadarRecord.h"
#include "tis_global/Struct.h"

namespace _Modules
{
    class RadarOverrun
    {
      public:
        struct PointInfo
        {
            float x;
            float y;
        };

        RadarOverrun()
        {
        }

        virtual ~RadarOverrun()
        {
        }

        std::vector<std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>>> isOverrun(std::shared_ptr<_Kits::DeviceSurfaceData> detect_data,
                                                                                      std::vector<TIS_Info::DeviceSingleData> &polygon);

      private:
        virtual bool isPointInPolygon(const TIS_Info::DeviceSingleData &point, const std::vector<TIS_Info::DeviceSingleData> &polygon);

        double calculateOffsetAngle(double x, double y);

        unsigned int getHashIndex();

        void returnIndex(int i);

        void expandHashTable(int newSize = 20);

      private:
        std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>> m_overrun_list_before;

        std::unordered_map<unsigned int, std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>>> m_overrun_hash;

        std::unordered_map<unsigned int, bool> m_overrun_hash_index;

        RadarRecord m_radar_record_sql;

        std::mutex m_hash_mutex;
    };
} // namespace _Modules

#endif