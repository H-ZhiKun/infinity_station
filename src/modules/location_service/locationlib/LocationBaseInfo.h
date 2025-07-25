#pragma once

#include <atomic>
class LocationBaseInfo
{

  public:
    double getDistance();
    void setDistance(const double &distance);
    double getSpeed();
    void setSpeed(const double &speed);
    // 间隔时间越短，精度越高
    void UpdateOnce(const double &time);

    void reset();

  private:
    std::atomic<double> m_distance = 0.0; // 距离累计,m
    std::atomic<double> m_speed = 0.0;    // 当前速度,m/s
};