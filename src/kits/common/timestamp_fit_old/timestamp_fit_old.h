#pragma once
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <QDateTime>
namespace _Kits {

class TimestampFit {
public:
    /**
     * @brief 将filetime字符串转换为日期时间字符串
     * @param filetime_str filetime字符串
     * @return 转换后的日期时间字符串 格式为 YYYY-MM-DD HH:MM:SS
     */
    static int64_t DatetimeToFiletime(const QDateTime& datetime);

private:
    static const int64_t WINDOWS_TICK = 10000000;
    static const int64_t EPOCH_DIFFERENCE = 116444736000000000LL;
};

} // namespace _Kits