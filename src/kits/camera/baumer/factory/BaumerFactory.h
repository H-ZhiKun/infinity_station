#pragma once
#include "kits/camera/baumer/base/BaumerBase.h"
#include <functional>
#include <map>
#include <memory>

namespace _Kits
{

    // 相机工厂基类

    class BaumerFactory
    {
      private:
        template <typename T>
        static std::unique_ptr<BaumerBase> CreateCamera()
        {
            return std::make_unique<T>();
        }

      public:
        virtual ~BaumerFactory() = default;

        template <typename T>
        static void RegisterCameraCreateFactory(const std::string &type)
        {
            BaumerFactory::s_factories[type] = CreateCamera<T>;
        }

        static std::unique_ptr<BaumerBase> CreateCameraByType(const std::string &type)
        {

            auto it = s_factories.find(type);
            if (it != s_factories.end())
            {
                return (it->second)();
            }
            return nullptr;
        }

        inline static std::map<std::string, std::function<std::unique_ptr<BaumerBase>(void)>> s_factories;
    };

} // namespace _Kits