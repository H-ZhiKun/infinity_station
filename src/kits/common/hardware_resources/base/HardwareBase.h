#pragma once
#include <QVariantMap>

namespace _Kits
{

    class HardwareBase
    {
      public:
        explicit HardwareBase() = default;
        virtual ~HardwareBase() noexcept = default;
        HardwareBase(const HardwareBase &) = delete;
        HardwareBase &operator=(const HardwareBase &) = delete;
        virtual QVariantMap resourcesInfo() = 0;     // 资源信息
        virtual QVariantList logicalDriveInfo() = 0; // 硬盘信息
    };

} // namespace _Kits
