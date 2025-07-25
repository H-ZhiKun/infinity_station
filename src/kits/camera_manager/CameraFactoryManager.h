#pragma once
// #include "BaumerBase.h"
// #include "BaumerManager.h"
#include "kits/camera_manager/baumer/BaumerManager.h"
// #include "CameraBase.h"
#include <map>
#include <memory>

namespace _Kits
{
// 相机工厂基类
class ICameraFactory
{
  public:
    virtual ~ICameraFactory() = default;
    virtual std::unique_ptr<BaumerManager> CreateCamera() = 0;
    static void RegisterFactory(const std::string &type,
                                std::unique_ptr<ICameraFactory> factory);
    static std::unique_ptr<BaumerManager> CreateCameraByType(
        const std::string &type);

  private:
    static std::map<std::string, std::unique_ptr<ICameraFactory>> s_factories;
};

// //海康相机工厂
// class HikCameraFactory : public ICameraFactory
// {
//   public:
//     std::unique_ptr<CameraBase> CreateCamera() override;
// };

// 海康NVR工厂
// class HikNvrFactory : public ICameraFactory
// {
//   public:
//     std::unique_ptr<CameraBase> CreateCamera() override;
// };

// 宝萌相机工厂
class BaumerCameraFactory : public ICameraFactory
{
  public:
    std::unique_ptr<BaumerManager> CreateCamera() override;
};

// class FFmpegUsbFactory : public ICameraFactory
// {
//   public:
//     std::unique_ptr<CameraBase> CreateCamera() override;
// };

} // namespace _Kits