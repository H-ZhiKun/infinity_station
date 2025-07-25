#include "YanYanDeviceByMFC.h"
#include <chrono>

using namespace _Kits;

YanYanDeviceByMFC::~YanYanDeviceByMFC()
{
    realease();
}
void YanYanDeviceByMFC::realease()
{

    if (nullptr != m_pJiHe)
    {
       Jcw_UninitInstance(m_pJiHe);
    }
    m_bIsInit.store(false);
}

bool YanYanDeviceByMFC::init(const std::string& deviceconfig)
{
    if (m_bIsInit.load())
    {
        LogInfo("YanYanDeviceByMFC::init  already init");
        return true;
    }
    
    m_pJiHe = Jcw_InitInstance();
    if (nullptr == m_pJiHe)
    {
        LogError("Jcw_InitInstance  error");
        return false;
    }

    auto ret = Jcw_LoadConfig(m_pJiHe, deviceconfig.c_str());
    if (JCW_OK != ret)
    {
        LogError("Jcw_LoadConfig  error:{}",ret);
        return false;
    }
    m_bIsInit.store(true);
    return true;
}

bool YanYanDeviceByMFC::start( const void* pTag,Jcw_fnJCWDResultCallBack callback)
{
    if (m_bIsStart.load())
    {
        LogInfo("Jcw_Start  already start");
        return true;
    }
    
    if (nullptr == m_pJiHe || !m_bIsInit.load())
    {
        LogError("m_pJiHe  is nullptr or is not init");
        return false;
    }
    
    auto ret = Jcw_SetJCWDCallBack(m_pJiHe, pTag, JVER_JCWD, callback);
    if (JCW_OK != ret)
    {
        LogError("Jcw_SetJCWDCallBack  error:{}",ret);
        return false;
    }

    ret = Jcw_Start(m_pJiHe);
    if (JCW_OK != ret)
    {
        LogError("Jcw_Start  error:{}",ret);
        return false;
    }
    m_bIsStart.store(true);
    m_thread = std::thread([this](){
        while(m_bIsStart.load())
        {
            if (nullptr != m_pJiHe)
            {
                auto ret = Jcw_SetMovSpeedKMH(m_pJiHe,60);
                if (JCW_OK != ret)
                {
                    LogError("Jcw_SetMovSpeedKMH  error:{}",ret);
                }
                else
                {
                    LogDebug("Jcw_SetMovSpeedKMH  success");
                }
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    return true;
}
bool YanYanDeviceByMFC::stop()
{
    if(nullptr != m_pJiHe || !m_bIsStart.load())
    {
        LogError("m_pJiHe  is nullptr or m_bIsStart is false");
        return false;
    }
    auto ret = Jcw_Stop(m_pJiHe);   
    if (JCW_OK != ret)
	{
        LogError("Jcw_Stop  error:{}",ret);
        return false;
    }
    m_bIsStart.store(false);
    if (m_thread.joinable())
    {
        m_thread.join();
    }
    
    return true;
}
