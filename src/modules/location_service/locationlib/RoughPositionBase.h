#pragma once
#include "PositionBaseInterface.h"
#include <string>

/*
RoughPositionBase 初始的位置服务类
*/

class RoughPositionBase : public PositionBaseInterface
{
  public:
    RoughPositionBase();
    virtual ~RoughPositionBase();

    virtual int init(const YAML::Node &config) override;

    virtual int start() override;

    virtual int stop() override;

    virtual TIS_Info::PositionData getPositionData(double dis, double speed) override;

    virtual TIS_Info::PositionData getPositionData(int id) override;

    virtual void setTaskInfo(TIS_Info::TaskInfo taskInfo) override;

    virtual bool isEffectPT(int *) override;

  private:
    bool selectLineData(double dis, TIS_Info::PositionData &curPositionInfo);
    bool isSpan(const QString &poleName);
    bool selectLineData(int id, TIS_Info::PositionData &posInfo);

  private:
    TIS_Info::TaskInfo m_taskInfo;
    TIS_Info::PositionData m_curPositionInfo;
};