#pragma once
#include "kits/common/log/CRossLogger.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include "tis_global/Struct.h"
#include "tis_global/Function.h"
#include "tis_global/EnumClass.h"
#include <memory>
#include <QObject>

namespace _Kits
{
    class InvokeBase : public QObject
    {
        Q_OBJECT

      public:
        explicit InvokeBase(const std::string &path, bool isAsync);
        virtual ~InvokeBase() noexcept;
        QMetaMethod getPrivateSlot();

      protected:
        std::string m_taskName;
        bool m_isAsync;
    };
} // namespace _Kits
