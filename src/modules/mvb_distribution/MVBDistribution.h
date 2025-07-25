#pragma once
#include "kits/thrid_devices/duagon/duagon_card/DuagonCardMVB.h"
#include "kits/common/module_base/ModuleBase.h"
#include <cstdint>
#include <json/json.h>
#include <memory>
#include <qobject.h>
#include <vector>

namespace _Modules
{
    class MVBDistribution : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(MVBDistribution)
      public:
        explicit MVBDistribution(QObject *parent = nullptr);
        virtual ~MVBDistribution() noexcept;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void duagonData(const int portId, const std::vector<uint8_t> &data);
        void setConfigHeart(const TIS_Info::DuagonCardData &);
      protected slots:
        void dataToDuagonCard(const TIS_Info::DuagonCardData &data);

      private:
        std::unique_ptr<_Kits::DuagonCardMVB> m_ptrDuagonCard = nullptr; // mvb通信接口
        YAML::Node m_details;
    };

} // namespace _Modules
