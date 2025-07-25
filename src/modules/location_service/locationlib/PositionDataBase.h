#pragma once
#include <QVariant>
#include "kits/common/database/CppBatis.h"
#include "kits/common/database/orm/TableStructs.h"
#include "tis_global/Struct.h"

class PositionDataBase
{
  private:
    /* data */
  public:
    PositionDataBase(/* args */) {
        // m_lvObj.reserve(10);
    };
    ~PositionDataBase() = default;
    void setTriggerMode();

    bool DataBaseInsert(QVariant);
    bool DataBaseInsert(const TIS_Info::PositionData &);
    bool DataBaseInsert(const std::vector<TIS_Info::PositionData> &);
    bool DataBaseInsert(const std::vector<_Kits::_Orm::location_data> &);
    bool DataBaseInsert(const _Kits::_Orm::location_data &vecData);
    _Kits::_Orm::location_data convertToDBData(const TIS_Info::PositionData &data);
    int DataBaseQuery(std::string stationName, std::string poleName, int direction, double &dis);
    int DataBaseQuery(std::string poleName, int direction, double &dis);
    int DataBaseQuery(QString rfidId, int direction,double &dis);
    int DataBaseQueryMaoDuan(double curDis,int direction,double &dis);
  private:
    float m_preMoveDis = 0.0;
    float m_triggerDis = 2.5;

    std::vector<_Kits::_Orm::location_data> m_InsertData;
    // std::vector<std::unique_ptr<_Kits::location_data>> m_lvObj;
};
