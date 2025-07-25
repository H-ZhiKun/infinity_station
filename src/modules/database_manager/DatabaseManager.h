#pragma once
#include "kits/common/module_base/ModuleBase.h"
#include "kits/common/csv_to_sql/CsvToSqlService.h"
#include "tis_global/EnumClass.h"
#include "tis_global/Function.h"
#include <unordered_set>
#include <vector>

namespace _Modules
{

    class DatabaseManager : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(DatabaseManager)
        TIS_CONNECT(TIS_Info::DatabaseManager::initStatus, TIS_Info::QmlPrivateEngine::callFromCpp)

      public:
        explicit DatabaseManager(QObject *parent = nullptr);
        virtual ~DatabaseManager() noexcept;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;
      signals:
        void initStatus(TIS_Info::QmlCommunication::ForQmlSignals ForQmlSignals, const QVariant &bInit);

      private:
        std::vector<QString> m_rdbms;
        std::unique_ptr<_Kits::csv_to_sql_service> mp_csvToSqlService;
    };

} // namespace _Modules
