#pragma once
#include "kits/common/module_base/ModuleBase.h"
#include <qmetaobject.h>
#include "service/AppFramework.h"
#include <utility>

#define __DECLARE_SERVICE_TYPES(class_name, func_name)                                                                                     \
    struct _##class_name##func_name                                                                                                        \
    {                                                                                                                                      \
        inline static const std::string _name = #func_name;                                                                                \
                                                                                                                                           \
        _##class_name##func_name(_Kits::ModuleBase *mod, QMetaMethod met) : _module(mod), _method(met)                                     \
        {                                                                                                                                  \
        }                                                                                                                                  \
                                                                                                                                           \
        template <typename... Args>                                                                                                        \
        bool _touchAsync(Args &&...args)                                                                                                   \
        {                                                                                                                                  \
            return _module->invokeAsync(_method, std::forward<Args>(args)...);                                                             \
        }                                                                                                                                  \
        template <typename Ret, typename... Args>                                                                                          \
        Ret _touchSync(Args &&...args)                                                                                                     \
        {                                                                                                                                  \
            return _module->invokeSync<Ret>(_method, std::forward<Args>(args)...);                                                         \
        }                                                                                                                                  \
                                                                                                                                           \
      private:                                                                                                                             \
        QMetaMethod _method;                                                                                                               \
        _Kits::ModuleBase *_module = nullptr;                                                                                              \
    };

#define __DECLARE_SERVICE_INVOKE_API_A(types)                                                                                              \
    template <typename... Args>                                                                                                            \
    bool invoke##types##_A(Args &&...args)                                                                                                 \
    {                                                                                                                                      \
        auto *invoke = _InnerApp().getInvokes<_Details::_##types>();                                                                       \
        if (invoke)                                                                                                                        \
        {                                                                                                                                  \
            return invoke->_touchAsync(std::forward<Args>(args)...);                                                                       \
        }                                                                                                                                  \
        return false;                                                                                                                      \
    }

#define __DECLARE_SERVICE_INVOKE_API_S(types)                                                                                              \
    template <typename Ret, typename... Args>                                                                                              \
    Ret invoke##types##_S(Args &&...args)                                                                                                  \
    {                                                                                                                                      \
        auto *invoke = _InnerApp().getInvokes<_Details::_##types>();                                                                       \
        if (invoke)                                                                                                                        \
        {                                                                                                                                  \
            return invoke->_touchSync<Ret>(std::forward<Args>(args)...);                                                                   \
        }                                                                                                                                  \
        return Ret{};                                                                                                                      \
    }

#define __DECLARE_SERVICE_NOTIFY_APIS(type_name, func_name)                                                                                \
    template <typename... Args>                                                                                                            \
    void invoke##func_name(Args &&...args)                                                                                                 \
    {                                                                                                                                      \
        auto &notifys = _InnerApp().getNotifys<_Details::_##type_name>();                                                                  \
        for (auto &it : notifys)                                                                                                           \
        {                                                                                                                                  \
            it._touchAsync(std::forward<Args>(args)...);                                                                                   \
        }                                                                                                                                  \
    }

#define ___DECLARE_SERVICE_HELPER_INNER_(type)                                                                                             \
    if (mod->getMetaMethod(_Details::_##type::_name).isValid())                                                                            \
    {                                                                                                                                      \
        _InnerApp().invokeRegister<_Details::_##type>(mod, mod->getMetaMethod(_Details::_##type::_name));                                  \
    }

namespace _Service
{
    namespace _Details
    {
        using namespace _AppFramework;
        __DECLARE_SERVICE_TYPES(, notifyTask)
        inline void notifyHelper(_Kits::ModuleBase *mod)
        {
            if (mod->getMetaMethod(_Details::_notifyTask::_name).isValid())
            {
                _InnerApp().notifyRegister<_Details::_notifyTask>(mod, mod->getMetaMethod(_Details::_notifyTask::_name));
            }
        }
    } // namespace _Details
} // namespace _Service

namespace _Service
{
    using namespace _AppFramework;
    __DECLARE_SERVICE_NOTIFY_APIS(notifyTask, NotifyTask)

} // namespace _Service
