#pragma once

#ifdef _WIN32
#include <functional>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/EnumParameter.h>
#elif defined(__linux__) && defined(__aarch64__)
#include <pylonc/PylonC.h>
#endif

#include "tis_global/EnumClass.h"
#include "tis_global/Struct.h"
#include <QObject>

namespace _Kits
{
    class BaslerBase : public QObject
    {
        Q_OBJECT

      public:
        BaslerBase(QObject *parent = nullptr);

        virtual ~BaslerBase() noexcept override;

        virtual bool init(std::unique_ptr<Pylon::CInstantCamera> camera)
        {
            return true;
        };

        virtual bool start()
        {
            return true;
        };
        virtual bool closeCamera()
        {
            return true;
        };

      private slots:
        virtual bool setGainRaw(uint16_t gain);          // 基础类型(CInstantCamera)不支持
        virtual bool setExposureTime(uint16_t exposure); // 基础类型(CInstantCamera)不支持
        virtual bool SetTrigger(std::string sMode, std::string sSource, std::string sEdge);

      signals:
        void sendImage(std::shared_ptr<TIS_Info::ImageBuffer> img);
    };

    template <typename T>
    class MyImageEventHandler : public Pylon::CImageEventHandler
    {
      public:
        using MemberFunction = void (T::*)(Pylon::CInstantCamera &, const Pylon::CGrabResultPtr &);

        // 绑定对象和函数
        MyImageEventHandler(T *obj, MemberFunction func) : m_obj(obj), m_func(func)
        {
        }

        // 重写事件处理函数
        virtual void OnImageGrabbed(Pylon::CInstantCamera &camera, const Pylon::CGrabResultPtr &grabResult) override
        {
            if (m_obj && m_func)
            {
                (m_obj->*m_func)(camera, grabResult); // 调用成员函数
            }
        }

      private:
        T *m_obj;
        MemberFunction m_func;
    };

} // namespace _Kits