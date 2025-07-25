#include "TemperatureService.h"
#include "kits/common/log/CRossLogger.h"
#include <filesystem>
#include <json/value.h>
#include <qobject.h>

// using namespace _Modules;

using namespace _Kits;
namespace _Modules
{

    TemperatureService::TemperatureService(QObject *parent)
    {
    }

    TemperatureService::~TemperatureService() noexcept
    {
    }

    bool TemperatureService::init(const YAML::Node &config)
    {
        if (!config["config_path"])
        {
            return false;
        }
        std::string fullPath = std::filesystem::current_path().string() + config["config_path"].as<std::string>();
        try
        {
            m_details = YAML::LoadFile(fullPath);
            m_fullPath = fullPath;
        }
        catch (const YAML::BadFile &e)
        {
            LogError("load file path error {}", fullPath);
            return false;
        }
        return true;
    }

    bool TemperatureService::start()
    {
        if (m_startFlag.load())
        {
            return false;
        }
        QVariantMap map;
        map["configpath"] = QByteArray::fromStdString(m_fullPath);
        emit initSignal(QVariant::fromValue(map));
        if (nullptr == m_pCamera)
        {
            m_pCamera = std::make_unique<HikTemperatureCamera>();
        }
        connect(m_pCamera.get(), &HikTemperatureCamera::IRTempRectSignal, this, &TemperatureService::IRTempRect);

        m_startFlag.store(true);
        m_connectCameraThread = std::thread([this]()
            {
                while (m_startFlag.load())
                {
                    int ret = m_pCamera->openCamera(m_details);
                    if (0 != ret)
                    {
                        LogError("open camera error:{}", ret);
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                    else
                    {
                        m_pCamera->startGrab();
                        break;
                    }
                }
            }
        );

        connect(m_pCamera.get(), &HikTemperatureCamera::IRimageReadySignal, this, &TemperatureService::IRImageReady);

        return true;
    }

    bool TemperatureService::stop()
    {
        emit stopSignal();
        m_startFlag.store(false);
        if(m_connectCameraThread.joinable())
        {
            m_connectCameraThread.join();
        }
        if (nullptr != m_pCamera)
        {
            m_pCamera->stopGrab();
            m_pCamera->closeCamera();
        }
      
        return true;
    }

} // namespace _Modules