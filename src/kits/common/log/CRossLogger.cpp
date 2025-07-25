#include "CRossLogger.h"

#include <filesystem>
#include <spdlog/sinks/daily_file_sink.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <yaml-cpp/yaml.h>
#ifdef _WIN32
#include "GBKConsoleSink.h"
#endif
#include <iostream>
namespace _Kits
{
    CRossLogger &CRossLogger::getLogger()
    {
        static CRossLogger logger;
        return logger;
    }
    CRossLogger::~CRossLogger()
    {
        spdlog::shutdown();
        spdlog::drop_all();
    }
    spdlog::level::level_enum CRossLogger::getLogLevelFromString(std::string_view level)
    {
        if (level == "trace")
        {
            return spdlog::level::trace;
        }
        else if (level == "debug")
        {
            return spdlog::level::debug;
        }
        else if (level == "info")
        {
            return spdlog::level::info;
        }
        else if (level == "warn" || level == "warning")
        {
            return spdlog::level::warn;
        }
        else if (level == "err" || level == "error")
        {
            return spdlog::level::err;
        }
        else if (level == "critical")
        {
            return spdlog::level::critical;
        }
        else if (level == "off")
        {
            return spdlog::level::off;
        }
        // 默认级别为 info
        return spdlog::level::info;
    }
    spdlog::source_loc CRossLogger::getLogSourceLocation(const SourceLocation &location)
    {
        return spdlog::source_loc{location.FileName(), static_cast<int>(location.LineNum()), location.FuncName()};
    }

    std::string CRossLogger::getDefaultLogPattern(const std::string &tid)
    {
        return "%^[%l][%Y-%m-%d %T.%e][%s|%!|%#][TID:" + tid + ",%t]: %v%$";
    }

    bool CRossLogger::initLogger()
    {
        std::string strfilePath = std::filesystem::current_path().string() + "/config/config.yaml";
        auto config = YAML::LoadFile(strfilePath);
        // 初始化日志记录器

        std::string strRootPath;
        std::string strLogLevel = "debug";
        std::string strFlushLevel = "warn";
        uint16_t maxFileSize = 30;
        if (config["log"]["root_path"] && config["log"]["log_level"] && config["log"]["max_counts"])
        {
            strRootPath = config["log"]["root_path"].as<std::string>();
            strLogLevel = config["log"]["log_level"].as<std::string>();
            strFlushLevel = config["log"]["flush_level"].as<std::string>();
            maxFileSize = config["log"]["max_counts"].as<uint16_t>();
        }

        std::filesystem::path logPath(strRootPath);
        if (!std::filesystem::exists(logPath))
        {
            if (!std::filesystem::create_directories(logPath))
            {
                throw std::runtime_error(fmt::format("Create logger path failed: {}", strRootPath));
            }
        }

        std::ostringstream oss;
        oss << std::this_thread::get_id();
        auto tid = oss.str();
        std::string pattern = getDefaultLogPattern(tid);
        m_level = getLogLevelFromString(strLogLevel);
        m_flushLevel = getLogLevelFromString(strFlushLevel);
        std::string fileName = strRootPath + std::string("/TIS.log");
        auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(fileName, 1, 0, false, maxFileSize);
        daily_sink->set_level(m_level);
        daily_sink->set_pattern(pattern);
        // 包装成带等级过滤功能的 sink
        spdlog::sink_ptr consoleSink = nullptr;
#ifdef _WIN32
        consoleSink = std::make_shared<GBKConsoleSink>();
#else
        consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#endif
        consoleSink->set_level(m_level);
        consoleSink->set_pattern(std::string(pattern));
        std::vector<spdlog::sink_ptr> sinks{daily_sink, consoleSink};
        m_logger = std::make_shared<spdlog::logger>("CRossLogger", std::begin(sinks), std::end(sinks));
        m_logger->set_level(m_level);
        m_logger->flush_on(m_flushLevel);
        spdlog::set_default_logger(m_logger);
        cleanExpiredFiles(strRootPath, maxFileSize);
        return true;
    }

    void CRossLogger::cleanExpiredFiles(const std::string &directory, int maxDays)
    {
        namespace fs = std::filesystem;
        using namespace std::chrono;

        const auto now = system_clock::now();

        try
        {
            for (const auto &entry : fs::directory_iterator(directory))
            {
                const auto lastWriteTime = fs::last_write_time(entry);
                const auto sysTime = time_point_cast<system_clock::duration>(lastWriteTime - fs::file_time_type::clock::now() + now);

                auto ageInDays = duration_cast<hours>(now - sysTime).count() / 24;
                if (ageInDays > maxDays)
                {
                    fs::remove_all(entry.path());
                    std::cout << "[Log Cleanup] Removed: " << entry.path() << std::endl;
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "[Log Cleanup] Error: " << e.what() << std::endl;
        }
    }
} // namespace _Kits
