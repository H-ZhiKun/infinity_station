#pragma once
#include "kits/camera/basler/base/BaslerBase.h"
#include <functional>
#include <map>
#include <memory>

namespace _Kits
{

    // 相机工厂基类

    class BaslerFactory
    {
      private:
        template <typename T>
        static std::unique_ptr<BaslerBase> CreateCamera()
        {
            return std::make_unique<T>();
        }

      public:
        virtual ~BaslerFactory() = default;

        template <typename T>
        static void RegisterCameraCreateFactory(const std::string &type)
        {
            BaslerFactory::s_factories[type] = CreateCamera<T>;
        }

        static std::unique_ptr<BaslerBase> CreateCameraByType(const std::string &type)
        {

            auto it = s_factories.find(type);
            if (it != s_factories.end())
            {
                return (it->second)();
            }
            return nullptr;
        }

        inline static std::map<std::string, std::function<std::unique_ptr<BaslerBase>(void)>> s_factories;
    };

} // namespace _Kits