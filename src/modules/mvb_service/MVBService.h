#pragma once
#include "kits/mvb_communication/src/MvbBaseInterface.h"
#include "kits/common/module_base/ModuleBase.h"
#include "mvbprocessmsg/MVBParseMsg.h"
#include "mvbprocessmsg/MVBPmsDataGenerator.h"
#include <QTimer>
#include <atomic>
#include <json/json.h>
#include <memory>
#include <qtmetamacros.h>

/***************************************************************************
 * @file    MVBService.h
 * @brief   mvb 与tmcs通信服务控制
 *
 *
 *
 * @note
 ***************************************************************************/

using namespace _Kits;
namespace _Modules
{
    class MVBService : public ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(MVBService)
      public:
        explicit MVBService(QObject *parent = nullptr);
        virtual ~MVBService() noexcept;
        MVBService(const MVBService &) = delete;
        MVBService &operator=(const MVBService &) = delete;
        virtual bool start(const YAML::Node &config) override;
        virtual bool stop() override;

      private:
        /**********************
         * @brief 接收到mvb数据
         * @param mvbMsgData  数据
         * @return
         ************************ */
        void recvMVBData(const MvbMsgData &mvbMsgData);
        /**********************
         * @brief 初始化心跳定时器
         * @param config  配置信息
         * @return
         ************************ */
        void initTimer(const YAML::Node &config);

        /**********************
         * @brief 发送mvb数据
         * @param addressIP  ip地址
         * @param port  端口
         * @param data  数据
         * @return true 表示成功，false表示失败
         ************************ */
        bool sendData(const std::string &addressIP, int port, const std::string &data);
      signals:
        // 更新tmcs数据
        void updateData(const QVariant &);
      private slots:
        // 接收模块状态
        void updateModulStatus(const QVariant &);
        // 接收报警信息
        void updateAlarmInfo(const QVariant &);
        // 接收唯一时间
        void notifyUniqueTime(const QVariant &);

      private:
        std::shared_ptr<MvbBaseInterface> m_mvbBaseptr = nullptr;              // mvb通信接口
        std::shared_ptr<MVBParseMsg> m_mvbParseMsgptr = nullptr;               // mvb数据解析接口
        std::unique_ptr<MVBPmsDataGenerator> m_mvbPmsDataGernerator = nullptr; // mvb 心跳pms数据组装
      private:
        std::atomic<bool> m_startFlag = false; // 启动标志
    };
} // namespace _Modules
