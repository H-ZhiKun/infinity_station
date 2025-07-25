#include "AppFrameworkImpl.h"
#include "AppModuleHelper.h"
#include "kits/common/eventloop_monitor/EventLoopMonitor.h"
#include "kits/common/event_thread/EventThread.h"
#include "kits/common/factory/ControllerRegister.h"
#include "kits/common/factory/ModuleRegister.h"
#include "kits/common/log/CRossLogger.h"
#include "kits/common/module_base/ModuleBase.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include "kits/common/utils/Utils.h"
#include "kits/common/system_config/SystemConfig.h"
#include "tis_global/Field.h"
#include <QApplication>
#include <QIcon>
#ifdef _WIN32
#include <Windows.h>
#endif
namespace _Service
{
    namespace _AppFramework
    {
        using namespace _Kits;
        AppFramework &AppFramework::instance() noexcept
        {
            return AppFrameworkImpl::instance();
        }
        inline AppFrameworkImpl &AppFrameworkImpl::instance() noexcept
        {
            static AppFrameworkImpl instance;
            return instance;
        }

        int AppFrameworkImpl::run(int argc, char *argv[])
        {

            QApplication qtCore(argc, argv);
            auto &evtThread = _Kits::EventThread::instance();
            qtCore.setWindowIcon(QIcon(":/infinity_station/res/icon/gw.ico")); // 设置标题栏图标

            auto &systemConfig = _Kits::SystemConfig::instance();
            if (!systemConfig.load())
            {
                return 1;
            }
            _Kits::LogInfo("AppFramework Start with {}.", systemConfig.version());
            auto config = systemConfig.mainNode();
            enableConsole(config);
            createModules(config);
            startModules();
            startMonitor();
            Utils::writeDaemon(1);
            // 注册清理逻辑（在事件循环结束前调用）
            QObject::connect(&qtCore, &QCoreApplication::aboutToQuit, [&]() { stop(); });
            m_bRunning = true;
            int ret = qtCore.exec();
            Utils::writeDaemon(0);
            evtThread.stop(); //  停止子事件循环
            _Kits::LogInfo("AppFramework Stop with {}.", systemConfig.version());
            return ret;
        }

        void AppFrameworkImpl::startMonitor()
        {
            static EventLoopMonitor monitor;
            monitor.init();
        }
        void AppFrameworkImpl::enableConsole(const YAML::Node &yaml)
        {
#ifdef _WIN32
            if (yaml["console"])
            {
                AllocConsole();
                freopen("CONOUT$", "w", stdout);
                freopen("CONOUT$", "w", stderr);
            }
#endif
        }

        void AppFrameworkImpl::stop() noexcept
        {
            ConcurrentPool::stop();
            m_bRunning = false;
            for (auto iter = m_plugins.begin(); iter != m_plugins.end();)
            {
                auto moduleName = iter->first;
                const auto &module = iter->second;
                _Kits::EventThread::instance().moveTreeToThread(TIS_Info::EventThread::eventloop_main, module.get());
                module->stop();
                iter = m_plugins.erase(iter);
                _Kits::LogInfo("module destroy success: {}", moduleName);
            }
        }

        void AppFrameworkImpl::createModules(const YAML::Node &yaml)
        {

            for (const auto &item : yaml)
            {
                auto className = item.first.as<std::string>();
                auto block = item.second;
                if (!isValidModule(className))
                {
                    continue;
                }
                auto module = _Kits::ModuleRegister::createModule<_Kits::ModuleBase>(className);
                if (module)
                {
                    if (module->init(block))
                    {
                        module->setCustomName(className);
                        module->tidyMetaMethod();
                        _Details::notifyHelper(module.get());
                        _Details::moduleHelper(module.get());
                        module->setObjectName(className);
                        m_plugins.emplace(className, std::move(module));
                        _Kits::LogInfo("module create success: {}", className);
                    }
                    else
                    {
                        _Kits::LogError("module create failed: {}", className);
                    }
                }
            }
        }

        void AppFrameworkImpl::startModules()
        {
            const std::string separator("::");
            auto &connectMaps = _Kits::ModuleRegister::getGlobalConnections();
            for (const auto &[primaryKey, primaryValues] : connectMaps)
            {
                auto signalPos = primaryKey.find(separator);
                auto signalModuleName = primaryKey.substr(0, signalPos);
                auto signalName = primaryKey.substr(signalPos + 2);
                if (m_plugins.find(signalModuleName) == m_plugins.end())
                    continue;
                auto *signalModule = m_plugins.at(signalModuleName).get();
                for (const auto &slotValue : primaryValues)
                {
                    auto slotPos = slotValue.find(separator);
                    auto slotModuleName = slotValue.substr(0, slotPos);
                    auto slotName = slotValue.substr(slotPos + 2);
                    if (m_plugins.find(slotModuleName) == m_plugins.end())
                        continue;
                    auto *slotModule = m_plugins.at(slotModuleName).get();
                    auto *signalModule = m_plugins.at(signalModuleName).get();
                    ModuleBase::tisMetaConnect(signalModule, signalName, slotModule, slotName);
                }
            }
            for (const auto &[className, module] : m_plugins)
            {
                module->customization(_Kits::ControllerRegister::getKeyRoutes(className));
                module->start();
                _Kits::EventThread::instance().moveTreeToThread(TIS_Info::EventThread::eventloop_module, module.get());
            }
        }

        void AppFrameworkImpl::cmdCheck()
        {
            auto findModule = m_plugins.find("QmlPrivateEngine");
            if (findModule == m_plugins.end())
            {
                LogError("QmlPrivateEngine module not find, in connectQml.");
                ConcurrentPool::runEvery("check input quit", 1000, []() {
                    QTextStream input(stdin);
                    if (input.atEnd())
                    {
                        return; // 如果没有输入，继续等待
                    }
                    QString command = input.readLine().trimmed();
                    if (command == "quit")
                    {
                        qDebug() << "Received quit command. Exiting...";
                        QCoreApplication::quit();
                    }
                });
                return;
            }
        }

        bool AppFrameworkImpl::isValidModule(const std::string &name)
        {
            auto findModule = m_plugins.find(name);
            if (findModule != m_plugins.end())
            {
                return false;
            }
            return true;
        }
    } // namespace _AppFramework
} // namespace _Service