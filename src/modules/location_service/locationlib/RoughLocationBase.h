#pragma once

#include "LocationBaseInterface.h"
#include "LocationBaseInfo.h"
#include <thread>
#include <atomic>

/*
RoughLocationBase 公司老版本的定位方式，粗糙而暴力
*/

class RoughLocationBase : public LocationBaseInterface
{

  public:
    RoughLocationBase();
    virtual ~RoughLocationBase();

    virtual int init(const YAML::Node &config) override;

    virtual int start() override;

    virtual void stop() override;

    virtual void adjustData(double dis, double speed) override;

    virtual void updataData(double &dis, double &speed) override;

    virtual double getSpeed() override;

    virtual double getDistance() override;

    virtual double getMoveDis() override;

    virtual void setMoveDis(double) override;
    void release();

  private:
    void run();

  private:
    LocationBaseInfo m_locationBaseInfo; // 位置信息
    std::atomic<bool> m_runThreadFlag = false;
    std::atomic<double> m_moveDis = 0.0; // km
    std::thread m_runThread;
    int m_refreshRate = 100; // 刷新频率
};