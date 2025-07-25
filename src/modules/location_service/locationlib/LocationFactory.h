#pragma once
#include "LocationBaseInterface.h"
#include <memory>

class LocationFactory
{

  public:
    LocationFactory(/* args */) = default;
    ~LocationFactory() = default;

    std::shared_ptr<LocationBaseInterface> createLocationBase(int type);
};
