#include <spdlog/sinks/sink.h>
#include <windows.h>
#include <spdlog/pattern_formatter.h>
#include <iostream>
#include <mutex>

namespace _Kits
{
    class GBKConsoleSink : public spdlog::sinks::sink
    {
      public:
        void log(const spdlog::details::log_msg &msg) override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            spdlog::memory_buf_t formatted;
            formatter_->format(msg, formatted);
            std::string utf8_str = fmt::to_string(formatted);
            std::string gbk_str = utf8_to_gbk(utf8_str);
            std::cout << gbk_str;
        }

        void flush() override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::cout << std::flush;
        }

        void set_pattern(const std::string &pattern) override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            formatter_ = std::unique_ptr<spdlog::formatter>(new spdlog::pattern_formatter(pattern));
        }

        void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            formatter_ = std::move(sink_formatter);
        }

      private:
        std::string utf8_to_gbk(const std::string &utf8)
        {
            int wide_len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
            if (wide_len <= 0)
                return {};

            std::wstring wide_str(wide_len, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wide_str[0], wide_len);

            int gbk_len = WideCharToMultiByte(CP_ACP, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (gbk_len <= 0)
                return {};

            std::string gbk_str(gbk_len, '\0');
            WideCharToMultiByte(CP_ACP, 0, wide_str.c_str(), -1, &gbk_str[0], gbk_len, nullptr, nullptr);

            gbk_str.pop_back(); // 移除末尾的 '\0'
            return gbk_str;
        }

        std::unique_ptr<spdlog::formatter> formatter_;
        std::mutex mutex_;
    };
} // namespace _Kits
