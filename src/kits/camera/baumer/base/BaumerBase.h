#pragma once
#include "baumer_camera/bgapi2_genicam.hpp"
#include "kits/common/object_pool/ObjectPool.h"
#include "tis_global/Struct.h"
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include <vector>

namespace _Kits
{
    class BaumerBase : public QObject
    {
        Q_OBJECT
      public:
        explicit BaumerBase(QObject *parent = nullptr);
        virtual ~BaumerBase();

        virtual bool init(BGAPI2::Device *pDevice)
        {
            return true;
        };

        virtual bool start()
        {
            return true;
        };

        virtual void storeImg(BGAPI2::Buffer *) = 0;

        virtual bool setName(const std::string &name)
        {
            m_name_ = name;
            return true;
        }

        virtual bool setIP(const std::string &ip)
        {
            m_ip_ = ip;
            return true;
        }

        virtual bool closeCamera()
        {
            return true;
        }

      signals:
        void sendImage(std::shared_ptr<TIS_Info::ImageBuffer> img);
        void sendArcImage(std::shared_ptr<TIS_Info::ImageBuffer> img);

      protected:
        std::shared_ptr<ObjectPool<TIS_Info::ImageBuffer>> m_ImageBufferPools;
        std::string m_name_;
        std::string m_ip_;
        BGAPI2::Device *m_pDevice = nullptr;
        BGAPI2::DataStream *m_pStream = nullptr;
    };
} // namespace _Kits