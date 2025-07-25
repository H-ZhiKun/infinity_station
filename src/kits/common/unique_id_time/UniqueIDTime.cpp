#include "UniqueIDTime.h"
#include <chrono>

uint64_t UniqueIDTime::getFileTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    // 转换为毫秒
    auto microsseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    uint64_t resultime = 0;

    resultime =  microsseconds * 10;
    
    resultime += C_EPOCH_TIME * 10000000LL;
    // resultime += milliseconds * 10;
    return resultime;
}

void UniqueIDTime::startTiming()
{
    m_startTime = getNanoSeconds();
}


double UniqueIDTime::getElapseTime()
{
    auto endTime = getNanoSeconds();
    double duration = (endTime - m_startTime)*1000 / 1000000000.0;
    return duration;
}

uint64_t UniqueIDTime::getNanoSeconds()
{
    auto now = std::chrono::high_resolution_clock::now();
    // 转换为纳秒
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    return nanoseconds;
}