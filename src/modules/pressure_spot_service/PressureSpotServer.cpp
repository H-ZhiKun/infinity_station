#include "PressureSpotServer.h"
using namespace _Kits;
namespace _Modules
{

    PressureSpotServer::PressureSpotServer(QObject *parent)
    {
    
    }

    PressureSpotServer::~PressureSpotServer() noexcept
    {
    }

    bool PressureSpotServer::init(const YAML::Node &config)
    {
        return true;
    }
    bool PressureSpotServer::start()
    {
        return true;
    }

    bool PressureSpotServer::stop()
    {
        return true;
    }

} // namespace _Modules