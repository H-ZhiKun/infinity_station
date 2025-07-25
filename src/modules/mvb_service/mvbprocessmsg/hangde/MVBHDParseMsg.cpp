#include "MVBHDParseMsg.h"
#include <bitset>
#include "../MVBCommonData.h"
#include <QDateTime>
#include <iostream>
#include <sstream>

int MVBHDParseMsg::parseMsg(const std::string& data,int msgtype, QVariant& parseData )
{

    QVariantMap parseDataMap;
  
    //车辆信息
    parseTarinInfo(data,parseDataMap);
    //时间信息
    parseMsgTime(data,parseDataMap);
    //车站信息
    parseStationId(data,parseDataMap);
    //pms信息
    parsePMSData(data,parseDataMap);
    parseData = QVariant::fromValue(parseDataMap);
    return 0;
}

void MVBHDParseMsg::parseMsgTime(const std::string& data,QVariantMap& parseData)
 {
    
    int year = static_cast<int>(data.at(26)) + 2000;          //  "年","");	//	byte[2]	年	0～99	99=2099年  48 1
    int month = static_cast<int>(data.at(27));         //  "月","");	//	byte[3]	月	1～12 49 1
    int day = static_cast<int>(data.at(28));           //  "日","");	//	byte[4]	日	1～31  50 1
    int hour = static_cast<int>(data.at(29));          //  "时","");	//	byte[5]	时	0～23 51 1
    int minute = static_cast<int>(data.at(30));           //  "分","");	//	byte[6]	分	0～59 52 1
    int second = static_cast<int>(data.at(31));        //  "秒","");	//	byte[7]	秒	0～59 53 1

    parseData["year"] = year;
    parseData["month"] = month;
    parseData["day"] = day;
    parseData["hour"] = hour;
    parseData["minute"] = minute;
    parseData["second"] = second;

    tm tm1;
    tm1.tm_year = year - 1900;
    tm1.tm_mon = month - 1;
    tm1.tm_mday = day;
    tm1.tm_hour = hour;
    tm1.tm_min = minute;
    tm1.tm_sec = second;
    tm1.tm_isdst = 0;

    auto curtime = mktime(&tm1);
    parseData["curtime"]= QDateTime::fromSecsSinceEpoch(curtime);
 }

void MVBHDParseMsg::parseStationId(const std::string& data,QVariantMap& parseData)
{
    //起始站
    int16_t startStationId = convertString<int16_t>(data.substr(318,2));
    parseData["startstationid"]= startStationId;
    //下一站
    int16_t nextStationId = convertString<int16_t>(data.substr(320,2));
    parseData["nextstationid"]= nextStationId;
    //终点站
    int16_t endStationId = convertString<int16_t>(data.substr(322,2));
    parseData["endstationid"]= endStationId;
    //下一站距离
    uint32_t nextstationdistance = convertString<int32_t>(data.substr(332,4));
    parseData["nextstationdist"]= nextstationdistance;
    //距当前站距离
    uint32_t curstationdistance = convertString<int32_t>(data.substr(336,4));
    parseData["currentstationdist"]= curstationdistance;
    //当日行驶里程
    uint32_t dayrundistance = convertString<int32_t>(data.substr(340,4));
    parseData["dayrundistance"]= dayrundistance;
    //速度
    int16_t realSpeed = convertString<int16_t>(data.substr(316,2));
    parseData["realspeed"]= realSpeed;


}
void MVBHDParseMsg::parsePMSData(const std::string& data,QVariantMap& parseData)
{
    //TCMS心跳包
    uint16_t tcmsAlive = convertString<uint16_t>(data.substr(24,2));
    parseData["tcmsalive"]= tcmsAlive;
    //列车编号
    int trainset = static_cast<int>(data.at(32)); 
    parseData["trainset"]= trainset;

}

void MVBHDParseMsg::parseTarinInfo(const std::string& data,QVariantMap& parseData)
{
    std::bitset<8> byte288(data.at(312));
    parseData["autosign"] = static_cast<bool>(byte288[0]);      //1=全自动报站
    parseData["panrsign_2"] = static_cast<bool>(byte288[1]);      //2车1=受电弓升弓 
    parseData["panrsign_3"] = static_cast<bool>(byte288[2]);      //3车1=受电弓升弓
    parseData["atbsign"] = static_cast<bool>(byte288[3]);      //1=自动折返
    parseData["cabact_1"] = static_cast<bool>(byte288[4]);      //1车司机室激活              
    parseData["cabact_4"] = static_cast<bool>(byte288[5]);      //4车司机室激活        
    parseData["sleepmode"] = static_cast<bool>(byte288[6]);      //休眠指令
    parseData["updown"] = static_cast<bool>(byte288[7]);      //上下行 1=上行  0=下行

    std::bitset<8> byte289(data.at(313));
    parseData["forward"] = static_cast<bool>(byte289[0]);      //方向向前
    parseData["reverse"] = static_cast<bool>(byte289[1]);      //方向向后
    parseData["hvpres"] = static_cast<bool>(byte289[2]);      //高压存在
    parseData["ivpres"] = static_cast<bool>(byte289[3]);      //中压存在


    int byt290 = static_cast<int>(data.at(314)); 
    parseData["cusstationinfo"] = byt290;      //离站及预到站信息         0=不确定  1=预到站   2=到站   4=预出站   8=出站

    int16_t retvalue = convertString<int16_t>(data.substr(324,2));
    parseData["voltvalue_2"] = retvalue;     //2车网压值
  
     retvalue = convertString<int16_t>(data.substr(326,2));
     parseData["voltvalue_3"] = retvalue;     //3车网压值

    retvalue = convertString<int16_t>(data.substr(328,2));
    parseData["currentvalue_2"] = retvalue;     //2车网流值
    retvalue = convertString<int16_t>(data.substr(330,2));
    parseData["currentvalue_3"] = retvalue;     //3车网流值

}

 template<typename T>
T MVBHDParseMsg::convertString(const std::string& value)
{
    T retvalue = 0x00;
    int size = sizeof(T);
    for (size_t i = 0; i < size; ++i) {
        retvalue |= static_cast<uint8_t>(value.at(i)) << ((size - 1 - i) * 8);
    }
    return retvalue;
}