
#pragma once
#include <limits>
#include <cstdint>

/***************************************************************************
 * @file    MVBCommonData.h
 * @brief  //模块信息
 *          
 *
 *
 * @note    
 ***************************************************************************/



enum ModuleType :uint16_t {
    ModuleType_PMS_IVideoCam = 0,           //录像模块
    ModuleType_PMS_IGeoParMon = 1,          //几何模块
    ModuleType_PMS_IArcMod = 2,             //燃弧模块
    ModuleType_PMS_IInfMod = 3,             //红外温度模块
    ModuleType_PMS_IStrMod = 4,             //受电弓结构识别模块
    ModuleType_PMS_IComMod = 5,             //补偿模块
    ModuleType_PMS_ITcmsMod = 6,            //Tcms服务模块
    ModuleType_PMS_IPantoMod = 7,           //受电弓模块
    ModuleType_PMS_IYangJiaoMod = 8,        //羊角模块
    ModuleType_PMS_ILocationMod = 9,        //定位模块
    ModuleType_PMS_IACCELERATION = 10,      //加速度模块
    ModuleType_PMS_IPressureMod = 11,       //压力检测模块
    ModuleType_PMS_IAbrasionMod = 12,       //磨耗检测模块
    ModuleType_PMS_IXunjianMod = 13,        //接触网巡检拍照模块
    ModuleType_RetRail_GeoMod = 14,         //回流轨几何参数检测模块
    ModuleType_RetRail_Camera = 15,         //回流轨与回流器视频监控模块
    ModuleType_RetRail_HardPotMod = 16,     //回流轨硬点模块
    ModuleType_ASAT_PMSModuleType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::uint16_t>::max()
};

//报警信息
enum ARMINFO_AlarmType :uint16_t {
  ARMINFO_AlarmType_None = 0,                   //无
  ARMINFO_AlarmType_Temp = 1,                   //温度
  ARMINFO_AlarmType_CatOff = 2,                 //拉出值
  ARMINFO_AlarmType_CatHei = 3,                 //导高
  ARMINFO_AlarmType_Arc = 4,                    //燃弧
  ARMINFO_AlarmType_Str = 5,                    //结构类型
  ARMINFO_AlarmType_Acc = 6,                    //加速度
  ARMINFO_AlarmType_YangJiao = 7,               //羊角         
  ARMINFO_AlarmType_TanHuaBan = 8,              //碳滑板
  ARMINFO_AlarmType_GongTou = 9,                //弓头
  ARMINFO_AlarmType_ContactForc = 10,           //接触力
  ARMINFO_AlarmType_RetRail_Hspace = 11,        //回流轨水平间距
  ARMINFO_AlarmType_RetRail_Vspace = 12,        //回流轨垂直高度
  ARMINFO_AlarmType_RetRail_HardPot = 13,       //回流轨硬点
  ARMINFO_AlarmType_DiversionWire = 14,         //导流线
  // AMIF_AlarmType_AMIF_AlarmType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ARMINFO_AlarmType_AMIF_AlarmType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<uint16_t>::max()
};

const int C_MODULE_ALIVE_TIME = 20;            //模块状态值

inline static bool isBigEndian()	//	
{
	 uint16_t usForTest = 0x1234;

	return ((uint8_t*)(&usForTest))[0] == 0x12;
}
