#pragma once

#include "kits/common/module_base/ModuleBase.h"
#include "kits/common/factory/ModuleRegister.h"
#include "modules/jihetrigger_service/JCWDataDef.h"

namespace _Modules
{
    class JiHeTriggerService : public _Kits::ModuleBase
    {
        Q_OBJECT

      public:
        JiHeTriggerService();
        ~JiHeTriggerService();

        bool start(const YAML::Node &config);
        bool stop();

      private:
        const void *JiHedata = nullptr; // 数据指针
                                        // 处理JiHeTriggerSignal数据
                                        // void processJiHeTriggerSignal(const QVariant &data);

      signals:
        // 信号定义
        void sendJiHeTriggerSignal(); // 发送JiHeTriggerSignal到其他模块

      public slots:
        // 槽函数定义
        void onJiHeDataReceived(const QVariant &data); // 接收JiHeData数据
    };
} // namespace _Modules