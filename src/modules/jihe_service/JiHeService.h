#pragma once
#include "kits/common/factory/ModuleRegister.h"
#include "kits/common/module_base/ModuleBase.h"
#include <thread>
#include "JiHeDataBaseOpt.h"
#include <QUdpSocket.h>
#include <QTimer.h> 
#include <vector>
#include "tis_global/Struct.h"
#include <QString>
#include <QTime>
namespace _Modules
{

    class JiHeService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(JiHeService)
      public:
        struct JiheTimeData
        {
          QString strTime;
          TIS_Info::JiHeData data;
        };
        explicit JiHeService(QObject *parent = nullptr);
        virtual ~JiHeService() noexcept;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;
        void run();
        void release();

      protected:
        void sendJiHeData(TIS_Info::JiHeData data);
      signals:
        /**********************
         * @brief 发送几何数据
         * @param msg  数据
         * @return 空
         ************************ */
        void sendJiHeData(const QVariant &);

        void sendJiHeDatatoLocation(const TIS_Info::JiHeData &);
      private slots:
        /**********************
         * @brief 任务接收
         * @param msg  数据
         * @return 空
         ************************ */
        void notifyTask(TIS_Info::TaskInfo);
        void readData();

        void onDataToAppTimer(); // 发送数据到界面
        void onDataToLocationTimer();      // 发送数据定位


      private:
        bool isDetection();
        std::vector<_Kits::_Orm::jihe_data> convertToDBData(const  std::vector<JiheTimeData> &);
        TIS_Info::JiHeData convertToTisData( _Kits::_Orm::jihe_data );
      private:

        std::shared_ptr<JiHeDataBaseOpt> m_pJiHeDataBaseOpt = nullptr;
        std::shared_ptr<QUdpSocket> m_pUdpSocket = nullptr;
        TIS_Info::TaskInfo m_currentTask;

        bool m_openAdjust = false;
        int  m_adjustRefreshTime=50;
        int  m_appRefreshTime=50;

        std::atomic<bool> m_runThreadFlag = false;
        std::thread m_runThread;
        int m_sendRate = 50;
        std::mutex m_dataMutex;

        std::vector<JiheTimeData> m_vecJiHeData;
        std::shared_ptr<QTimer> m_pToAppTimer;
        std::shared_ptr<QTimer> m_pToLocationTimer;
    };

} // namespace _Modules
