// #include "kits/camera_service/BaumerCameraManager.h"
#include "CameraBumerManager.h"

namespace _Kits
{
CameraBumerManager::CameraBumerManager()
{
}

CameraBumerManager::~CameraBumerManager()
{
}
void CameraBumerManager::init(const YAML::Node &config)
{
    // 读取config
    // if (config["usbBumer"])
    // {

    //     // 相机初始化
    //     initializeBGAPI();
    //     searchUSBCamera();
    // }
    if (config["GEVBumer"])
    {
        nodeParams_ = config["GEVBumer"]["camera"];
        initializeBGAPI();
        searchGEVCamera();
    }
}
void CameraBumerManager::initializeBGAPI()
{
    try
    {
        BGAPI2::SystemList *system_list = BGAPI2::SystemList::GetInstance();
        system_list->Refresh();
        for (BGAPI2::SystemList::iterator sys_iter = system_list->begin();
             sys_iter != system_list->end();
             sys_iter++)
        {
            BGAPI2::System *system_pointer = sys_iter->second;
            BGAPI2::String tl_type = system_pointer->GetTLType();
            if (tl_type != "GEV")
            {
                continue;
            }
            system_pointer->Open();
            pSystem_ = system_pointer;
            BGAPI2::InterfaceList *interface_list =
                system_pointer->GetInterfaces();
            interface_list->Refresh(100);
            for (BGAPI2::InterfaceList::iterator ifc_iter =
                     interface_list->begin();
                 ifc_iter != interface_list->end();
                 ifc_iter++)
            {
                ifc_iter->second->Open();
                std::string name = ifc_iter->second->GetDisplayName().get();
                std::string ip =
                    ifc_iter->second->GetNode("GevInterfaceSubnetIPAddress")
                        ->GetValue()
                        .get();
                std::string mac =
                    ifc_iter->second->GetNode("GevInterfaceMACAddress")
                        ->GetValue()
                        .get();
                std::string mask =
                    ifc_iter->second->GetNode("GevInterfaceSubnetMask")
                        ->GetValue()
                        .get();
                // 为相机对应数据流注册掉线触发事件
                if (mac != interfaceMAC_)
                {
                    qDebug() << "interface mac not matched." << mac;
                    ifc_iter->second->Close();
                    continue;
                }
                else if (ip != interfaceIp_)
                {
                    qDebug() << "interface mac not matched." << ip;
                    ifc_iter->second->Close();
                    continue;
                }
                ifc_iter->second->RegisterPnPEvent(
                    BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
                // ifc_iter->second->RegisterPnPEventHandler(
                //     this, (Events::PnPEventHandler)&PnPEventHandler);
                u32InterfaceIp_ =
                    ifc_iter->second->GetNode("GevInterfaceSubnetIPAddress")
                        ->GetInt();
                u32InterfaceMask_ =
                    ifc_iter->second->GetNode("GevInterfaceSubnetMask")
                        ->GetInt();
                qDebug() << "interface founded: ";
                qDebug() << "founded ip: {}" << ip;
                qDebug() << "founded MAC: {}" << mac;
                qDebug() << "founded mask: {}" << mask;
            }
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type:" << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
}
void CameraBumerManager::searchGEVCamera()
{
    try
    {
        BGAPI2::InterfaceList *interface_list = pSystem_->GetInterfaces();
        interface_list->Refresh(100);
        for (BGAPI2::InterfaceList::iterator ifc_iter = interface_list->begin();
             ifc_iter != interface_list->end();
             ifc_iter++)
        {
            ifc_iter->second->Open();
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type: " << ex.GetType().get();
        qDebug() << "Error function: " << ex.GetFunctionName().get();
        qDebug() << "Error description: " << ex.GetErrorDescription().get();
    }
}

} // namespace _Kits