#pragma once
#include <QObject>
#include <yaml-cpp/yaml.h>
#include <thread>
#include "tis_global/Struct.h"
#include <QSqlDatabase> // 数据库连接
#include <QSqlQuery>    // SQL查询执行
#include <QSqlError>    // 数据库错误处理
#include <mutex>
namespace _Kits
{
    class ConnectivityManagerBase : public QObject
    {
        Q_OBJECT

      public:
        explicit ConnectivityManagerBase(QObject *parent = nullptr);
        virtual ~ConnectivityManagerBase() noexcept;

        virtual bool init(const YAML::Node &config) = 0;
        virtual bool start() = 0;
        virtual bool stop() = 0;

        virtual QString getName() const = 0;

      signals:
        void sendOutSpeedDataTotal(TIS_Info::SpeedData speedData);  // 几个通道速度之和
        void sendOutSpeedDataSingle(TIS_Info::SpeedData speedData); // 单个通道速度
        void sendNaturalData(const QVariantMap, const QString);

      public slots:
        virtual void onControlSingleDo(const QVariant &data) = 0;
        virtual void onRecvYYJiheTrigger() = 0; // 接收YYJiHe触发信号
        virtual void onRecvSpeedSingle(uint8_t channel) = 0;

        virtual void onisSaveNaturalData(bool) = 0;

      protected:
        TIS_Info::SpeedData m_speedData;
        bool mb_isSimulation = false;
        uint32_t mi_simulationPulse = 0.0;

        std::thread m_thread_speedCalc; // 速度计算线程
        std::atomic<bool> mb_IsStop = true;

        bool _mb_isSaveNaturalData = false;
        QString _mstr_natural_data_path = "";
    };
} // namespace _Kits