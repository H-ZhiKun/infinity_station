#pragma once
#include "YYJiHe.h"
#include "kits/jihe_kits/JiHeManagerBase.h"
#include "kits/common/object_pool/ObjectPool.h"
namespace _Kits
{
    class YYJiHeManager : public JiHeManagerBase
    {
        Q_OBJECT
      public:
        explicit YYJiHeManager(QObject *parent = nullptr);
        virtual ~YYJiHeManager();
        virtual bool start(const std::string &configFile) override;
        virtual bool stop() override;

      private:
        bool initSystem();

        std::string m_strJcwPath;

        std::thread m_thInit_;
        bool m_bInit = false;

        std::shared_ptr<_Kits::YYJiHe> m_jihe;
        _Kits::CGWJJCDataBase m_jiheData;
        std::unique_ptr<ObjectPool<_Kits::CGWJJCDataBase>> m_pJHBufferPools;
    };
} // namespace _Kits