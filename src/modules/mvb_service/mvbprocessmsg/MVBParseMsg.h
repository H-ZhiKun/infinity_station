#pragma once 
#include <QVariant>
#include <string>
#include <memory>

/***************************************************************************
 * @file    MVBParseMsg.h
 * @brief   mvb协议解析类
 *          
 *
 *
 * @note    
 ***************************************************************************/

class MVBParseMsg
{
public:
    MVBParseMsg(/* args */) = default;
    virtual ~MVBParseMsg() = default;

    /**********************
    * @brief 解析数据
    * @param data           收到车辆信息数据
    * @param msgtype        消息类型
    * @param parseData      解析后的数据
    * @return 
    ************************ */
    virtual int parseMsg(const std::string& data,int msgtype,QVariant& parseData ) = 0;
};
