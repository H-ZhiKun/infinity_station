#include "MVBPmsDataGenerator.h"
#include "MVBCommonData.h"



MVBPmsDataGenerator::MVBPmsDataGenerator(std::unique_ptr<MVBDataGenerator> mvbDataGenerator)
:m_dataGeneratorPtr(std::move(mvbDataGenerator))
{

}

MVBPmsDataGenerator::~MVBPmsDataGenerator()
{
    m_armInfosetlist.clear();
}

void MVBPmsDataGenerator::initSetAlarmFunc()
{
    regitsetArmFuncTemp(0,&MVBDataGenerator::setTempAlarm);
    regitsetArmFuncTemp(1,&MVBDataGenerator::setCatOffALarm);
    regitsetArmFuncTemp(2,&MVBDataGenerator::setCatHeiAlarm);
    regitsetArmFuncTemp(3,&MVBDataGenerator::setArcAlarm);
    regitsetArmFuncTemp(4,&MVBDataGenerator::setCatHeiAlarm);
    regitsetArmFuncTemp(5,&MVBDataGenerator::setStrAlarm);
    regitsetArmFuncTemp(6,&MVBDataGenerator::setAccAlarm);
    regitsetArmFuncTemp(7,&MVBDataGenerator::setYangJiaoAlarm);
    regitsetArmFuncTemp(8,&MVBDataGenerator::setTanHuaBanAlarm);
    regitsetArmFuncTemp(9,&MVBDataGenerator::setGongTouAlarm);
    regitsetArmFuncTemp(10,&MVBDataGenerator::setContactForcAlarm);
    regitsetArmFuncTemp(11,&MVBDataGenerator::setRetRailHspaceAlarm);
    regitsetArmFuncTemp(12,&MVBDataGenerator::setRetRailVspaceAlarm);
    regitsetArmFuncTemp(13,&MVBDataGenerator::setRetRailHardPotAlarm);
    regitsetArmFuncTemp(14,&MVBDataGenerator::setDiversionWireAlarm);
}

void MVBPmsDataGenerator::initSetModulStatusFunc()
{
    regitsetModulStatusFuncTemp(0,&MVBDataGenerator::setPMSCameraFault);
    regitsetModulStatusFuncTemp(1,&MVBDataGenerator::setPMSGeoAbrModFault);
    regitsetModulStatusFuncTemp(2,&MVBDataGenerator::setPMSArcModFault);
    regitsetModulStatusFuncTemp(3,&MVBDataGenerator::setPMSTmpModFault);
    // regitsetModulStatusFuncTemp(4,nullptr);
    regitsetModulStatusFuncTemp(5,&MVBDataGenerator::setPMSComModFault);
    regitsetModulStatusFuncTemp(6,&MVBDataGenerator::setPMSCommuModFault);
    // regitsetModulStatusFuncTemp(7,nullptr);
    // regitsetModulStatusFuncTemp(8,nullptr);
    // regitsetModulStatusFuncTemp(9,nullptr);
    regitsetModulStatusFuncTemp(10,&MVBDataGenerator::setRetRailHardPotModFault);
    regitsetModulStatusFuncTemp(11,&MVBDataGenerator::setPMSPressureModFault);
    regitsetModulStatusFuncTemp(12,&MVBDataGenerator::setPMSGeoAbrModFault);
    regitsetModulStatusFuncTemp(13,&MVBDataGenerator::setPMSXunjianModFault);
    regitsetModulStatusFuncTemp(14,&MVBDataGenerator::setRetRailGeoModFault);
    regitsetModulStatusFuncTemp(15,&MVBDataGenerator::setRetRailCameraFault);
    regitsetModulStatusFuncTemp(16,&MVBDataGenerator::setRetRailHardPotModFault);

}

void MVBPmsDataGenerator::initModuleStatus()
{
    std::lock_guard<std::mutex> lock(m_moduleStatusMtx);
    m_modulStatus.insert(std::pair<uint16_t,int>(0,C_MODULE_ALIVE_TIME));
}

void MVBPmsDataGenerator::setAlarmInfo(int armno,int iAlarmLevel)
{
    auto iter = m_armInfosetlist.find(armno);
    if (iter == m_armInfosetlist.end())
    {
       return;
    }
    
    iter->second(iAlarmLevel);
    
}

void MVBPmsDataGenerator::setModulstatusInfo(int modulno,int value)
{
     auto iter = m_modulStatussetlist.find(modulno);
    if (iter == m_modulStatussetlist.end())
    {
       return;
    }
    
    iter->second(value);
}

template <typename Func>
void MVBPmsDataGenerator::regitsetArmFuncTemp(int armno,Func&& func)
{
    if constexpr (std::is_member_function_pointer<Func>::value)
    {

        auto setAlarmFunc = std::bind(func,this->m_dataGeneratorPtr.get(),std::placeholders::_1);
 
        registerAlarmFunc(armno,setAlarmFunc);
    }
}

template <typename Func>
void MVBPmsDataGenerator::regitsetModulStatusFuncTemp(int modulno,Func&& func)
{
    if constexpr (std::is_member_function_pointer<Func>::value)
    {
           auto setModulStatusFunc = std::bind(func,this->m_dataGeneratorPtr.get(),std::placeholders::_1);
 
        registerModulStatusFunc(modulno,setModulStatusFunc);
    }
}

void MVBPmsDataGenerator::registerModulStatusFunc(int modulno,std::function<void(int)> &&func)
{
    m_modulStatussetlist.insert(std::pair<uint16_t,std::function<void(int)>>(modulno,func));
}

void MVBPmsDataGenerator::registerAlarmFunc(int armno ,std::function<void(int)>&& func)
{
     m_armInfosetlist.insert(std::pair<uint16_t,std::function<void(int)>>(armno,func));
}

std::string MVBPmsDataGenerator::getPmsData() 
{
     
    if (nullptr != m_dataGeneratorPtr)
    {
        // QVariantMap mapData = configdata.toMap();
        m_dataGeneratorPtr->setPMSAlive(m_AliveNum);
        // int comid = mapData.value("comid",42150).toInt();
        // m_dataGeneratorPtr->setPMSComID(comid); //根据配置来
        m_dataGeneratorPtr->initCommonInfo();
        m_AliveNum++;
        return m_dataGeneratorPtr->getPmsData();
    }
    m_AliveNum++;
    return "";
}

void MVBPmsDataGenerator::updateModulesStaus(int moduleno)
{
    std::lock_guard<std::mutex> lock(m_moduleStatusMtx);
    m_modulStatus[moduleno] = C_MODULE_ALIVE_TIME;
}

void MVBPmsDataGenerator::decreaseModulesAliveTime()
{
    std::lock_guard<std::mutex> lock(m_moduleStatusMtx);
    for (auto & it: m_modulStatus)
    {
        if (it.second-- < 0)
        {
            setModulstatusInfo(it.first,true);
        }      
    }   
}

void MVBPmsDataGenerator::setSleepCmd(bool value) 
{
     if (nullptr != m_dataGeneratorPtr)
    {
        m_dataGeneratorPtr->setSleepCmd(value);
    }
}

void MVBPmsDataGenerator::initConfigInfo(const std::string & filePath)
{
    if (nullptr != m_dataGeneratorPtr)
    {
        m_dataGeneratorPtr->initConfigInfo(filePath);
    }
    
}