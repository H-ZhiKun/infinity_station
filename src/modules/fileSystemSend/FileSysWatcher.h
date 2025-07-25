#pragma once

#include "kits/common/module_base/ModuleBase.h"
#include <yaml-cpp/yaml.h>
//#include "kits/required/log/CRossLogger.h"
#include<QDateTime>
#include<QDir>

using namespace _Kits;

namespace _Modules
{
    class FileSysWatcher : public ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(FileSysWatcher)

      public:
        FileSysWatcher(QObject *parent = nullptr);
        virtual ~FileSysWatcher() noexcept;

        bool init(const YAML::Node &config) override;
        bool start() override;
        bool stop() override;

      signals:
        void sendrootPath(const QString &); //发送根路径
        
      private:
        QString rootPath;
        QString m_watchPath;
        YAML::Node m_details;
    

    };
} // namespace _Modules
