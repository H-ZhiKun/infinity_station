#pragma once

#include "kits/common/database/CppBatis.h"
#include <iostream>
#include "kits/common/log/CRossLogger.h"

namespace _Kits
{
    class sqloi
    {
      public:
        sqloi() = default;
        ~sqloi() = default;

        template <typename T>
        bool insertSqlData(QList<T> &data)
        {
            // 插入数据到表中
            try
            {
                if (!_Kits::SqlInsert<T>().insert(data).exec())
                {
                    _Kits::LogError("insertSqlData:[{}] error", T::tableName().toStdString().c_str());
                }
            }
            catch (const std::exception &e)
            {
                std::cout << e.what() << '\n';
                return false;
            }

            return true;
        }
    };
} // namespace _Kits