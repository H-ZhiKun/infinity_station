#include "speed_controller.h"
#include "service/AppFramework.h"
#include "tis_global/Field.h"
#include <qvariant.h>
#include <sys/stat.h>

using namespace _Controllers;

void SpeedController::sendSpeedToModule(const TIS_Info::SpeedData &data)
{
    _Service::App().invokeModuleAsync(TIS_Info::LocationService::recvSpeedDisData, data);


    return;
}