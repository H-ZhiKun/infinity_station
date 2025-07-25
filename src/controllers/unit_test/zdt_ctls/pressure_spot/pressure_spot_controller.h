#pragma once

#include "kits/common/controller_base/ControllerBase.h"
#include "kits/communication/udp/UdpClient.h"
#include <mutex>
namespace _Controllers
{
    class PressureSpotController : public ControllerBase<PressureSpotController>
    {
      public:
        void sendSpeedToModule(const TIS_Info::SpeedData &);
        TASK_LIST_BEGIN
        SYNC_TASK_ADD(TIS_Info::UdpCenter::dispatchClient, PressureSpotController::setUdpClient);
        TASK_LIST_END
      private:
        void setUdpClient(const std::unordered_map<std::string, std::shared_ptr<_Kits::UdpClient>> &);

        const std::string m_udpName = "PressureSpot";
        TIS_Info::PressureSpotData m_pressureSpotData;
        std::mutex m_mutexData;
      private slots:
        void onFT910DataReceived(const std::shared_ptr<std::vector<QByteArray>> &ptrDatas);
    };

} // namespace _Controllers