#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <yaml-cpp/yaml.h>
#include <QVariant>
#include <string>
#include "tis_global/Struct.h"

class PositionBaseInterface
{
  public:
    PositionBaseInterface() = default;
    virtual ~PositionBaseInterface() = default;

    /**********************
     * @brief 初始化Position
     * @param config  配置文件路径
     * @return 0 表示成功，非零表示失败
     ************************ */
    virtual int init(const YAML::Node &config) = 0;

    virtual int start() = 0;

    virtual int stop() = 0;

    /**********************
     * @brief 发送position
     * @param dis  里程
     * @return 0 表示成功，非零表示失败
     ************************ */
    virtual TIS_Info::PositionData getPositionData(int id) = 0;

    virtual TIS_Info::PositionData getPositionData(double dis, double speed) = 0;

    virtual void setTaskInfo(TIS_Info::TaskInfo taskInfo) = 0;

    virtual bool isEffectPT(int *) = 0;

  public:
    // stationBuff m_StationBuff;
};