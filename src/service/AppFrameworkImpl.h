#pragma once
#include "AppFramework.h"
#include "kits/common/module_base/ModuleBase.h"
namespace _Service
{
    namespace _AppFramework
    {
        class AppFrameworkImpl final : public AppFramework
        {
          public:
            [[nodiscard]] static AppFrameworkImpl &instance() noexcept;
            virtual ~AppFrameworkImpl() noexcept = default;

            virtual int run(int argc, char *argv[]) override;
            virtual void stop() noexcept override;

          protected:
            AppFrameworkImpl() noexcept = default;
            // 子类化接口区

            void enableConsole(const YAML::Node &yaml);
            void cmdCheck();
            void createModules(const YAML::Node &yaml);
            void startModules();
            bool isValidModule(const std::string &name);
            void startMonitor();
            std::unordered_map<std::string,
                               std::unique_ptr<_Kits::ModuleBase>> m_plugins; // 使用类型索引存储模块
        };
    } // namespace _AppFramework
} // namespace _Service