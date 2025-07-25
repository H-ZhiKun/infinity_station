#pragma once
#include "baumer_camera/bgapi2_genicam.hpp"
#include "kits/camera/baumer/base/BaumerBase.h"
#include "kits/common/object_pool/ObjectPool.h"
#include "tis_global/Struct.h"
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include <vector>

namespace _Kits
{
    class BaumerCamera : public BaumerBase
    {
        Q_OBJECT

      public:
        explicit BaumerCamera();
        virtual ~BaumerCamera();

        virtual bool init(BGAPI2::Device *pDevice) override;
        virtual bool start() override;
        virtual void storeImg(BGAPI2::Buffer *) override;
        virtual bool closeCamera() override;

      public:
        static constexpr const char *CameraType = "NORMAL";

      private:
        void openDataStream();
        void closeDataStream();
        void addBufferToStream();
        std::vector<BGAPI2::Buffer *> m_vBuffers;
        bool m_bSuccess = false;
    };
} // namespace _Kits