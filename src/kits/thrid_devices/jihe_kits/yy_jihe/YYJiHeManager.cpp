#include "YYJiHeManager.h"
#include "YYJiHe.h"
#include <chrono>
#include <memory>
#include <qdebug>
#include <qobject.h>
#include <qtimer.h>
#include <thread>
namespace _Kits
{
    YYJiHeManager::YYJiHeManager(QObject *parent) : JiHeManagerBase(parent)
    {
    }
    YYJiHeManager::~YYJiHeManager()
    {
        if (m_thInit_.joinable())
            m_thInit_.join();
    }

    bool YYJiHeManager::start(const std::string &configFile)
    {
        m_thInit_ = std::thread([this] {
            while (true)
            {
                if(initSystem())
                    break;
                std::this_thread::sleep_for(std::chrono::seconds(100));
            }

            while (true)
            {
                //auto imgBuffer = m_pJHBufferPools->getObject();
                if (m_jihe->Refresh(m_jiheData))
                {
                    emit JiHeManagerBase::sendJiHeData(m_jiheData);
                }
            }
        });

        return true;
    }

    bool YYJiHeManager::stop()
    {
        return true;
    }
    bool YYJiHeManager::initSystem()
    {
        try
        {
            if (!m_jihe)
            {
                m_jihe = std::make_unique<_Kits::YYJiHe>();
                bool ret=m_jihe->Open(m_strJcwPath);
                return ret;
                /* code */
            }
            
        }
        catch (...)
        {
            qDebug() << "Error catch" ;
        }
        return true;
    }
} // namespace _Kits