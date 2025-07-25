#pragma once

#include "SourceLocation.h"
#include "spdlog/spdlog.h"

#ifdef _WIN32
#ifdef LOGGERCORE_EXPORTS
#define LOGGERCORE_API __declspec(dllexport)
#else
#define LOGGERCORE_API __declspec(dllimport)
#endif
#else
#define LOGGERCORE_API __attribute__((visibility("default")))
#endif

namespace _Kits
{
    class LOGGERCORE_API CRossLogger final
    {
      public:
        static CRossLogger &getLogger();
        ~CRossLogger();

        spdlog::level::level_enum getLogLevelFromString(std::string_view level);
        std::string getDefaultLogPattern(const std::string &tid);
        spdlog::source_loc getLogSourceLocation(const SourceLocation &location);

      protected:
        CRossLogger()
        {
            initLogger();
        }
        bool initLogger();
        void cleanExpiredFiles(const std::string &directory, int maxDays);

      private:
        // 存储主线程 ID
        uint64_t m_main_thread_id = 0;
        spdlog::level::level_enum m_level;
        spdlog::level::level_enum m_flushLevel = spdlog::level::warn;
        std::shared_ptr<spdlog::logger> m_logger;
    };

    // trace
    template <typename... Args>
    struct LogTrace
    {
        LogTrace(fmt::format_string<Args...> fmt, Args &&...args, SourceLocation location = {})
        {
            spdlog::log(CRossLogger::getLogger().getLogSourceLocation(location), spdlog::level::trace, fmt, std::forward<Args>(args)...);
        }
    };

    template <typename... Args>
    LogTrace(fmt::format_string<Args...> fmt, Args &&...args) -> LogTrace<Args...>;

    // debug
    template <typename... Args>
    struct LogDebug
    {
        LogDebug(fmt::format_string<Args...> fmt, Args &&...args, SourceLocation location = {})
        {
            spdlog::log(CRossLogger::getLogger().getLogSourceLocation(location), spdlog::level::debug, fmt, std::forward<Args>(args)...);
        }
    };

    template <typename... Args>
    LogDebug(fmt::format_string<Args...> fmt, Args &&...args) -> LogDebug<Args...>;

    // info
    template <typename... Args>
    struct LogInfo
    {
        LogInfo(fmt::format_string<Args...> fmt, Args &&...args, SourceLocation location = {})
        {
            spdlog::log(CRossLogger::getLogger().getLogSourceLocation(location), spdlog::level::info, fmt, std::forward<Args>(args)...);
        }
    };

    template <typename... Args>
    LogInfo(fmt::format_string<Args...> fmt, Args &&...args) -> LogInfo<Args...>;

    // warn
    template <typename... Args>
    struct LogWarn
    {
        LogWarn(fmt::format_string<Args...> fmt, Args &&...args, SourceLocation location = {})
        {
            spdlog::log(CRossLogger::getLogger().getLogSourceLocation(location), spdlog::level::warn, fmt, std::forward<Args>(args)...);
        }
    };

    template <typename... Args>
    LogWarn(fmt::format_string<Args...> fmt, Args &&...args) -> LogWarn<Args...>;

    // error
    template <typename... Args>
    struct LogError
    {
        LogError(fmt::format_string<Args...> fmt, Args &&...args, SourceLocation location = {})
        {
            spdlog::log(CRossLogger::getLogger().getLogSourceLocation(location), spdlog::level::err, fmt, std::forward<Args>(args)...);
        }
    };

    template <typename... Args>
    LogError(fmt::format_string<Args...> fmt, Args &&...args) -> LogError<Args...>;

    // critical
    template <typename... Args>
    struct LogCritical
    {
        LogCritical(fmt::format_string<Args...> fmt, Args &&...args, SourceLocation location = {})
        {
            spdlog::log(CRossLogger::getLogger().getLogSourceLocation(location), spdlog::level::critical, fmt, std::forward<Args>(args)...);
        }
    };

    template <typename... Args>
    LogCritical(fmt::format_string<Args...> fmt, Args &&...args) -> LogCritical<Args...>;

} // namespace _Kits
