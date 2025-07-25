#include "timestamp_fit_old.h"

namespace _Kits {

     int64_t TimestampFit::DatetimeToFiletime(const QDateTime& datetime) 
     {
         try {
             // 获取Unix时间戳(秒)
             int64_t timestamp = datetime.toSecsSinceEpoch();
             
             // 转换为Windows文件时间
             int64_t filetime = (timestamp * WINDOWS_TICK) + EPOCH_DIFFERENCE;
             
             return filetime;
         }
         catch (const std::exception&) {
             // 如果转换失败，返回当前时间的filetime
             return (QDateTime::currentDateTime().toSecsSinceEpoch() * WINDOWS_TICK) + EPOCH_DIFFERENCE;
         }
     }


} // namespace _Kits