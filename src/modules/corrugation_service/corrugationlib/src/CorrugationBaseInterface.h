#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <yaml-cpp/yaml.h>

struct CorrugationData
{

    uint8_t F_data[50];//前端传感器
    uint8_t M_data[50];//中间传感器
    uint8_t B_data[50];//后端传感器
};

class CorrugationBaseInterface
{
  public:
    CorrugationBaseInterface() = default;
    virtual ~CorrugationBaseInterface() {};
    /**********************
     * @brief 初始化Corrugation
     * @param config  配置文件路径
     * @return 0 表示成功，非零表示失败
     ************************ */
    virtual int init(const YAML::Node &config) = 0;
    /**********************
     * @brief 开始采集，任务开始
     * @param
     * @return 0 表示成功，非零表示失败
     ************************ */
    virtual int start() = 0;

    /**********************
     * @brief 停止采集，任务停止
     * @param
     * @return
     ************************ */
    virtual void stop() = 0;
    /**********************
     * @brief 获取数据
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
};