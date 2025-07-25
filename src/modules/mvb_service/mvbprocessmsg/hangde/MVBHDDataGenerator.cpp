#include "MVBHDDataGenerator.h"
#include <bitset>
#include <iostream>
#include "../MVBCommonData.h"
#include <chrono>
#include <yaml-cpp/yaml.h>
#include <filesystem>

MVBHDDataGenerator::MVBHDDataGenerator(/* args */)
{
    m_data.reserve(46);
    m_data.resize(46,0);
    m_isBigEndian = isBigEndian();
}

MVBHDDataGenerator::~MVBHDDataGenerator()
{

}

void MVBHDDataGenerator::setBitSetValue(int offset,int beginbit,int endbit,int modbit,bool value)
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

std::string MVBHDDataGenerator::getPmsData()
{
    uint8_t* char_ptr = reinterpret_cast<uint8_t*>(m_data.data());
    uint16_t rescrc =  Modbus_CRC16(char_ptr,44);
    setValue(44,2,rescrc,m_isBigEndian);
    std::lock_guard<std::mutex> lock(m_dataMtx);
    std::string strdata(m_data.begin(), m_data.end());
    return strdata;
   // return "";
}

template <typename T>
void MVBHDDataGenerator::setValue(int offset,int len, T value,bool isBigEndian)
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

void MVBHDDataGenerator::setTempAlarm(int iAlarmLevel)
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(32,6,7,6,true);
        setBitSetValue(33,0,0,0,false);
        break;
    case 2:
        setBitSetValue(32,6,7,7,true);
        setBitSetValue(33,0,0,0,false);
        break;
    case 3:
        setBitSetValue(32,6,7,6,false);
        setBitSetValue(33,0,0,0,true);
        break;
    default:
        setBitSetValue(32,6,7,6,false);
        setBitSetValue(33,0,0,0,false);
        break;
    }
}

void MVBHDDataGenerator::setCatOffALarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(32,0,2,0,true);
        break;
    case 2:
        setBitSetValue(32,0,2,1,true);
        break;
    case 3:
        setBitSetValue(32,0,2,2,true);
        break;
    default:
        setBitSetValue(32,0,2,2,false);
        break;
    }

}

void MVBHDDataGenerator::setCatHeiAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(32,3,5,3,true);
        break;
    case 2:
        setBitSetValue(32,3,5,4,true);
        break;
    case 3:
        setBitSetValue(32,3,5,5,true);
        break;
    default:
        setBitSetValue(32,3,5,5,false);
        break;
    }

}

void MVBHDDataGenerator::setArcAlarm(int iAlarmLevel) 
{
    switch (iAlarmLevel) {
    case 1:
        setBitSetValue(33,1,3,1,true);
        break;
    case 2:
        setBitSetValue(33,1,3,2,true);
        break;
    case 3:
        setBitSetValue(33,1,3,3,true);
        break;
    default:
        setBitSetValue(33,1,3,1,false);
        break;
    }
}

void MVBHDDataGenerator::setPMSGeoAbrModFault(bool value)
{
    setBitSetValue(33,4,4,4,value);
}   

void MVBHDDataGenerator::setPMSArcModFault(bool value)
{
    setBitSetValue(33,6,6,6,value);
}

void MVBHDDataGenerator::setPMSTmpModFault(bool value)
{
    setBitSetValue(33,5,5,5,value);
}   


void MVBHDDataGenerator::setSleepCmd(bool value)
{
    setBitSetValue(34,0,0,0,value);
}

void MVBHDDataGenerator::setPMSAlive(uint16_t alive)
{

    setValue(4,4,(int)alive,m_isBigEndian);
    setValue(24,2,(uint16_t)alive,m_isBigEndian);
    
}

void MVBHDDataGenerator::setPMSComID(int comid)
{
    setValue(12,4,comid,m_isBigEndian);  
}

void MVBHDDataGenerator::initsystemtime()
{
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    // 转换为 time_t 类型（C 语言风格的时间）
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&now_c);
    // 获取年、月、日
    int year  = localTime->tm_year + 1900;  // tm_year 是从 1900 年算起的

    setValue(26,1,static_cast<uint8_t>(year % 100),m_isBigEndian);  
    setValue(27,1,static_cast<uint8_t>(localTime->tm_mon + 1),m_isBigEndian); 
    setValue(28,1,static_cast<uint8_t>(localTime->tm_mday),m_isBigEndian); 
    setValue(29,1,static_cast<uint8_t>(localTime->tm_hour),m_isBigEndian); 
    setValue(30,1,static_cast<uint8_t>(localTime->tm_min),m_isBigEndian); 
    setValue(31,1,static_cast<uint8_t>(localTime->tm_sec),m_isBigEndian); 
}
void MVBHDDataGenerator::initCommonInfo()
{
    setValue(0,2,uint16_t(0x6968),true);    
    setValue(2,1,uint8_t(0xEB),true);       //version
    setValue(3,1,uint8_t(0x04),true);    
    
    setPMSComID(m_comid);
    setValue(16,1,static_cast<uint8_t>(m_unitnum),m_isBigEndian);  
    setValue(17,1,static_cast<uint8_t>(m_carnum),m_isBigEndian);  

    setValue(22,2,(int16_t)20,m_isBigEndian);   //数据长度

    initsystemtime();

    //setBitSetValue(32,1,1,7,true);
    //setBitSetValue(33,1,1,7,true);
    
    //setBitSetValue(34,1,1,0,true);
    setValue(36,2,(int16_t)(m_pmsVersion),m_isBigEndian);  
}


void  MVBHDDataGenerator::initConfigInfo(const std::string& configpath)
{
    if (configpath.empty()) {
        //std::cerr << "No configuration path found." << std::endl;
        return ;
    }
    YAML::Node config_node = YAML::LoadFile(std::filesystem::current_path().string() + configpath);
    m_comid = config_node["comid"].as<int>();
    // m_unitnum = config_node["unitnum"].as<int>();
    // m_carnum = config_node["carnum"].as<int>();
    // m_pmsVersion = config_node["pmsversion"].as<uint16_t>();

}
