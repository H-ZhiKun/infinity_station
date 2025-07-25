#include "MVBNBDataGenerator.h"
#include <bitset>
#include <iostream>
#include "../MVBCommonData.h"
#include <yaml-cpp/yaml.h>
#include <filesystem>

MVBNBDataGenerator::MVBNBDataGenerator(/* args */)
{
    m_data.reserve(72);
    m_data.resize(72,0);
    m_isBigEndian = isBigEndian();
}

MVBNBDataGenerator::~MVBNBDataGenerator()
{

}

void MVBNBDataGenerator::setBitSetValue(int offset,int beginbit,int endbit,int modbit,bool value)
{

    std::lock_guard<std::mutex> lock(m_dataMtx);
    if (offset > m_data.size() ||
       (beginbit < 0 && beginbit > 7) || 
       (endbit < 0 && endbit > 7) || 
       (modbit < 0 && modbit > 7))
    {
        return;
    }
    
    std::bitset<8> setModset = m_data.at(offset);
    for (size_t i = beginbit; i < endbit; i++)
    {
       setModset.set(modbit,false);
    }
    setModset.set(modbit,value);
    uint8_t sarm =static_cast<uint8_t> (setModset.to_ulong());
    m_data.at(offset) = sarm;
}

std::string MVBNBDataGenerator::getPmsData()
{
    std::lock_guard<std::mutex> lock(m_dataMtx);
    std::string strdata(m_data.begin(), m_data.end());
    return strdata;
}

template <typename T>
void MVBNBDataGenerator::setValue(int offset,int len, T value,bool isBigEndian)
{
    int sizeT = sizeof(value);
    std::lock_guard<std::mutex> lock(m_dataMtx);
    if (offset > m_data.size() ||
     ((offset + len) > m_data.size()) ||
     (len < sizeT) )
    {
        return;
    }
    if (isBigEndian)
    {
        for (int i = 0; i < sizeT; i++)
        {
           m_data[offset+i] = (value >> (i*8)) & 0xFF;
        }     
    }
    else
    {
        for (int i = 0; i < sizeT; i++)
        {
           m_data[offset+sizeT -i -1] = (value >> (i*8)) & 0xFF;
        }   
    }
    
}

 uint32_t MVBNBDataGenerator::calcFCS32(int len,uint32_t fcs)
 {
     std::lock_guard<std::mutex> lock(m_dataMtx);
     if (len > m_data.size())
     {
       return -1;
     }
     for (int i = 0; i < len; i++)
     {
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ m_data.at(i)) & 0xff];
     }
     
     return (fcs ^ 0xffffffff);
     
 }

void MVBNBDataGenerator::setTempAlarm(int iAlarmLevel)
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(44,0,2,0,true);
        break;
    case 2:
        setBitSetValue(44,0,2,1,true);
        break;
    case 3:
        setBitSetValue(44,0,2,2,true);
        break;
    default:
       setBitSetValue(44,0,2,0,false);
        break;
    }

}

void MVBNBDataGenerator::setCatOffALarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(46,0,2,0,true);
        break;
    case 2:
        setBitSetValue(46,0,2,1,true);
        break;
    case 3:
        setBitSetValue(46,0,2,2,true);
        break;
    default:
       setBitSetValue(46,0,2,0,false);
        break;
    }
}

void MVBNBDataGenerator::setCatHeiAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(47,0,2,9,true);
        break;
    case 2:
        setBitSetValue(47,0,2,1,true);
        break;
    case 3:
        setBitSetValue(47,0,2,2,true);
        break;
    default:
        setBitSetValue(47,0,2,0,false);
        break;
    }
}

void MVBNBDataGenerator::setArcAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(43,0,2,0,true);
        break;
    case 2:
        setBitSetValue(43,0,2,1,true);
        break;
    case 3:
        setBitSetValue(43,0,2,2,true);
        break;
    default:
        setBitSetValue(43,0,2,0,false);
        break;
    }

}

void MVBNBDataGenerator::setStrAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(42,0,2,0,true);
        break;
    case 2:
        setBitSetValue(42,0,2,1,true);
        break;
    case 3:
        setBitSetValue(42,0,2,2,true);
        break;
    default:
        setBitSetValue(42,0,2,0,false);
        break;
    }
}

void MVBNBDataGenerator::setAccAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(45,0,2,0,true);
        break;
    case 2:
        setBitSetValue(45,0,2,1,true);
        break;
    case 3:
        setBitSetValue(45,0,2,2,true);
        break;
    default:
        setBitSetValue(45,0,2,0,false);
        break;
    }
}

void MVBNBDataGenerator::setYangJiaoAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(42,4,6,4,true);
        break;
    case 2:
        setBitSetValue(42,4,6,5,true);
        break;
    case 3:
        setBitSetValue(42,4,6,6,true);
        break;
    default:
        setBitSetValue(42,4,6,4,false);
        break;
    }
}

void MVBNBDataGenerator::setTanHuaBanAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(43,4,6,4,true);
        break;
    case 2:
        setBitSetValue(43,4,6,5,true);
        break;
    case 3:
        setBitSetValue(43,4,6,6,true);
        break;
    default:
        setBitSetValue(43,4,6,4,false);
        break;
    }
 
}

void MVBNBDataGenerator::setGongTouAlarm(int iAlarmLevel) 
{

}

void MVBNBDataGenerator::setContactForcAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(45,4,6,4,true);
        break;
    case 2:
        setBitSetValue(45,4,6,5,true);
        break;
    case 3:
        setBitSetValue(45,4,6,6,true);
        break;
    default:
        setBitSetValue(45,4,6,4,false);
        break;
    }
}

void MVBNBDataGenerator::setRetRailHspaceAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(46,4,6,4,true);
        break;
    case 2:
       setBitSetValue(46,4,6,5,true);
        break;
    case 3:
        setBitSetValue(46,4,6,6,true);
        break;
    default:
        setBitSetValue(46,4,6,4,false);
        break;
    }
}

void MVBNBDataGenerator::setRetRailVspaceAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(47,4,6,4,true);
        break;
    case 2:
        setBitSetValue(47,4,6,5,true);
        break;
    case 3:
        setBitSetValue(47,4,6,6,true);
        break;
    default:
        setBitSetValue(47,4,6,4,false);
        break;
    }
}

void MVBNBDataGenerator::setRetRailHardPotAlarm(int iAlarmLevel)
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(48,0,2,0,true);
        break;
    case 2:
        setBitSetValue(48,0,2,1,true);
        break;
    case 3:
        setBitSetValue(48,0,2,2,true);
        break;
    default:
        setBitSetValue(48,0,2,0,false);
        break;
    }
}

void MVBNBDataGenerator::setDiversionWireAlarm(int iAlarmLevel)
{

}

void MVBNBDataGenerator::setPMSCameraFault(bool value)
{
    setBitSetValue(50,1,1,1,value);
}   

void MVBNBDataGenerator::setPMSGeoAbrModFault(bool value)
{
    setBitSetValue(50,5,5,5,value);
}   

void MVBNBDataGenerator::setPMSArcModFault(bool value)
{
    setBitSetValue(50,2,2,2,value);
}

void MVBNBDataGenerator::setPMSTmpModFault(bool value)
{
    setBitSetValue(50,0,0,0,value);
}   

void MVBNBDataGenerator::setPMSIPanModStatus(bool value)
{

}   

void MVBNBDataGenerator::setPMSComModFault(bool value)
{
    setBitSetValue(50,6,6,6,value);
}   

void MVBNBDataGenerator::setPMSCommuModFault(bool value)
{
    setBitSetValue(50,7,7,7,value);
}   

void MVBNBDataGenerator::setPMSHardPotModFault(bool value)
{
    setBitSetValue(50,3,3,3,value);
}   

void MVBNBDataGenerator::setPMSPressureModFault(bool value)
{
    setBitSetValue(50,4,4,4,value);
} 

void MVBNBDataGenerator::setPMSXunjianModFault(bool value)
{
    setBitSetValue(50,1,1,1,value);
}   

void MVBNBDataGenerator::setRetRailGeoModFault(bool value)
{
    setBitSetValue(50,2,2,2,value);
}   

void MVBNBDataGenerator::setRetRailCameraFault(bool value)
{
    setBitSetValue(51,3,3,3,value);
}   

void MVBNBDataGenerator::setRetRailHardPotModFault(bool value)
{
    setBitSetValue(51,4,4,4,value);
} 

void MVBNBDataGenerator::setNvrDskFault(bool value) 
{
     setBitSetValue(51,0,0,0,value);
}

void MVBNBDataGenerator::setPMSChecking(bool value) 
{
    setBitSetValue(60,7,7,7,value);
}   

void MVBNBDataGenerator::setPMSCheckComplete(bool value)
{
     setBitSetValue(60,5,5,5,value);
}

void MVBNBDataGenerator::setPMSCheckRes(bool value)
{
     setBitSetValue(60,4,4,4,value);
}

void MVBNBDataGenerator::setSleepCmd(bool value)
{
    setBitSetValue(60,3,3,3,value);
}

void MVBNBDataGenerator::setPMSAlive(uint16_t alive)
{

    setValue(0,4,(int)alive,m_isBigEndian);
    setValue(40,2,alive,m_isBigEndian);
}

void MVBNBDataGenerator::setPMSComID(int comid)
{
    setValue(8,4,comid,m_isBigEndian);
}

void MVBNBDataGenerator::initCommonInfo()
{
    setValue(4,2,uint16_t(m_softVersion),m_isBigEndian);       //version
    setValue(6,2,uint16_t(m_msgType),m_isBigEndian);       //msgtype
    setValue(20,4,32,m_isBigEndian);       //msgtype
    auto fcsresult = calcFCS32(36, 0xFFFFFFFF);
    setValue(36,4,fcsresult,true);       //set fcs
    setValue(70,2,uint16_t(m_pmsVersion),m_isBigEndian);    //setpms  version
    setPMSComID(m_comid);
    
}

void MVBNBDataGenerator::initConfigInfo(const std::string& configpath)
{
    if (configpath.empty()) {
        //std::cerr << "No configuration path found." << std::endl;
        return ;
    }
    YAML::Node config_node = YAML::LoadFile(std::filesystem::current_path().string() + configpath);
    m_comid = config_node["comid"].as<int>();
    m_softVersion = config_node["softversion"].as<uint16_t>();
    m_msgType = config_node["msgtype"].as<uint16_t>();
    m_pmsVersion = config_node["pmsversion"].as<uint16_t>();

}
