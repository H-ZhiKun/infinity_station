#include "MvbCommunicationHandle.h"
#include <iostream>
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;

MvbCommunicationHandle::MvbCommunicationHandle(QObject *parent)
{
    m_pMVBCommunication = CreateMVBComm();
}

MvbCommunicationHandle::~MvbCommunicationHandle()
{
    DestroyMVBComm(m_pMVBCommunication);
}

bool MvbCommunicationHandle::sendAsat()
{
    return true;
}
bool MvbCommunicationHandle::sendAmif(int type, int level, long long llTimeStamp, double dTimeStamp)
{
    if (nullptr != m_pMVBCommunication)
    {
        m_pMVBCommunication->SendAmif(type, level, llTimeStamp, dTimeStamp);
    }

    return false;
}
void MvbCommunicationHandle::start(const YAML::Node &config)
{

    auto strjcdbserviceport = config["jcdbserviceip"].as<std::string>();
    auto njcdbserviceport = config["jcdbserviceport"].as<uint16_t>();
    auto ret = m_pMVBCommunication->startJCDB(strjcdbserviceport, njcdbserviceport);
    LogInfo("startJCDB,ip:{},port:{},ret:{}", strjcdbserviceport, njcdbserviceport, ret);

    auto strmvbrecieveip = config["mvbrecieveip"].as<std::string>();
    auto nmvbrecieveport = config["mvbrecieveport"].as<uint16_t>();
    ret = m_pMVBCommunication->startMVBReciever(strmvbrecieveip, nmvbrecieveport);
    LogInfo("startMVBReciever,ip:{},port:{},ret:{}", strmvbrecieveip, nmvbrecieveport, ret);

    auto strasatmanagerserviceip = config["asatmanagerserviceip"].as<std::string>();
    auto nasatmanagerserviceport = config["asatmanagerserviceport"].as<uint16_t>();
    ret = m_pMVBCommunication->startAsatProcessManager(strasatmanagerserviceip, nasatmanagerserviceport);
    LogInfo("startAsatProcessManager,ip:{},port:{},ret:{}", strasatmanagerserviceip, nasatmanagerserviceport, ret);

    auto strasatmvbserviceip = config["asatmvbserviceip"].as<std::string>();
    auto nasatmvbserviceport = config["asatmvbserviceport"].as<uint16_t>();
    ret = m_pMVBCommunication->startAsatMvb(strasatmvbserviceip, nasatmvbserviceport);
    LogInfo("startAsatMvb,ip:{},port:{},ret:{}", strasatmvbserviceip, nasatmvbserviceport, ret);

    auto stramifserviceip = config["amifserviceip"].as<std::string>();
    auto namifserviceport = config["amifserviceport"].as<uint16_t>();
    ret = m_pMVBCommunication->startAmif(stramifserviceip, namifserviceport);
    LogInfo("startAmif,ip:{},port:{},ret:{}", stramifserviceip, namifserviceport, ret);
}

bool MvbCommunicationHandle::getJcdbData(stJCDBInfo &jcdbDataInfo)
{
    return m_pMVBCommunication->GetJcdbData(jcdbDataInfo);
}

bool MvbCommunicationHandle::setJcdbCallBack()
{
    // auto funJcdbcallback = std::bind(&MvbCommunicationHandle::jcdbDataInfoCallback, this, std::placeholders::_1);
    auto funJcdbcallback = std::function<void(const stJCDBInfo &)>(
        [this](const stJCDBInfo &jcdbDataInfo) -> void { return jcdbDataInfoCallback(jcdbDataInfo); });

    return m_pMVBCommunication->SetJcdbCallBack(funJcdbcallback);
}
void MvbCommunicationHandle::jcdbDataInfoCallback(const stJCDBInfo &jcdbDataInfo)
{
    std::cout << "jcdbDataInfo.jcdb_data********:" << jcdbDataInfo.strLocationPole << ":" << jcdbDataInfo.strLocationStation << ":"
              << jcdbDataInfo.fLocationSpeed << std::endl;
}
