
#include "FileSysWatcher.h"
#include "kits/common/log/CRossLogger.h"
namespace _Modules
{

    using namespace _Kits;

    FileSysWatcher::FileSysWatcher(QObject *parent) : ModuleBase(parent)
    {
    }
    FileSysWatcher::~FileSysWatcher() noexcept
    {
    }
    bool FileSysWatcher::init(const YAML::Node &config)
    {
        if (!config["config_path"])
        {
            return false;
        }
        std::string fullPath = std::filesystem::current_path().string() + config["config_path"].as<std::string>();
        try
        {
            m_details = YAML::LoadFile(fullPath);
        }
        catch (const YAML::BadFile &e)
        {
            //LogError("load file path error {}", fullPath);
            return false;
        }
        return true;
    }
    bool FileSysWatcher::start()
    {
        if (m_details["watch_path"].as<std::string>().empty())
        {
            LogError("No watch path found.");
            return false;
        }
        m_watchPath = QString::fromStdString(m_details["watch_path"].as<std::string>());
        QString dateStr = QDateTime::currentDateTime().toString("yyyyMMdd");
        m_watchPath = QDir(m_watchPath).filePath(dateStr);
        rootPath = m_watchPath;
        emit sendrootPath(rootPath);
       
        _Kits::LogInfo("Watching directory: {}", rootPath.toStdString());
  

       
        return true;
    }
    bool FileSysWatcher::stop()
    {
      return true;
    }

 
    // void FileSysWatcher::onRefreshWatchDirs()
    // {
    //     QRegularExpression regex("^capture[0-4].*\\.jpg$", QRegularExpression::CaseInsensitiveOption);

    //     QDirIterator fileIt(rootPath, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);

    //     while (fileIt.hasNext())
    //     {
    //         QString fullPath = fileIt.next();
    //         QFileInfo fi(fullPath);
    //         if (fi.lastModified().msecsTo(QDateTime::currentDateTime()) < 500)
    //              continue;

    //         if (!regex.match(fi.fileName()).hasMatch())
    //             continue;
    //         emit sendPath(fullPath);
            
    //     }

    //     QTimer::singleShot(1000, this, SLOT(onRefreshWatchDirs()));
    
    // }
  
     
}