#pragma once
#include "InvokeBase.h"
#include "kits/common/module_base/ModuleBase.h"
#include "kits/communication/remote/RemoteSubscriber.h"
#include "kits/communication/remote/RemotePublisher.h"
#include "tis_global/Struct.h"

#include <memory>
#include <qmetaobject.h>
#include <unordered_map>
namespace _Modules
{

    class RemoteAccess : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(RemoteAccess)

      public:
        explicit RemoteAccess(QObject *parent = nullptr);
        virtual ~RemoteAccess() noexcept;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;
        virtual bool customization(const std::unordered_map<_Kits::RegisterKey, std::list<std::any>> &lvTasks) override;

      private slots:
        void publish(const TIS_Info::RemoteMessage &);

      private:
        std::unique_ptr<_Kits::RemotePublisher> m_publisher = nullptr;
        std::unique_ptr<_Kits::RemoteSubscriber> m_subscriber = nullptr;
        YAML::Node m_config;
        std::unordered_map<std::string, std::shared_ptr<_Kits::InvokeBase>> m_mapTopicMethod;
    };

} // namespace _Modules
