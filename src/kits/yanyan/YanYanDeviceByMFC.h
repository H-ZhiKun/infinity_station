#pragma once
#include <yaml-cpp/yaml.h>
#include "kits/common/log/CRossLogger.h"
#include "JCWMes.h"
#include "JCWDataDef.h"
#include <string>
#include <thread>
#include <atomic>
#include <functional>

namespace _Kits
{

    class YanYanDeviceByMFC
    {
    public:
        explicit YanYanDeviceByMFC() = default;
        ~YanYanDeviceByMFC() noexcept;
    public:
        bool init(const std::string& deviceconfig);
        bool start( const void* pTag,Jcw_fnJCWDResultCallBack callback);
        bool stop();
    private:
        void realease();
        
    private:
        void* m_pJiHe = nullptr;
        std::atomic_bool m_bIsStart = false;
        std::atomic_bool m_bIsInit = false;
        std::thread m_thread;


    };
    
} // namespace _Modules
