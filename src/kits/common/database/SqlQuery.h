#pragma once
#include "kits/common/database/connections/DatabaseConnections.h"
#include <QSqlQuery>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include "kits/common/log/CRossLogger.h"

namespace _Kits
{

    template <typename T, typename DBDriver>
    class SqlQuery
    {
      public:
        SqlQuery()
        {
            m_db = DatabaseConnections<DBDriver>::getConnection();
            m_query = QSqlQuery(m_db);
        };
        virtual ~SqlQuery() = default;

      protected:
        void innerError()
        {
            LogError("[SQL执行]:sql = {}, error = {}", m_fullSQl.toStdString(), m_query.lastError().text().toStdString());
        }
        QString m_sql;
        QString m_fullSQl;
        QSqlDatabase m_db;
        bool m_success = false;
        QSqlQuery m_query;
    };

} // namespace _Kits
