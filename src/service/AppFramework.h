#pragma once
#include <atomic>
#include <memory>
#include <vector>
namespace _Service
{
    namespace _AppFramework
    {

        class AppFramework
        {

          public:
            virtual ~AppFramework() noexcept = default;
            /**
             * @brief 获取应用程序框架的唯一实例
             * @details
             * 该函数返回应用程序框架的唯一实例，是单例模式的实现。该函数不接收任何参数。
             * @return
             * 返回AppFramework类型的引用，表示应用程序框架的唯一实例。
             */
            [[nodiscard]] static AppFramework &instance() noexcept;
            virtual int run(int argc, char *argv[]) = 0;
            virtual void stop() noexcept = 0;
            /**
             * @brief 通知所有模块
             *
             * 该函数用于向所有已注册的模块发送通知。每个模块需要实现相应的槽函数来处理该通知。
             *
             * 示例：
             * 如果通知类型为 NotifyType::Task，则子类应当实现：

             *
             * @param type 通知的类型
             * @param args 传递的参数数据
             */
            template <typename NotifyClass, typename ModuleBase, typename Method>
            void notifyRegister(ModuleBase *module, Method met)
            {
                m_notifys<NotifyClass>.push_back(std::move(NotifyClass(module, met)));
            }

            template <typename NotifyClass>
            std::vector<NotifyClass> &getNotifys()
            {
                return m_notifys<NotifyClass>;
            }

            template <typename InvokeClass, typename ModuleBase, typename Method>
            void invokeRegister(ModuleBase *module, Method met)
            {
                m_invokes<InvokeClass> = std::make_unique<InvokeClass>(module, met);
            }

            template <typename InvokeClass>
            InvokeClass *getInvokes()
            {
                if (!m_bRunning || !m_invokes<InvokeClass>)
                {
                    return nullptr;
                }
                return m_invokes<InvokeClass>.get();
            }

          protected:
            AppFramework() = default;
            AppFramework(const AppFramework &) noexcept = delete;
            AppFramework &operator=(const AppFramework &) = delete;
            AppFramework(AppFramework &&) = delete;
            AppFramework &operator=(AppFramework &&) = delete;
            template <typename NotifyClass>
            inline static std::vector<NotifyClass> m_notifys;

            template <typename InvokeClass>
            inline static std::unique_ptr<InvokeClass> m_invokes;
            std::atomic_bool m_bRunning = {false};
        };

        [[nodiscard]] inline AppFramework &_InnerApp() noexcept
        {
            return AppFramework::instance();
        }
    } // namespace _AppFramework
} // namespace _Service
