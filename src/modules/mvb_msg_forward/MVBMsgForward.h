#pragma once

#include "kits/common/module_base/ModuleBase.h"

namespace _Modules
{
    class MVBMsgForward : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(MVBMsgForward)
      public:
        explicit MVBMsgForward(QObject *parent = nullptr);
        virtual ~MVBMsgForward() noexcept;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void mvbRecvDataForward(const QVariantMap &);

      protected slots:
        void sendMvbRecvData(const QVariantMap &);
    };
} // namespace _Modules