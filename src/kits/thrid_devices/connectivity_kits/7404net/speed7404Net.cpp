#include "speed7404Net.h"

using namespace _Kits;

speed7404Net::speed7404Net(QObject *parent) : speedBase(parent)
{
    
}

speed7404Net::~speed7404Net()
{
    
}

std::vector<uint32_t> speed7404Net::parseDynamicCounterResponse(const std::vector<uint8_t>& response, int channelCount) {
    std::vector<uint32_t> counters;
    
    // 检查响应有效性
    if (response.size() < 25) { // 9(头部) + 4通道×4字节 = 25字节
        qWarning() << "Invalid response size:" << response.size();
        return counters;
    }

    // 解析所有4个通道
    for (int channel = 0; channel < channelCount; ++channel) {
        size_t startPos = 9 + channel * channelCount;
        uint32_t counter = 
            (static_cast<uint32_t>(response[startPos]) << 24) |
            (static_cast<uint32_t>(response[startPos+1]) << 16) |
            (static_cast<uint32_t>(response[startPos+2]) << 8) |
            static_cast<uint32_t>(response[startPos+3]);

        if (counter == 0x00)
        {
            continue;
        }
        
        counters.push_back(counter);
    }
    return counters;
}
