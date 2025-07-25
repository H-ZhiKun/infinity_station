#pragma once
// #include "TaskObserver.h"
#include <vector>
#include <memory>
#include "taskcommunication.h"
#include "taskInfo.h"
// #include "DBCommunicationHandle.h"
// #include "MVBCommunicationHandle.h"
#include <QObject>
#include <QVariant>

class TaskCommunicationHandle:public QObject
{
    Q_OBJECT
private:
    CTaskcommunication * m_pTaskCommunication = nullptr;
public:
    explicit TaskCommunicationHandle(QObject *parent = nullptr);
    virtual ~TaskCommunicationHandle();
    void init();
    int start(const std::string& ip, int port);

    signals:
    void sendTaskInfo(const QVariant& taskInfo);
private:
    void recvTaskInfo(const stTaskStateReply& taskInfo);
};

