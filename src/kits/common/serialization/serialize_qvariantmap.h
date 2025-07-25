#pragma once
#include "reflection_extension.h"
#include <QVariant>
#include <QVariantMap>
#include <type_traits>

namespace _Kits
{

    namespace serialize_qvariantmap
    {

        template <class T>
        struct special_traits
        {
            static constexpr bool value = false;
        };

        template <typename T, std::enable_if_t<!_Kits::has_member<T>() && !special_traits<T>::value, int> = 0>
        QVariant objToVariant(const T &value)
        {
            return QVariant::fromValue(value);
        }

        template <typename T, std::enable_if_t<special_traits<T>::value, int> = 0>
        QVariant objToVariant(const T &value)
        {
            return special_traits<T>::objToVariant(value);
        }

        template <typename T, std::enable_if_t<_Kits::has_member<T>(), int> = 0>
        QVariant objToVariant(const T &object)
        {
            QVariantMap map;
            _Kits::foreach_member(object, [&](const char *key, auto &value) { map[QString(key)] = objToVariant(value); });
            return map;
        }

        template <typename T, std::enable_if_t<!_Kits::has_member<T>() && !special_traits<T>::value, int> = 0>
        T variantToObj(const QVariant &var)
        {
            return var.value<T>();
        }

        template <typename T, std::enable_if_t<special_traits<T>::value, int> = 0>
        T variantToObj(const QVariant &var)
        {
            return special_traits<T>::variantToObj(var);
        }

        template <typename T, std::enable_if_t<_Kits::has_member<T>(), int> = 0>
        T variantToObj(const QVariant &var)
        {
            QVariantMap map = var.toMap();
            T object;
            _Kits::foreach_member(object, [&](const char *key, auto &value) {
                using MemberT = std::decay_t<decltype(value)>;
                value = variantToObj<MemberT>(map[QString(key)]);
            });
            return object;
        }
    } // namespace serialize_qvariantmap
    // 调用接口
    template <typename T>
    QVariantMap toVariantMap(const T &object)
    {
        return serialize_qvariantmap::objToVariant(object).toMap();
    }

    template <typename T>
    T fromVariantMap(const QVariantMap &map)
    {
        return serialize_qvariantmap::variantToObj<T>(QVariant(map));
    }

} // namespace _Kits