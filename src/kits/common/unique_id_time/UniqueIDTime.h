#pragma once
#include <string>

/***************************************************************************
 * @file    UniqueIDTime.h
 * @brief   获取系统唯一时间类
 *          
 *
 *
 * @note   
 ***************************************************************************/

class UniqueIDTime
{
public:
    UniqueIDTime(/* args */) = default;
    ~UniqueIDTime() = default;
    /**********************
     * @brief 获取当前时间戳
     * @param   
     * @return 时间戳
     ************************ */
    uint64_t getFileTime();
    /**********************
     * @brief 设置开始时间
     * @param   
     * @return 
     ************************ */
    void startTiming();
    /**********************
     * @brief 获取时间差
     * @param   
     * @return 时间差
     ************************ */
    double getElapseTime();
private:
     /**********************
     * @brief 获取纳秒
     * @param   
     * @return 纳秒
     ************************ */
    uint64_t getNanoSeconds();
private:
    static constexpr uint64_t C_EPOCH_TIME =  11644473600LL;  //柏林时间 1970 01 01
    double m_dTimeStamp = 0;        // 当前时间戳
    uint64_t m_startTime = 0;       // 开始时间
};
