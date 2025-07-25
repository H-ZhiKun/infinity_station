#pragma once
#include <functional>
#include <iostream>
#include <optional>
#include <yaml-cpp/yaml.h>

class LocationBaseInterface
{
  public:
    LocationBaseInterface() = default;
    virtual ~LocationBaseInterface() {};
    /**********************
     * @brief 初始化Location
     * @param config  配置文件路径
     * @return 0 表示成功，非零表示失败
     ************************ */
    virtual int init(const YAML::Node &config) = 0;
    /**********************
     * @brief 开始定位，任务开始
     * @param
     * @return 0 表示成功，非零表示失败
     ************************ */
    virtual int start() = 0;

    /**********************
     * @brief 停止定位，任务停止
     * @param
     * @return
     ************************ */
    virtual void stop() = 0;
    /**********************
     * @brief 校正数据
     * @param
     * @return
     ************************ */
    virtual void adjustData(double dis, double speed) = 0;
        /**********************
     * @brief 发送数据
     * @param
     * @return
     ************************ */
    virtual void updataData(double& dis, double& speed) = 0;

    virtual double getSpeed()=0;

    virtual double getDistance()=0;

    virtual void  setMoveDis(double )=0;

    virtual double getMoveDis()=0;


};