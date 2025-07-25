#pragma once

#include "kits/common/module_base/ModuleBase.h"
#include "tis_global/Struct.h"

namespace _Modules
{
    class MVBDataSwitch : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(MVBDataSwitch)
      public:
        explicit MVBDataSwitch(QObject *parent = nullptr);
        virtual ~MVBDataSwitch() noexcept;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void switchConfigData(const TIS_Info::MvbAmifSwitch &, const TIS_Info::MvbAsatSwitch &);

      private:
        YAML::Node m_details;
    };

} // namespace _Modules