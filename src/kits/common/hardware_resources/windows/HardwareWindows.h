#pragma once
#include "kits/common/hardware_resources/base/HardwareBase.h"
namespace _Kits
{

    class HardwareWindows : public HardwareBase
    {
      public:
        explicit HardwareWindows();
        virtual ~HardwareWindows() noexcept;
        virtual QVariantMap resourcesInfo() override;
        virtual QVariantList logicalDriveInfo() override;

      private:
        std::string wstringToString(const std::wstring &wstr);
    };

} // namespace _Kits
