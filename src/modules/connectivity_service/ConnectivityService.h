#pragma once

#include "kits/thrid_devices/connectivity_kits/base/ConnectivityManagerBase.h"
#include "kits/common/factory/ModuleRegister.h"
#include "kits/common/module_base/ModuleBase.h"
#include <QObject>
#include "tis_global/Struct.h"
namespace _Modules
{

    class ConnectivityService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(ConnectivityService)

      public:
        ConnectivityService();
        ~ConnectivityService();
        virtual bool init(const YAML::Node &config) override final;
        virtual bool start() override final;
        virtual bool stop() override final;

        constexpr static inline const char *speed = "speed";
        constexpr static inline const char *pulse = "pulse";
        constexpr static inline const char *distance = "distance";

      private:
        std::vector<std::unique_ptr<_Kits::ConnectivityManagerBase>> mvec_Connect; // 一般来说只会有一个速度程序

        bool _mb_isSave_natural = false;
        QString _mstr_root_file_path = "";

      public slots:
        void onRecvSpeedData(TIS_Info::SpeedData speedData);

      signals:
        void speedOutput(const TIS_Info::SpeedData &data);
        void JiHeTrigger(bool isStart); // 触发信号

        /// @brief 是否保存原始数据
        /// @param 是否保存
        void isSaveNaturalData(bool);

        void sendNaturalData(const QVariantMap, const QString);
        void initSaveNaturalDataCtrl(const QString);
    };

} // namespace _Modules
