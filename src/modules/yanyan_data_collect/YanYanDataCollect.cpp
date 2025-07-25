#include "YanYanDataCollect.h"
#include "kits/common/log/CRossLogger.h"
#include <filesystem>
#include <json/value.h>
#include <qobject.h>


// using namespace _Modules;

using namespace _Kits;
namespace _Modules
{

    // void YanYanDataCollect::yanyanCallback(const void* puser, JCWDVER ver, const void* data, uint32_t len)
    // {
    //     void* nonConstData = const_cast<void*>(puser);
    //     YanYanDataCollect *pYanYanDataCollect = static_cast<YanYanDataCollect*>(nonConstData);
    //     if (nullptr != pYanYanDataCollect)
    //     {
    //         pYanYanDataCollect->handleCallbackData(ver, data, len);
    //     }
    // }
    YanYanDataCollect::YanYanDataCollect(QObject *parent)
    {
   
    }

    bool YanYanDataCollect::init(const YAML::Node &config)
    {
        if (!config["config_path"])
        {
            return false;
        }
       auto configPath = std::filesystem::current_path().string() + config["config_path"].as<std::string>();

        if (config["save_raw_data"])
        {
            m_saveRawDataFlag = config["save_raw_data"].as<int>();
        }
        m_yanyanDevicePtr = std::make_unique<YanYanDeviceByMFC>();

       //auto ret = m_yanyanDevicePtr->init(fullPath);
       if (!m_yanyanDevicePtr->init(configPath))
       {
            LogError("init yanyan device error:{}",configPath);
            return false;
       }
        // QVariantMap map;
        // map["configpath"] = QByteArray::fromStdString(fullPath);
        // emit initSignal(QVariant::fromValue(map));
        return true;
    }

    bool YanYanDataCollect::start()
    {
        if (m_startFlag.load())
        {
            return false;
        }
        if (nullptr == m_yanyanDevicePtr)
        {
            return false;
        }

        emit initSignal(m_saveRawDataFlag);
        m_callback = [](const void* puser, JCWDVER ver, const void* data, uint32_t len) {
             void* nonConstData = const_cast<void*>(puser);
           YanYanDataCollect *pYanYanDataCollect = static_cast<YanYanDataCollect*>(nonConstData);
            if (nullptr != pYanYanDataCollect)
            {
                pYanYanDataCollect->handleCallbackData(ver, data, len);
            }
        };
        m_yanyanDevicePtr->start(this,m_callback);
        
        m_startFlag.store(true);
        return true;
    }

    bool YanYanDataCollect::stop()
    {
        emit stopSignal();
        if (nullptr != m_yanyanDevicePtr)
        {
           m_yanyanDevicePtr->stop();
        }
        m_startFlag.store(false);
        return true;
    }
    void YanYanDataCollect::handleCallbackData(JCWDVER ver, const void* data, uint32_t len)
    {
        if(JVER_JCWD == ver)
        {
            auto now = std::chrono::steady_clock::now();
            auto duration = now.time_since_epoch();
            auto timestamp =  std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            if (nullptr ==  m_yanyanRawData)
            {
                 m_yanyanRawData = std::make_shared<TIS_Info::RawData>(len+1);
            }
            m_yanyanRawData->timestamp = timestamp;
            const uint8_t* udata = static_cast<const uint8_t*>(data);
            std::copy(udata, udata + len, m_yanyanRawData->data.begin());
            emit rawDataReady(m_yanyanRawData);
        }

    }
} // namespace _Modules