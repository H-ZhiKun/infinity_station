#pragma once
#include <QVariant>
#include "kits/common/database/CppBatis.h"
#include "kits/common/database/orm/TableStructs.h"

#include "tis_global/Struct.h"
namespace _Modules
{
    class JiHeDataBaseOpt
    {
      private:
        /* data */
      public:
        explicit JiHeDataBaseOpt(/* args */) = default;
        virtual ~JiHeDataBaseOpt() noexcept = default;

        bool DataBaseInsert(QVariant);
        bool DataBaseInsert(const std::vector<_Kits::_Orm::jihe_data> &);

      private:
    };
} // namespace _Modules
