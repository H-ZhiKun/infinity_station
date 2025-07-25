#include "JiHeTriggerService.h"

_Modules::JiHeTriggerService::JiHeTriggerService()
{
     JiHedata = nullptr;
}

_Modules::JiHeTriggerService::~JiHeTriggerService() 
{
     JiHedata = nullptr;
}


bool _Modules::JiHeTriggerService::start(const YAML::Node &config)
{


      return true;
}

bool _Modules::JiHeTriggerService::stop()
{
     return true;
}

void _Modules::JiHeTriggerService::onJiHeDataReceived(const QVariant &data)
{
         // 获取发送过来的 void* 数据
    JiHedata = data.value<void*>();
    
    if (!JiHedata) {
        qWarning() << "Received null JiHedata";
        return;
    }

    // 转换并获取数据
    JCWD jcwjh = *((JCWD*)JiHedata);
    JCWPosi posType = jcwjh.GetPosiflag();

    // 根据位置类型处理
    if (posType == JCXP_POLE) {
        emit sendJiHeTriggerSignal();
    }

}
