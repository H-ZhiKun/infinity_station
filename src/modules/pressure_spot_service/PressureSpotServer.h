#pragma once
#include "kits/common/module_base/ModuleBase.h"

using namespace _Kits;
namespace _Modules
{
    class PressureSpotServer : public ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(PressureSpotServer)
      public:
        explicit PressureSpotServer(QObject *parent = nullptr);
        virtual ~PressureSpotServer() noexcept;
        PressureSpotServer(const PressureSpotServer &) = delete;
        PressureSpotServer &operator=(const PressureSpotServer &) = delete;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      private:

      signals:
      private slots:
    };
} // namespace _Modules
