#pragma once
#include "tis_global/Struct.h"
#include "service/AppInterface.h"
#include "tis_global/EnumClass.h"
#include "kits/common/factory/ControllerRegister.h"
#include "kits/common/factory/FunctionWapper.h"
#include "kits/common/invoke/Invoke_creator.h"
#include <string>
#include <utility>

namespace _Controllers
{

#define TASK_LIST_BEGIN                                                                                                                    \
    static void initRouting()                                                                                                              \
    {
#define SYNC_TASK_ADD(path, func) instance().registerTasks(path, &func, false)
#define ASYNC_TASK_ADD(path, func) instance().registerTasks(path, &func, true)
#define QML_ADD(qmlPath, func) instance().registerQmlActions(qmlPath, &func)
#define REMOTE_TASK_ADD(topic, func) instance().registerRemote(topic, &func)
#define TASK_LIST_END }

    template <typename T, bool AutoCreation = true>
    class ControllerBase
    {
      public:
        static const bool isAutoCreation = AutoCreation;

        virtual ~ControllerBase()
        {
        }

      protected:
        ControllerBase()
        {
        }
        static T &instance()
        {
            static T m_ins_;
            return m_ins_;
        }

        template <typename Func>
        void registerTasks(const std::string &path, Func &&func, bool isAsync)
        {
            auto boundFunc = _Kits::FunctionWapper::wapper(func, static_cast<T *>(this));
            auto task = _Kits::InvokeCreator<decltype(boundFunc)>::make_invoke(path, isAsync, boundFunc);
            _Kits::ControllerRegister::registerTaskRoutes(path, task);
        }

        template <typename Func>
        void registerRemote(const std::string &path, Func &&func)
        {
            auto boundFunc = _Kits::FunctionWapper::wapper(func, static_cast<T *>(this));
            auto task = _Kits::InvokeCreator<decltype(boundFunc)>::make_invoke(path, true, boundFunc);
            _Kits::ControllerRegister::registerRemoteTask(path, task);
        }

        template <typename Func>
        void registerQmlActions(TIS_Info::QmlCommunication::QmlActions qmlPath, Func &&func)
        {
            auto boundFunc = _Kits::FunctionWapper::wapper(func, static_cast<T *>(this));
            _Kits::ControllerRegister::registerQmlPath(static_cast<int>(qmlPath), std::move(boundFunc));
        }

      private:
        class TaskRegistrator
        {
          public:
            TaskRegistrator()
            {
                if (AutoCreation)
                {
                    T::initRouting();
                }
            }
        };

        friend TaskRegistrator;
        static TaskRegistrator registrator_;
        virtual void *touch()
        {
            return &registrator_;
        }

      public:
        static int m_int_;
    };

    template <typename T, bool AutoCreation>
    typename ControllerBase<T, AutoCreation>::TaskRegistrator ControllerBase<T, AutoCreation>::registrator_;
} // namespace _Controllers