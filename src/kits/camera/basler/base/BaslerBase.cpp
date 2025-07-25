#include "BaslerBase.h"

_Kits::BaslerBase::BaslerBase(QObject *parent) : QObject(parent)
{
}

_Kits::BaslerBase::~BaslerBase()
{
}

bool _Kits::BaslerBase::setGainRaw(uint16_t gain)
{
    return true;
}

bool _Kits::BaslerBase::setExposureTime(uint16_t exposure)
{
    return true;
}

bool _Kits::BaslerBase::SetTrigger(std::string sMode, std::string sSource, std::string sEdge)
{
    return true;
}