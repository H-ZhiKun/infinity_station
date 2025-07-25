#include "LocationFactory.h"
#include"RoughLocationBase.h"

std::shared_ptr<LocationBaseInterface> LocationFactory::createLocationBase(int type)
{
    switch (type)
    {
    case 1: {
        return std::make_shared<RoughLocationBase>();
    }
    break;

    default:
        break;
        return nullptr;
    }
    return nullptr;
}