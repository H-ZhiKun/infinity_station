#include "speed6051Net.h"
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;

speed6051Net::speed6051Net(QObject *parent) : speedBase(parent)
{
}

speed6051Net::~speed6051Net()
{
}

std::vector<uint32_t> speed6051Net::parseDynamicCounterResponse(const std::vector<uint8_t> &response, int channelCount)
{
    std::vector<uint32_t> counterData;
    const size_t HEADER_SIZE = sizeof(HEADER);
    static int si_count = -1;

    // 静态变量保存上一次未处理的剩余数据
    static std::vector<uint8_t> s_remainingData;
    std::vector<uint8_t> fullData = response;
    if (s_remainingData.size() > 0)
    {
        // 将上次剩余数据与本次数据拼接
        fullData.insert(fullData.end(), s_remainingData.begin(), s_remainingData.end());
        s_remainingData.clear(); // 清空剩余数据
    }

    // 查找协议头的位置（滑动窗口逐字节比对）
    size_t headerPos = -1;
    for (size_t i = 0; i <= fullData.size() - HEADER_SIZE; ++i)
    {
        if (std::equal(HEADER, HEADER + HEADER_SIZE, fullData.begin() + i))
        {
            headerPos = i;
            break;
        }
    }

    if (headerPos != 0)
    {
        s_remainingData.insert(s_remainingData.end(), response.begin(), response.begin() + headerPos);
    }

    // 跳过协议头
    size_t offset = headerPos + HEADER_SIZE;

    // 提取并转换计数器数据（大端转小端）
    uint32_t count0 = (static_cast<uint32_t>(fullData[offset])) << 8 | (static_cast<uint32_t>(fullData[offset + 1])) |
                      (static_cast<uint32_t>(fullData[offset + 2]) << 24) | static_cast<uint32_t>(fullData[offset + 3] << 16);

    // 0B 01 03 08 9E 1D 02 05 00 00 00 00 00 00 00 00 00

    if (++si_count % 250 == 0)
    {
        _Kits::LogInfo("Saved data size: {}", std::to_string(fullData.size()));

        // 创建16进制格式的字符串
        std::string hexString;
        for (uint8_t byte : fullData)
        {
            // 将每个字节转换为两位16进制字符串
            char hex[3];
            snprintf(hex, sizeof(hex), "%02X", byte);
            hexString += hex;
            hexString += " "; // 添加空格分隔
        }

        // 移除末尾多余的空格
        if (!hexString.empty())
        {
            hexString.pop_back();
        }

        _Kits::LogInfo("Hex data: {}", hexString);
        _Kits::LogInfo("count0: {}", std::to_string(count0));
    }

    if (channelCount == 1)
    {
        counterData.push_back(count0);
        if (si_count % 250 == 0)
        {
            _Kits::LogInfo("size: {}", counterData.size());
        }

        return counterData;
    }

    uint32_t count1 = (static_cast<uint32_t>(fullData[offset + 4]) << 8) | (static_cast<uint32_t>(fullData[offset + 5])) |
                      (static_cast<uint32_t>(fullData[offset + 6]) << 24) | static_cast<uint32_t>(fullData[offset + 7] << 16);

    // 根据通道数存储数据
    if (channelCount >= 1)
        counterData.push_back(count0);
    if (channelCount >= 2 && count1 != 0)
        counterData.push_back(count1);

    return counterData;
}