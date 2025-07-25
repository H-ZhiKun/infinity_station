#include "MVBMsgForward.h"

namespace _Modules
{
    using namespace _Kits;

    MVBMsgForward::MVBMsgForward(QObject *parent) : _Kits::ModuleBase(parent)
    {
    }
    MVBMsgForward::~MVBMsgForward() noexcept
    {
    }

    bool MVBMsgForward::init(const YAML::Node &config)
    {
        return true;
    }

    bool MVBMsgForward::start()
    {
        return true;
    }

    bool MVBMsgForward::stop()
    {
        return true;
    }

    void MVBMsgForward::sendMvbRecvData(const QVariantMap &recvData)
    {
        emit mvbRecvDataForward(recvData);
    }

} // namespace _Modules
