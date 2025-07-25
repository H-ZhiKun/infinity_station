#pragma once

#include <map>
#include <functional>
#include "MVBDataGenerator.h"
#include <memory>
#include <string>
#include <mutex>
#include <qvariant.h>


/***************************************************************************
 * @file    MVBPmsDataGenerator.h
 * @brief   mvb pms数据缺陷组装接口类
 *          
 *
 *
 * @note    
 ***************************************************************************/


class MVBPmsDataGenerator
{
private:
    
public:
    MVBPmsDataGenerator(std::unique_ptr<MVBDataGenerator> mvbDataGenerator);
    ~MVBPmsDataGenerator();
    /**********************
    * @brief 初始化设置报警函数
    * @param 
    * @return 
    ************************ */
    void initSetAlarmFunc();
    /**********************
    * @brief 初始化设置模块状态函数
    * @param 
    * @return 
    ************************ */
    void initSetModulStatusFunc();
    /**********************
    * @brief 初始化模块的状态值
    * @param 
    * @return 
    ************************ */
    void initModuleStatus();
    /**********************
    * @brief 注册设置报警函数
    * @param armno  警告编号   与 ARMINFO_AlarmType 对应
    * @param func   设置报警的函数 必须是MVBDataGenerator类的成员函数
    * @return 
    ************************ */
    template <typename Func>
    void regitsetArmFuncTemp(int armno,Func&& func);
    /**********************
    * @brief 注册设置模块状态函数
    * @param modulno  模块编号 与 ModuleType 对应
    * @param func     设置模块状态函数 必须是MVBDataGenerator类的成员函数
    * @return 
    ************************ */
    template <typename Func>
    void regitsetModulStatusFuncTemp(int modulno,Func&& func);
    /**********************
    * @brief 设置报警
    * @param armno 警告编号
    * @param iAlarmLevel 报警等级 0-初级 1-中，2-高 其他无效
    * @return 
    ************************ */
    void setAlarmInfo(int armno,int iAlarmLevel);
    /**********************
    * @brief 设置模块状态
    * @param modulno 模块编号
    * @param value   状态值 0 离线，1在线
    * @return 
    ************************ */
    void setModulstatusInfo(int modulno,int value);
    /**********************
    * @brief 更新模块状态到初始值
    * @param modulno 模块编号
    * @return 
    ************************ */
    void updateModulesStaus(int moduleno);
    /**********************
    * @brief 更新模块状态值
    * @param
    * @return 
    ************************ */
    void decreaseModulesAliveTime();
    /**********************
    * @brief 设置休眠状态
    * @param value 
    * @return 
    ************************ */
    void setSleepCmd(bool value);
      /**********************
    * @brief 获取发送的pms数据
    * @param  
    * @return std::string 
    ************************ */
    std::string getPmsData();
    /**********************
    * @brief 初始化配置信息
    * @param  filePath  配置路径
    * @return 
    ************************ */
    void initConfigInfo(const std::string & filePath);

private:
    /**********************
    * @brief 注册设置报警函数
    * @param armno  警告编号   与 ARMINFO_AlarmType 对应
    * @param func   设置报警的函数
    * @return 
    ************************ */
    void registerAlarmFunc(int armno,std::function<void(int)> &&func);
    /**********************
    * @brief 注册设置模块状态函数
    * @param modulno  模块编号 与 ModuleType 对应
    * @param func     设置模块状态函数
    * @return 
    ************************ */
    void registerModulStatusFunc(int modulno,std::function<void(int)> &&func);

private:
    std::unordered_map<uint16_t,std::function<void(int)>> m_armInfosetlist;     //设置报警函数与报警编号对应表
    std::unordered_map<uint16_t,std::function<void(int)>> m_modulStatussetlist; //设置模块状态函数与模块编号对应表
    std::unique_ptr<MVBDataGenerator> m_dataGeneratorPtr = nullptr;             //协议组装
    std::mutex m_moduleStatusMtx;                                               //模块状态值表锁
    std::unordered_map<uint16_t,int> m_modulStatus;                             //模块状态值表
    uint16_t m_AliveNum = 1;                                                    //发送心跳次数
};


