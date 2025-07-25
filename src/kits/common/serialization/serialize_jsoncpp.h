#pragma once
#include "reflection_extension.h"
#include <json/json.h>
#include <memory>
#include <sstream>
#include <string>

namespace _Kits
{

    namespace serialize_jsoncpp
    {

        template <class T>
        struct special_traits
        {
            static constexpr bool value = false;
        };

        template <class T, std::enable_if_t<!_Kits::has_member<T>() && !special_traits<T>::value, int> = 0>
        Json::Value objToJson(T const &object)
        {
            return Json::Value(object);
        }

        template <class T, std::enable_if_t<!_Kits::has_member<T>() && special_traits<T>::value, int> = 0>
        Json::Value objToJson(T const &object)
        {
            return special_traits<T>::objToJson(object);
        }

        template <class T, std::enable_if_t<_Kits::has_member<T>(), int> = 0>
        Json::Value objToJson(T const &object)
        {
            Json::Value root;
            _Kits::foreach_member(object, [&](const char *key, auto &value) { root[key] = objToJson(value); });
            return root;
        }

        template <class T, std::enable_if_t<!_Kits::has_member<T>() && !special_traits<T>::value, int> = 0>
        T jsonToObj(Json::Value const &root)
        {
            return root.as<T>();
        }

        template <class T, std::enable_if_t<!_Kits::has_member<T>() && special_traits<T>::value, int> = 0>
        T jsonToObj(Json::Value const &root)
        {
            return special_traits<T>::jsonToObj(root);
        }

        template <class T, std::enable_if_t<_Kits::has_member<T>(), int> = 0>
        T jsonToObj(Json::Value const &root)
        {
            T object;
            _Kits::foreach_member(object, [&](const char *key, auto &value) { value = jsonToObj<std::decay_t<decltype(value)>>(root[key]); });
            return object;
        }

    } // namespace serialize_jsoncpp
    inline std::string jsonToString(Json::Value root)
    {
        Json::StreamWriterBuilder builder;
        builder["indentation"] = ""; // 不缩进，输出紧凑格式
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        std::ostringstream os;
        writer->write(root, &os);
        return os.str();
    }

    inline Json::Value stringToJson(std::string const &json)
    {
        Json::Value root;
        Json::Reader reader;
        reader.parse(json, root);
        return root;
    }
    template <class T>
    Json::Value toJson(const T &object)
    {
        return serialize_jsoncpp::objToJson(object);
    }

    template <class T>
    T fromJson(const Json::Value &json)
    {
        return serialize_jsoncpp::jsonToObj<T>(json);
    }
} // namespace _Kits