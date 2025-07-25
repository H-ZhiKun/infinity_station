// CameraFactory.h 修改后
#pragma once
#include "kits/camera_service/CameraBase.h"
#include <memory>
#include <map>
#include <functional>

namespace _Kits
{

    template <typename T>
    std::unique_ptr<CameraBase> CreateCamera()
    {
        return std::make_unique<T>();
    }

    class ICameraFactory
    {
      public:
        virtual ~ICameraFactory() = default;

        template <typename T>
        static void RegisterCameraCreateFactory(const std::string &type)
        {
            GetFactoryMap().emplace(type, &CreateCamera<T>);
        }

        static std::unique_ptr<CameraBase> CreateCameraByType(const std::string &type)
        {
            const auto &factories = GetFactoryMap();
            auto it = factories.find(type);
            return (it != factories.end()) ? it->second() : nullptr;
        }

      private:
        static auto &GetFactoryMap()
        {
            static std::map<std::string, std::function<std::unique_ptr<CameraBase>()>> s_factories;
            return s_factories;
        }
    };

} // namespace _Kits