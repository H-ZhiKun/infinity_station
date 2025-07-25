#include "kits/communication/udp/UdpClient.h"
#include "kits/pressure_spot_kits/PressureSpotBase/PressureSpotBase.h"
#include <mutex>
#include "tis_global/Struct.h"
namespace _Kits
{
    class PressureSpotFT910 : public PressureSpotBase
    {
        Q_OBJECT
    public:


        explicit  PressureSpotFT910(QObject *parent = nullptr) ;
        virtual ~PressureSpotFT910() ;

        virtual bool Init() override;
        virtual bool Start() override;
        virtual bool Stop() override;

        void GetPressureSpotData( TIS_Info::PressureSpotData &data);
    
    private slots:
        void OnReceiveData(const QByteArray &data);
        //std::unique_ptr<_Kits::UdpClient> m_udpClient;
    private:
        std::mutex m_mutexData;
        TIS_Info::PressureSpotData m_pressureSpotData;
    };
}