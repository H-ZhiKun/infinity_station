#pragma once
#include "MVBCommunication.h"
#include "mvbinfo.h"
#include <QObject>
#include "yaml-cpp/yaml.h"

class MvbCommunicationHandle:public QObject
{
    Q_OBJECT
private:
    /* data */
    MVBCommunication * m_pMVBCommunication = nullptr;
public:
    explicit MvbCommunicationHandle(QObject *parent = nullptr);
    virtual ~MvbCommunicationHandle();
    bool sendAsat();
    bool sendAmif(int type,int level,long long llTimeStamp, double dTimeStamp);
    void start(const YAML::Node &config);   
    bool getJcdbData(stJCDBInfo & jcdbDataInfo);
    bool setJcdbCallBack();

private:
    void jcdbDataInfoCallback(const stJCDBInfo & jcdbDataInfo);
};


