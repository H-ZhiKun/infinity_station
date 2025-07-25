#pragma once
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>
#include <chrono>

namespace _Kits
{
class TimeToString
{
private:
    /* data */
private:
    TimeToString(/* args */) = delete;
    ~TimeToString() = delete;

public: 
    static std::string getYearMonthDayStr()
    {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);    
        std::tm* localtime = std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(localtime, "%Y%m%d"); 
        return oss.str();
    }
    
    static std::string getYearMonthDayHourMinStr()
    {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);    
        std::tm* localtime = std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(localtime, "%Y_%m_%d_%H_%M"); 
        return oss.str();
    }

    static std::string getHourMinSecStr()
    {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);    
        std::tm* localtime = std::localtime(&t);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000; // 毫秒部分
        std::ostringstream oss;
        oss << std::put_time(localtime, "%H%M%S") << std::setw(2) << std::setfill('0') << ms.count(); 
        return oss.str();
    }
};
}




