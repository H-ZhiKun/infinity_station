#pragma once
#include "locationlib/LocationBaseInterface.h"
#include "locationlib/PositionBaseInterface.h"
#include "locationlib/PositionDataBase.h"

#include "thread"
#include <json/json.h>
#include <memory>
#include <qtmetamacros.h>

#include "tis_global/Struct.h"
#include "kits/common/factory/ModuleRegister.h"
#include "kits/common/module_base/ModuleBase.h"
#include "locationlib/TriggerSaveDB.h"
using namespace _Kits;
namespace _Modules
{
    class LocationService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(LocationService)
      public:
        explicit LocationService(QObject *parent = nullptr);
        virtual ~LocationService() noexcept;
        LocationService(const LocationService &) = delete;
        LocationService &operator=(const LocationService &) = delete;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      protected:
        void WriteData2DB(TIS_Info::PositionData data);
        void sendPositionData(const TIS_Info::PositionData &data);
      signals:
        /**********************
         * @brief 定位模块向其他模块发送定位数据
         * @param msg  数据
         * @return 空
         ************************ */
        void sendPositionData(const QVariant &);

        void sendLocationStatus(bool);
      private slots:

        void recvSpeedDisData(const TIS_Info::SpeedData &); // 速度模块调这个

        void recvRfidData(const QString &); // RFID模块调这个

        void recvMvbData(const QVariant &); // mvb模块调这个

        void recvHandData(const QVariant &); // 手动校准调这个

        void recvImageData(const TIS_Info::JiHeData &data); // 图像校正调这个

        void recvMaoDuanData(const QVariant &) ; // 锚段校正调这个

        /**********************
         * @brief 任务接收
         * @param msg  数据
         * @return 空
         ************************ */
        void notifyTask(TIS_Info::TaskInfo);

      private:
        QVariant getCurrentPosition();
        TIS_Info::PositionData getCurrentPositionData();

        void run();
        void release();
        bool isDetection();
        bool isSaveOneData();

      private:
        std::shared_ptr<LocationBaseInterface> m_locationBaseptr = nullptr;
        std::shared_ptr<PositionBaseInterface> m_positionBaseptr = nullptr;
        std::shared_ptr<PositionDataBase> m_positionDataBaseptr = nullptr;
        std::unique_ptr<TriggerSaveDB> m_triggerSaveDBptr = nullptr;
        TIS_Info::TaskInfo m_currentTask;
        std::atomic<bool> m_startButtonFlag = false;
        std::atomic<bool> m_runThreadFlag = false;
        std::thread m_runThread;
        int m_sendRate = 10; // 10ms能保证0-90kmh范围内数据可靠
        int m_lineLir=1;

        bool m_openImgAdjust = false;
        bool m_openMaoDuanAdjust = false;
        bool m_openRfidAdjust = false;
    };
} // namespace _Modules
