#pragma once
#include "kits/common/read_csv/ReadCSVFile.h"
#include "kits/common/module_base/ModuleBase.h"
#include "sqlIo.h"
#include "json/json.h"
#include "PreviousTaskCtrl.h"
#include <QDateTime>
#include <QTimer>
#include <QUrl>
#include <atomic>
#include <json/json.h>
#include <memory>
#include <qtmetamacros.h>

/***************************************************************************
 * @file    TaskService.h
 * @brief   任务模块管理类
 *
 *
 *
 * @note
 ***************************************************************************/

using namespace _Kits;
namespace _Modules
{
    class TaskService : public ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(TaskService)
      public:
        explicit TaskService(QObject *parent = nullptr);
        virtual ~TaskService() noexcept;
        TaskService(const TaskService &) = delete;
        TaskService &operator=(const TaskService &) = delete;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void notifyTaskData(TIS_Info::TaskInfo); // 通知任务数据
        void notifyUniqueTime(const QVariant &); // 通知唯一时间
        void sendCSVData(const QVariant &);

        void sendLineName(const QVariant &lineNames); // 发送线路名
        void sendLineData(const Json::Value &);       // 发送线路数据
        void sendTaskData(const Json::Value);         // 发送任务数据_打开CSV方式
        void sendTaskNameToQml(const QString &);      // 发送任务名称到QML

        void sendStationData(const Json::Value &); // 发送站点数据

      public slots:
        void recvMBLKData(const QVariant &);        // 接收MBLK数据
        void OnrecvTaskName(const QString);         // 接收任务名称
        void onRecvTaskChooseSig(const QVariant &); // 接收任务选择信号

      private:
        /// @brief 将线路数据转换为Json格式
        /// @param line_info 线路数据
        /// @return
        bool convertLinedata(const std::vector<_Kits::_Orm::line_data> &line_info);
        void sortLineDataById(std::vector<_Kits::_Orm::line_data> &it);
        void recvStationName(const QString &station_name, const int &line_dir);

      private:
        std::atomic<bool> m_startFlag = false;
        std::unique_ptr<PreviousTaskCtrl> m_taskCtrlptr = nullptr;
        std::unique_ptr<ReadCSVFile> m_readCSVFile = nullptr;
        std::unique_ptr<sqlio> m_sqlio = nullptr;

        // ReadCSVFile m_readCSVFile;
        //  UniqueIDTime m_idTime;
    };
} // namespace _Modules
