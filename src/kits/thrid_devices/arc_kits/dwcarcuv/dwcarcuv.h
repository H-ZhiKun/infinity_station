#pragma once

#include "kits/common/database/orm/TableStructs.h"
#include "kits/common/database/CppBatis.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <mutex>
#include <QObject>
namespace _Kits
{

    class DWCArcUV :  public QObject
    {
        Q_OBJECT
     public:
        DWCArcUV(QObject *parent = nullptr);
        ~DWCArcUV()= default;

        void setArcThreshold(double threshold) ;
        void WriteBatchToVector(const TIS_Info::arcData &);
        void WriteDataToDatabase();
        void setDeviceIndex(const std::string &index) ;
        void setCameraName(const std::string &cameraName) ;
        int getArcID(); // 获取燃弧ID


      signals:
          // 添加电压和燃弧相关信号
          void voltageReceived(double voltage);           // 接收到新的电压值
          void sendArcStatistics(TIS_Info::arcData);

      protected:
        bool checkArcOccurrence(double voltage) ;
        void updateArcStatistics(bool isArc) ;

      public slots:
        void OnDWCVoltageReceived(float voltage)  ;
        void OnVideoPathRecv(const TIS_Info::arcData & ) ; // 接收视频路径数据

      private:
        bool m_isReceiving;    // 接收状态标志
        int m_pulseCount;      // 当前燃弧事件的脉冲计数
        int m_totalPulseCount; // 总脉冲计数
        double m_lastVoltage;  // 上一次的电压值
        double m_arcThreshold; // 燃弧阈值

        std::vector<_Kits::_Orm::arc_data> m_arc_Vector; // 燃弧数据容器
        std::mutex m_mutex;

        std::vector<_Kits::_Orm::arc_data> m_normal_dataVector; // 写入数据库的容器
        std::mutex m_mutex_for_data;
        std::string devIndex; // 设备编号 用于触发哪个燃弧录像相机
        std::string m_cameraName; // 相机名称

    };

} // namespace _Kits