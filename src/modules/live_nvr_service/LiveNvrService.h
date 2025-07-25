#pragma once

#include "kits/common/factory/ModuleRegister.h"
#include "kits/common/module_base/ModuleBase.h"
#include "kits/thrid_devices/live_nvr/LiveNvr.h"
#include "tis_global/Struct.h"
#include <memory>

namespace _Modules
{
    class LiveNvrService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(LiveNvrService)

      public:
        LiveNvrService();
        virtual ~LiveNvrService() noexcept override; // 显式声明noexcept
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      private:
        std::unique_ptr<_Kits::LiveNvr> m_pLiveNvr;

      private slots:
        void onEndRecord(const QVariant &);

      signals:
        void sendChanel(int data);         // 发送通道号到其他模块
        void sendNvrDataInfo(TIS_Info::NvrDataInfo data);
        void sendEndRecordState(const QVariant & data);
        void sendNvrDataInfoToController(const QVector<TIS_Info::NvrDataInfo>& data);
    };
} // namespace _Modules