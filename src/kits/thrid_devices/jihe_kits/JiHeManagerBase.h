#pragma once
#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
#include"JiHeStruct.h"

namespace _Kits
{
    class JiHeManagerBase : public QObject
    {
        Q_OBJECT
      public:
        explicit JiHeManagerBase(QObject *parent = nullptr);
        virtual ~JiHeManagerBase();
        virtual bool start(const std::string &configFile) = 0;
        virtual bool stop() = 0;
      signals:

        void sendJiHeData(const _Kits::CGWJJCDataBase&);
        private:
        
    };
} // namespace _Kits