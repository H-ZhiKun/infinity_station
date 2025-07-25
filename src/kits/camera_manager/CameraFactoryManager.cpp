// #include "kits/camera_service/CameraFactory.h"
// #include "kits/camera_service/CameraBaumer.h"
// #include "kits/camera_service/CameraHik.h"
// #include "kits/camera_service/CameraHikNvr.h"
// #include "kits/camera_service/CameraUsb.h"
// #include "CameraFactoryManager.h"
#include "CameraFactoryManager.h"

using namespace _Kits;

std::map<std::string, std::unique_ptr<ICameraFactory>>
    ICameraFactory::s_factories;

void ICameraFactory::RegisterFactory(const std::string &type,
                                     std::unique_ptr<ICameraFactory> factory)
{
    s_factories[type] = std::move(factory);
}

std::unique_ptr<BaumerManager> ICameraFactory::CreateCameraByType(
    const std::string &type)
{
    auto it = s_factories.find(type);
    if (it != s_factories.end())
    {
        return it->second->CreateCamera();
    }
    return nullptr;
}

// std::unique_ptr<CameraBase> HikCameraFactory::CreateCamera()
// {
//     return std::make_unique<CameraHik>();
// }

// std::unique_ptr<CameraBase> HikNvrFactory::CreateCamera()
// {
//     return std::make_unique<CameraHikNvr>();
// }
// 堡盟相机管理类  std::unique_ptr<BaumerManager>
std::unique_ptr<BaumerManager> BaumerCameraFactory::CreateCamera()
{
    return std::make_unique<BaumerManager>();
}

// std::unique_ptr<CameraBase> FFmpegUsbFactory::CreateCamera()
// {
//     return std::make_unique<CameraUsb>();
// }

/*使用示例
    ICameraFactory::RegisterFactory("HIK",
   std::make_unique<HikCameraFactory>());

    // 创建相机实例
    auto camera = ICameraFactory::CreateCameraByType("HIK");
    if (camera) {
        camera->openCamera(ip, user, pwd, port);
    }
*/