#include "TaskCommunicationHandle.h"
#include <iostream>
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;
TaskCommunicationHandle::TaskCommunicationHandle(QObject *parent)
{
}

TaskCommunicationHandle::~TaskCommunicationHandle()
{
    DestroyTaskComm(m_pTaskCommunication);
}

void TaskCommunicationHandle::init()
{
    m_pTaskCommunication = CreateTaskComm();
    // m_pTaskCommunication = CreateTaskCommunication();
    // auto callback = std::bind(&TaskCommunicationHandle::recvTaskInfo, this, std::placeholders::_1);
    auto callBack = std::function<void(const stTaskStateReply &)>(
        [this](const stTaskStateReply &taskReply) -> void { return recvTaskInfo(taskReply); });
    m_pTaskCommunication->init(callBack);
}

void TaskCommunicationHandle::recvTaskInfo(const stTaskStateReply &taskInfo)
{
    // if(1 == taskInfo.state)
    // {
    //     // if (nullptr != m_pDBCommunicationHandle)
    //     // {
    //     //     m_pDBCommunicationHandle->startDB(taskInfo.dbName);
    //     // }
    // }
    QVariantMap qvarmap;
    qvarmap["taskname"] = QByteArray::fromStdString(taskInfo.taskName);
    ;
    qvarmap["taskstate"] = taskInfo.state;
    qvarmap["taskdbname"] = QByteArray::fromStdString(taskInfo.dbName);

    emit sendTaskInfo(qvarmap);

    LogTrace("TaskCommunicationHandle::recvTaskInfo:{},{}", taskInfo.state, taskInfo.taskName);
}

int TaskCommunicationHandle::start(const std::string &ip, int port)
{
    if (nullptr != m_pTaskCommunication)
    {
        return m_pTaskCommunication->start(ip, port);
    }
    return -1;
}
