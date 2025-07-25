#pragma once

#include "kits/thrid_devices/connectivity_kits/base/speedBase.h"
#include <QHostAddress>
#include <QByteArray>
#include <mutex>
#include <condition_variable>
#include <deque>

namespace _Kits
{

    class speed6051Net : public speedBase
    {
        Q_OBJECT

      public:
        speed6051Net(QObject *parent = nullptr);
        virtual ~speed6051Net();

      public:
        /**
         * @brief 动态解析模块返回的计数值
         * @param response 模块返回的原始数据
         * @param channel 需要解析的通道编号（默认为 0）
         * @return 解析后的计数值，如果解析失败返回 0
         */
        virtual std::vector<uint32_t> parseDynamicCounterResponse(const std::vector<uint8_t> &response, int channelCount) override;

      private:
        inline static constexpr uint8_t HEADER[4] = {0x0B, 0x01, 0x03, 0x08};
        inline static constexpr uint8_t HEADER_SZIE = sizeof(HEADER);
    };

} // namespace _Kits