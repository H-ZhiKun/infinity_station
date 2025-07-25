#include "DwcDataDispatch.h"
#include "kits/common/log/CRossLogger.h"

using namespace _Modules;

_Modules::DwcDataDispatch::DwcDataDispatch() : ModuleBase()
{
}

_Modules::DwcDataDispatch::~DwcDataDispatch() noexcept
{
}

bool _Modules::DwcDataDispatch::init(const YAML::Node &config)
{
    return true;
}

bool _Modules::DwcDataDispatch::start()
{
    emit startCtrl();
    return true;
}

bool _Modules::DwcDataDispatch::stop()
{
    return true;
}

void _Modules::DwcDataDispatch::getVoltageDataFromCtrl(const float voltage_data)
{
    emit sendVoltageDataOut(voltage_data);
}

void _Modules::DwcDataDispatch::getElectricDataFromCtrl(const float electric_data)
{
    emit sendElectriDataOut(electric_data);
}

void _Modules::DwcDataDispatch::getPressData0FromCtrl(const float press_data)
{
    emit sendPressData0Out(press_data);
}

void _Modules::DwcDataDispatch::getAccData0xFromCtrl(const float acc_data)
{
    emit sendAccData0xOut(acc_data);
}
