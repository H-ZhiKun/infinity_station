#include "speedBase.h"

_Kits::speedBase::speedBase(QObject *parent): QObject(parent)
{
}

_Kits::speedBase::~speedBase()
{
}

std::vector<uint32_t> _Kits::speedBase::parseDynamicCounterResponse(
    const std::vector<uint8_t> &response, int channelCount)
{
    return std::vector<uint32_t>();
}
