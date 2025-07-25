#pragma once
#include "OrmMapperImpl.h"
#include <QSqlRecord>
#include <QVariant>
#include <qlogging.h>
#include <qsqlrecord.h>
namespace _Kits
{
    inline QVariantMap fromRecord(const QSqlRecord &record)
    {
        QVariantMap map;
        for (int i = 0; i < record.count(); ++i)
        {
            map.insert(record.fieldName(i), record.value(i));
        }
        return map;
    }
} // namespace _Kits