#pragma once
#include <atomic>
#include <json/json.h>
#include <memory>
#include <qtmetamacros.h>
#include "kits/camera/hik/temperature/HikTemperatureCamera.h"
#include "kits/common/module_base/ModuleBase.h"
#include <thread>
/***************************************************************************
 * @file    TemperatureService.h
 * @brief
 *
 *
 *
 * @note
 ***************************************************************************/
// using namespace _Kits;
namespace _Modules
{
    class TemperatureService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(TemperatureService)
      public:
        explicit TemperatureService(QObject *parent = nullptr);
        virtual ~TemperatureService() noexcept;
        TemperatureService(const TemperatureService &) = delete;
        TemperatureService &operator=(const TemperatureService &) = delete;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void IRImageReady(std::shared_ptr<TIS_Info::IRImageBuffer> image);
        void IRTempRect(const int startx, const int starty, const int endx, const int endy, const int w, const int h);
        void initSignal(const QVariant &);
        void stopSignal();
        void displayLogSignal(const QString &log);

      private:
        std::atomic<bool> m_startFlag = false; // 启动标志
        std::unique_ptr<_Kits::HikTemperatureCamera> m_pCamera = nullptr;
        YAML::Node m_details;
        std::string m_fullPath = "";
        std::thread m_connectCameraThread;
    };
} // namespace _Modules
