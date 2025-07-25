#pragma once

#include "OfflineDateStatistic.h"
#include "OfflineSqlOp.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/radar_kit/base/DeviceData.hpp"
#include "kits/common/read_csv/ReadCSVFile.h"
#include "kits/common/factory/ModuleRegister.h"
#include "kits/common/module_base/ModuleBase.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QList>
#include <QString>
#include <QStringList>
#include <Qfile>
#include <qDebug>
#include <string>
#include "kits/common/serialization/serialize_jsoncpp.h"
#include "kits/common/serialization/serialize_qvariantmap.h"

namespace _Modules
{

    class OfflineRadarService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(OfflineRadarService)

      public:
        OfflineRadarService();

        virtual ~OfflineRadarService() noexcept; // 显式声明noexcept
        bool init(const YAML::Node &config) override;
        bool start() override;
        bool stop() override;

      private:
        // 启动，也是重启 改变只是改变taskid，也就是重新索引一次表
        bool StartSearch(int taskId);
        bool StartSearch(const QString &taskIdPath);
        // 主要是clear一次
        bool StopSearch();
        // 筛选条件
        bool SearchCondition();
        // 查询任务列表
        bool SearchTaskList();

        Json::Value ConvertToJson(const std::deque<std::unique_ptr<_Kits::_Orm::radar_data>> &results); // 转换为json格式
        Json::Value ParsePointsToJsonArray(const QString &pointsStr);                                   // 解析点云数据为json格式
        Json::Value ConvertToJson(const std::vector<_Kits::_Orm::location_data> &results);
        Json::Value ConvertToJson(const std::vector<OfflineDateStatistic::OverInfoOffline> &results);
        Json::Value ConvertToJson(const std::vector<_Kits::_Orm::task_data> &task_data_);               // 任务表转换json
        bool readRadarCsv(const QString &filePath, std::vector<_Kits::_Orm::radar_over_data> &results); // 读取超限CSV文件
        QStringList ParseCsvLine(const QString &line);                                                  // 解析CSV行

      signals:
        // 点云                 需要随时间戳流动实时更新
        void radarDataSendQml(const QVariant &data);

        // 定位信息             需要随时间戳流动实时更新
        void localtionDataSendQml(const QVariant &data);

        // 时间戳前后           任务的起始时间戳和结束时间戳
        void timeLineSendQml(const QVariant &data);

        // 超限内容统计发送     只发送一次，但是每次重新索引都需要再发送
        void overInfoSendQml(const QVariant &data);

        // 任务列表发送给qml
        void taskTableSendQml(const QVariant &data);

      public slots:
        // 接受qml发送的时间戳
        void OnreceiveTimeLine(const QVariant &data);

        // 接受qml发送的任务选择指令        查询任务id
        void OnreceiveTaskStart(const QVariant &data);
        // 接收qml发送任务csv解析结果json
        void OnreceiveTaskCSV(const QVariant &data);

      private:
        // 存储点云数据缓冲区，需要实时去sql查询
        std::deque<std::unique_ptr<_Kits::_Orm::radar_data>> m_radarData_;
        // 存储超限
        std::vector<std::unique_ptr<_Kits::_Orm::radar_over_data>> m_overData_;
        // 数据库操作类
        std::unique_ptr<OfflineSqlOp> m_sqlOp_;
        // 超限数据统计类
        std::unique_ptr<OfflineDateStatistic> m_overStatistic_;
        // 所有任务信息
        std::vector<_Kits::_Orm::task_data> m_taskData_;
        // std::pair<QString, QString> m_taskTimeRange_; // 前后时间戳
    };

} // namespace _Modules