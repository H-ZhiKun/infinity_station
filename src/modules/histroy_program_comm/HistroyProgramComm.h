#pragma once
#include "kits/common/module_base/ModuleBase.h"
#include "TaskCommunicationHandle.h"
#include "MvbCommunicationHandle.h"
#include "DBCommunicationHandle.h"
#include <atomic>
#include <memory>

// #include <qtmetamacros.h>

/***************************************************************************
 * @file    HistroyProgramComm.h
 * @brief   与模块通信
 *
 *
 *
 * @note
 ***************************************************************************/

// using namespace _Kits;
namespace _Modules
{
    class HistroyProgramComm : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(HistroyProgramComm)
      public:
        explicit HistroyProgramComm(QObject *parent = nullptr);
        virtual ~HistroyProgramComm() noexcept;
        HistroyProgramComm(const HistroyProgramComm &) = delete;
        HistroyProgramComm &operator=(const HistroyProgramComm &) = delete;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      private:
        bool startDB(const YAML::Node &config);
        bool startMVbCommunication(const YAML::Node &config);
        void initrecvJCDBDataTime();
        void startUniqueIDRecviever(const YAML::Node &config);
      signals:
        void notifyTaskInfo(const QVariant &);
        void updateLocationInfo(const QVariant &);
      public slots:
        void receiveTaskInfoSlots(const QVariant &);
        void sendAmifData(const QVariant &);
        void sendAsatData(const QVariant &);
        void writeArcData(const QVariant &);
        void writePressAndAccData(const QVariant &);
        void writeIRData(const QVariant &);

      private:
        std::unique_ptr<TaskCommunicationHandle> m_taskCommunicationHandle = nullptr;
        std::unique_ptr<DBCommunicationHandle> m_dbCommunicationHandle = nullptr;
        std::unique_ptr<MvbCommunicationHandle> m_mvbCommunicationHandle = nullptr;

      private:
        std::atomic<bool> m_startFlag = false; // 启动标志
        YAML::Node m_details;
    };
} // namespace _Modules
