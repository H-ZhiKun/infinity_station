#pragma once 

#include  "../MVBParseMsg.h"
#include <string>

/***************************************************************************
 * @file    MVBHDParseMsg.h
 * @brief  杭德解析数据协议
 *          
 *
 *
 * @note    
 ***************************************************************************/
class MVBHDParseMsg final:public MVBParseMsg
{
private:
    /* data */
public:
     MVBHDParseMsg() = default;
    ~MVBHDParseMsg() = default;
        /**********************
    * @brief 解析数据
    * @param data           收到车辆信息数据
    * @param msgtype        消息类型
    * @param parseData      解析后的数据
    * @return 
    ************************ */
    int parseMsg(const std::string& data,int msgtype,QVariant& parseData ) override;

private:
    /**********************
    * @brief 解析时间数据
    * @param data           收到的mvb数据
    * @param parseData      时间数据
    * @return 
    ************************ */
    void parseMsgTime(const std::string& data,QVariantMap& parseData);
    /**********************
    * @brief 解析车站数据
    * @param data           收到的mvb数据
    * @param parseData      车站数据
    * @return 
    ************************ */
    void parseStationId(const std::string& data,QVariantMap& parseData);
    /**********************
    * @brief 解析心跳包数据
    * @param data           收到的心跳包数据
    * @param parseData      心跳包数据
    * @return 
    ************************ */
    void parsePMSData(const std::string& data,QVariantMap& parseData);
    /**********************
    * @brief 解析车辆信息数据
    * @param data           收到的mvb数据
    * @param parseData      车辆信息数据
    * @return 
    ************************ */
    void parseTarinInfo(const std::string& data,QVariantMap& parseData);
    /**********************
    * @brief 字符串转为uint32_t/uint16_t
    * @param value          字符串
    * @return   uint32_t/uint16_t
    ************************ */
    template<typename T>
    T convertString(const std::string& value);
};

