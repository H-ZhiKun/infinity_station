#pragma once
#include "thread"
#include <json/json.h>
#include <memory>
#include <qtmetamacros.h>

using namespace _Kits;
namespace _Modules
{
    class CollectService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(CollectService)
      public:
        explicit CollectService(QObject *parent = nullptr);
        virtual ~CollectService() noexcept;
        CollectService(const CollectService &) = delete;
        CollectService &operator=(const CollectService &) = delete;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:

      private slots:

      private:
        void run();
        void release();

        bool initYAML(const YAML::Node &);
        bool initDevice();

      private:
        std::atomic<bool> m_runThreadFlag = false;
        std::thread m_runThread;
        int m_sendRate = 1000;
        YAML::Node m_config;
    };
} // namespace _Modules
