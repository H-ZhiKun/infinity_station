#pragma once
#include <string>
#include <qvariant.h>


/***************************************************************************
 * @file    MVBDataGenerator.h
 * @brief   mvb pms 协议组装类
 *          
 *
 *
 * @note    
 ***************************************************************************/


class MVBDataGenerator
{
private:
    /* data */
public:
    MVBDataGenerator(/* args */) = default;
    virtual ~MVBDataGenerator() = default;

public:
    virtual void setTempAlarm(int iAlarmLevel) = 0;             //  设置温度警告
    virtual void setCatOffALarm(int iAlarmLevel) = 0;           //  设置拉出值警告 
    virtual void setCatHeiAlarm(int iAlarmLevel) = 0;           //  设置导高警告
    virtual void setArcAlarm(int iAlarmLevel) = 0;              //  设置燃弧警告
    virtual void setStrAlarm(int iAlarmLevel) = 0;              //  设置结构类型警告   
    virtual void setAccAlarm(int iAlarmLevel) = 0;              //  设置加速度警告
    virtual void setYangJiaoAlarm(int iAlarmLevel) = 0;         //  设置羊角警告
    virtual void setTanHuaBanAlarm(int iAlarmLevel) = 0;        //  设置碳滑板警告
    virtual void setGongTouAlarm(int iAlarmLevel) = 0;          //  设置弓头警告
    virtual void setContactForcAlarm(int iAlarmLevel) = 0;      //  设置接触力警告
    virtual void setRetRailHspaceAlarm(int iAlarmLevel) = 0;    //  设置回流轨水平间距警告
    virtual void setRetRailVspaceAlarm(int iAlarmLevel) = 0;    //  设置回流轨垂直高度警告
    virtual void setRetRailHardPotAlarm(int iAlarmLevel) = 0;   //  设置回流轨硬点警告
    virtual void setDiversionWireAlarm(int iAlarmLevel) = 0;    //  设置导流线警告


    virtual void setPMSCameraFault(bool value) = 0;       //录像模块
    virtual void setPMSGeoAbrModFault(bool value) = 0;    //几何模块
    virtual void setPMSArcModFault(bool value) = 0;       //燃弧模块
    virtual void setPMSTmpModFault(bool value) = 0;       //红外温度模块
    virtual void setPMSIPanModStatus(bool value) = 0;     //受电弓结构识别模块
    virtual void setPMSComModFault(bool value) = 0;       //补偿模块
    virtual void setPMSCommuModFault(bool value) = 0;     //Tcms服务模块
    virtual void setPMSHardPotModFault(bool value) = 0;   //加速度模块
    virtual void setPMSPressureModFault(bool value) = 0;  //压力检测模块
    virtual void setPMSXunjianModFault(bool value) = 0;   //接触网巡检拍照模块
    virtual void setRetRailGeoModFault(bool value) = 0;   //回流轨几何参数检测模块
    virtual void setRetRailCameraFault(bool value) = 0;   //回流轨与回流器视频监控模块
    virtual void setRetRailHardPotModFault(bool value) =0; //回流轨硬点模块
    virtual void setNvrDskFault(bool value) = 0;           //NVR硬盘故障  
    virtual void setPMSChecking(bool value) = 0;              //正在自检
    virtual void setPMSCheckComplete(bool value) = 0;         //自检完成
    virtual void setPMSCheckRes(bool value) = 0;            //自测成功
    virtual std::string getPmsData()  = 0;                  //获取pms心跳包数据
    virtual void setPMSAlive(uint16_t alive) = 0;           //设置pms心跳次数
    // virtual void setPMSComID(int comid) = 0 ;               //设置comid
    virtual void initCommonInfo() = 0;                      //初始化公共信息
    virtual void setSleepCmd(bool value) = 0;               //设置是否休眠标志
    virtual void initConfigInfo(const std::string& configpath) = 0;           //设置配置路径
};
