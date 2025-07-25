#include "MVBNBParseMsg.h"
#include <bitset>
#include "../MVBCommonData.h"
#include <QDateTime>

int MVBNBParseMsg::parseMsg(const std::string& data,int msgtype,QVariant& parseData )
{
    QVariantMap parseDataMap;
  
    if (1 == msgtype )
    {
        //车辆信息
        parseTarinInfo(data,parseDataMap);
        //基础信息
        parseBaseCommon(data,parseDataMap);
        //时间信息
        parseMsgTime(data,parseDataMap);
        //车站信息
        parseStationId(data,parseDataMap);
    }
    else 
    {
        parsePMSData(data,parseDataMap);
    }
    
 
    //pms信息
   // parsePMSData(data,parseDataMap);

    parseData = QVariant::fromValue(parseDataMap);
    return 0;
}

void MVBNBParseMsg::parseMsgTime(const std::string& data,QVariantMap& parseData)
 {
    
    int year = static_cast<int>(data.at(8)) + 2000;          //  "年","");	//	byte[8]	年	0～99	99=2099年  48 1
    int month = static_cast<int>(data.at(9));         //  "月","");	//	byte[9]	月	1～12 49 1
    int day = static_cast<int>(data.at(10));           //  "日","");	//	byte[10]	日	1～31  50 1
    int hour = static_cast<int>(data.at(11));          //  "时","");	//	byte[11]	时	0～23 51 1
    int minute = static_cast<int>(data.at(12));           //  "分","");	//	byte[12]	分	0～59 52 1
    int second = static_cast<int>(data.at(13));        //  "秒","");	//	byte[13]	秒	0～59 53 1

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
    parseData["curtime"] = QDateTime::fromSecsSinceEpoch(curtime);

    return ;
 }

void MVBNBParseMsg::parseStationId(const std::string& data,QVariantMap& parseData)
{
    //起始站
    int16_t startStationId = convertString<uint16_t>(data.substr(72,2));  
    parseData["startstationid"]= startStationId;
    //当前站 
    int16_t curStationId = convertString<uint16_t>(data.substr(74,2));
    parseData["curstationid"]= curStationId;
    //下一站
    int16_t nextStationId = convertString<uint16_t>(data.substr(74,2));
    parseData["nextstationid"]= nextStationId;
    //终点站
    int16_t endStationId = convertString<uint16_t>(data.substr(78,2));
    parseData["endstationid"]= endStationId;
    //速度
    int16_t realSpeed = convertString<uint16_t>(data.substr(14,2));
    parseData["realspeed"]= realSpeed;
    //行驶里程
    uint32_t travlledDistance = convertString<uint32_t>(data.substr(82,4));
    parseData["travlleddistance"] = travlledDistance;
}

void MVBNBParseMsg::parsePMSData(const std::string& data,QVariantMap& parseData)
{
        //心跳包***************
    uint16_t pmsAlive = convertString<uint16_t>(data.substr(0,2));
    parseData["pmsalive"]= pmsAlive;

    std::bitset<8> byte2(data.at(42));  //所有门关闭状态
    bool allDoorsClosedFlag = byte2[7];  //	
    bool B1PanUpCmd = byte2[1];  // B2车升弓到位
    bool B2PanUpCmd = byte2[2];  //	B1车升弓到位
    bool B2PanUp = byte2[3];  //	B2车升弓指令
    bool B1PanUp = byte2[4];  //    B1车升弓指令
    bool leftdoor = byte2[5];  //	占有端右侧门关好
    bool rightdoor = byte2[6];  //    占有端左侧门关好

    parseData["alldoorsclosedflag"] = allDoorsClosedFlag;
    parseData["b1panupcmd"] = B1PanUpCmd;
    parseData["b2Panupcmd"] = B2PanUpCmd;
    parseData["b2panup"] = B2PanUp;	
    parseData["b1panup"] = B1PanUp;
}

void MVBNBParseMsg::parseBaseCommon(const std::string& data,QVariantMap& parseData)
{
    //解析时间
    std::bitset<8> byte7(data.at(7));
    bool timeValidFlag = byte7[0];  //	时间有效标志位
    bool setTimeFlag = byte7[1];  //    时间设置标志位
    bool sleepFalg = byte7[4];      // 列车休眠指令

    parseData["timevalidflag"] = timeValidFlag;
    parseData["settimeflag"] = setTimeFlag;
    parseData["sleepfalg"] = sleepFalg;
}

void MVBNBParseMsg::parseTarinInfo(const std::string& data,QVariantMap& parseData)
{
      //车辆数据
    uint16_t alive = convertString<uint16_t>(data.substr(0,2));
    parseData["alive"]= alive;
    uint16_t lineNum = convertString<uint16_t>(data.substr(4,2));
    parseData["linenum"]= lineNum;
    std::bitset<8> byte6(data.at(6));
    bool backwardFlag = byte6[4];  //	列车前向状态 1=前向；0=无效
    bool forwardFlag = byte6[5];  //  列车后向状态  1=后向；0=无效
    bool A2cabActivedFlag = byte6[6];  //	A2车司机室激活
    bool A1cabActivedFlag = byte6[7];  //	A1车司机室激活

    parseData["backwardflag"] = backwardFlag;
    parseData["forwardflag"] = forwardFlag;
    parseData["a2cabActivedflag"] = A2cabActivedFlag;
    parseData["a1cabActivedflag"] = A1cabActivedFlag;

    uint16_t voltage = convertString<uint16_t>(data.substr(53,2));
    parseData["voltage"]= voltage;      //网压

    uint16_t netStream = convertString<uint16_t>(data.substr(55,2));
    parseData["netstream"]= netStream;  //网流
}

 template<typename T>
T MVBNBParseMsg::convertString(const std::string& value)
{
    T retvalue = 0x00;
    int size = sizeof(T);
    for (size_t i = 0; i < size; ++i) {
        retvalue |= static_cast<uint8_t>(value.at(i)) << ((size - 1 - i) * 8);
    }
    return retvalue;
}
