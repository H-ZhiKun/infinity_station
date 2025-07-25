#ifndef _RADAR_SERVICE_H_
#define _RADAR_SERVICE_H_

#include "RadarOverrun.h"
#include "RadarRecord.h"
#include "kits/thrid_devices/radar_kit/base/DeviceData.hpp"
#include "kits/thrid_devices/radar_kit/base/DeviceDeal.h"
#include "kits/thrid_devices/radar_kit/base/MyThreadPool.h"
#include "kits/thrid_devices/radar_kit/sick/SickSingleData.h"
#include "kits/common/module_base/ModuleBase.h"
#include "yaml-cpp/yaml.h"
#include <QString>
#include <QTimer>
#include <QVariant>
#include <map>
#include <memory>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>
#include "tis_global/Struct.h"

#ifndef Q_MOC_RUN
#ifdef emit
#undef emit
#include <tbb/concurrent_queue.h>
#define emit
#else
#include <tbb/concurrent_queue.h>
#endif // emit
#endif // Q_MOC_RUN

namespace _Modules
{

    class RadarService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(RadarService)

        struct LineInfo
        {
            uint16_t line_start;
            uint16_t line_end;
        };

        struct OverrunInfo
        {
            LineInfo line_info;
            std::vector<TIS_Info::DeviceSingleData> point_info;
        };

        struct SkewInfo
        {
            float distance;
            float angle_1; //[angle_1,angle_2]筛选这个范围的点云 用于判断坐标系扶正的点云数据
            float angle_2;
        };

      public:
        RadarService();
        ~RadarService();
        bool init(const YAML::Node &config) override;
        bool start() override;
        bool stop() override;

      public slots:
        // 接收数据
        void OnDeviceDataGet(unsigned short device_index);

      private slots:
        void _OnTimerRestart();

        void notifyTask(TIS_Info::TaskInfo);

      public slots:
        void _OnTimerDataSendQml();

      private:
        void _isOverrun();

      signals:
        void DataSendQml(std::shared_ptr<std::vector<TIS_Info::DeviceSingleData>>);
        void StateSendQml(QString);
        void OverinfoSendQml(QString);
        void OverconfigSendQml(std::vector<TIS_Info::DeviceSingleData>);

      private:
        std::unordered_map<unsigned short, _Kits::DeviceSurfaceData> mmap_device_data; // 点云数据以中心点为坐标系原点的数据

        std::unordered_map<unsigned short, float> mmap_device_angle; // 保存每个相机的角度

        std::unordered_map<unsigned short, bool> mmap_device_state;

        std::unordered_map<unsigned short, std::shared_ptr<DeviceDeal>> mmap_device_deal; // 每个设备的编号，和对象

        std::atomic_bool mb_stop = false;

        std::thread mthread_device_data_normal;
        tbb::concurrent_queue<std::shared_ptr<_Kits::DeviceSurfaceData>> m_queue_device_data_normal;
        void _deviceDataNormal();

        std::thread mthread_device_data_overrun;
        tbb::concurrent_queue<std::shared_ptr<_Kits::DeviceSurfaceData>> m_queue_device_data_overrun;
        void _deviceDataOverrun();

        QTimer *m_timer_restart;
        inline static RadarRecord g_radar_record_sql;

        QTimer *m_timer_send_qml;
        QTimer *m_timer_state_qml;

        std::thread m_thread_sqlRecord; // 异步写入sql
        void _sqlRecord();

        std::thread m_thread_limit_send; // qml和sql都会记录

        std::vector<std::shared_ptr<_Kits::DeviceSurfaceData>> mqvec_data_send_qml;

        YAML::Node m_config;

        std::vector<OverrunInfo> mvec_overrun_info;

        RadarOverrun m_radar_overrun;
        int mi_index = -1; // 上一次超限信息的index位置

        SkewInfo m_skew_info;

        std::shared_ptr<_Kits::DeviceSurfaceData> mp_data_all_for_qml;
        std::mutex m_mutex_data_all_for_qml;
        std::mutex m_mutex_data;
        std::mutex m_mutex_dataget;
        std::unique_ptr<ThreadPool> m_threadPool; // 自定义线程池

        TIS_Info::TaskInfo m_task_info;

      private:
        // void RestartDevice();
        QString GetDirection(float angle);

        void DataRecordSql(std::shared_ptr<_Kits::DeviceSurfaceData> data, TIS_Info::TaskInfo &task_info);
    };

} // namespace _Modules

#endif