#include "DBCommunicationHandle.h"
#include <iostream>
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;
DBCommunicationHandle::DBCommunicationHandle(QObject *parent)
{
}

DBCommunicationHandle::~DBCommunicationHandle()
{
}

int DBCommunicationHandle::connect(const std::string &url, const std::string &user, const std::string &pass, int max_conn)
{
    if (nullptr == m_pDBCommunication)
    {
        m_pDBCommunication = CreateDBComm();
    }

    auto ret = m_pDBCommunication->connect(url, user, pass, max_conn);
    if (!ret)
    {
        LogError("connect db fail:{}", ret);
        return -1;
    }

    LogInfo("connect db ret:{} ", ret);
    return 0;
}

void DBCommunicationHandle::initDB(bool bIsArc, bool bIsIR, bool bIsPanto, bool bIsPressAndAcc, bool bIsVideo)
{
    if (nullptr == m_pDBCommunication)
    {
        m_pDBCommunication = CreateDBComm();
    }
    m_hasArcTable = bIsArc;
    m_hasIrTable = bIsIR;
    m_hasPantoTable = bIsPanto;
    m_hasPressAndAccTable = bIsPressAndAcc;
    m_hasVideoTable = bIsVideo;

    if (m_hasArcTable)
    {
        m_pDBCommunication->initArcDB("ArcTemperatureMonitor", "1.0.1.210607");
        LogInfo("initArcDB success");
    }
    if (m_hasVideoTable)
    {
        m_pDBCommunication->initVideoDB("ArcTemperatureMonitor", "1.0.1.210607");
        LogInfo("initVideoDB success");
    }
    if (m_hasIrTable)
    {
        m_pDBCommunication->initIRDB("ArcTemperatureMonitor", "1.0.1.210607");
        LogInfo("initIRDB success");
    }
    if (m_hasPantoTable)
    {
        m_pDBCommunication->initPantoTable("ArcTemperatureMonitor", "1.0.1.210607");
        LogInfo("initPantoTable success");
    }
    if (m_hasPressAndAccTable)
    {
        m_pDBCommunication->initPressAndAccDB("ArcTemperatureMonitor", "1.0.1.210607");
        LogInfo("initPressAndAccDB success");
    }
}
int DBCommunicationHandle::startDB(const std::string &dataBaseName)
{
    if (nullptr == m_pDBCommunication)
    {
        LogError("DBCommunicationHandle::connect error");
        return -1;
    }

    if (m_bTaskState)
    {
        stopDB();
    }
    int ret = 0;
    if (!m_bTaskState)
    {
        bool bRet = true;
        if (m_hasArcTable)
        {
            if (!m_pDBCommunication->startArcDB(dataBaseName))
            {
                LogError("startArcDB error");
                ret = 1;
            }
        }
        if (m_hasVideoTable)
        {
            if (!m_pDBCommunication->startVideoDB(dataBaseName))
            {
                LogError("startVideoDB error");
                ret = 2;
            }
        }
        if (m_hasIrTable)
        {
            if (!m_pDBCommunication->startIRDB(dataBaseName))
            {
                LogError("startIRDB error");
                ret = 3;
            }
        }
        if (m_hasPantoTable)
        {
            if (!m_pDBCommunication->startPantoTable(dataBaseName))
            {
                LogError("startPantoTable error");
                ret = 4;
            }
        }
        if (m_hasPressAndAccTable)
        {
            if (!m_pDBCommunication->startPressAndAccDB(dataBaseName))
            {
                LogError("startPressAndAccDB error");
                ret = 5;
            }
        }
        //	Logger->Info("create or write Database name : %s  %s", dataBaseName, bRet ? "Success" : "Failed");
        LogInfo("startDB end;{}", ret);
        m_bTaskState = true;
    }
    return ret;
}
void DBCommunicationHandle::stopDB()
{
    if (!m_bTaskState.load())
    {
        return;
    }

    if (m_hasArcTable)
    {
        m_pDBCommunication->stopArcDB();
    }
    if (m_hasVideoTable)
    {
        m_pDBCommunication->stopVideoDB();
    }
    if (m_hasIrTable)
    {
        m_pDBCommunication->stopIRDB();
    }
    if (m_hasPantoTable)
    {
        m_pDBCommunication->stopPantoTable();
    }
    if (m_hasPressAndAccTable)
    {
        m_pDBCommunication->stopPressAndAccDB();
    }
    m_bTaskState.store(false);
}
bool DBCommunicationHandle::Push(SArcData &sArcData)
{
    // if (m_pConfigReader->m_iWriteVideoPath == 0 )
    // {
    // 	memset(sArcData.strVideoName, 0, MAX_PATH_LENGTH);
    // }
    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }

    if (m_bTaskState && m_hasArcTable)
    {

        auto ret = m_pDBCommunication->Push(sArcData);
        LogDebug("Push SArcData:{}", ret);
        return ret;
    }

    return false;
}

bool DBCommunicationHandle::Push(std::vector<SArcData> &vecArcData)
{
    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }

    if (m_bTaskState && m_hasArcTable)
    {
        return m_pDBCommunication->Push(vecArcData);
    }
    return false;
}

bool DBCommunicationHandle::Push(std::string strVideoName, double dVideoTime)
{
    if (m_bTaskState && m_hasVideoTable)
    {
        // SVideoData sVideoData;
        // sVideoData.iVideoCount = 1;
        // strcpy_s(sVideoData.strVideoNmae[0], NAME_STRING_SIZE, strVideoName.c_str());
        // //Logger->Info("sVideoData.strVideoNmae[0] %s strVideoName %s", sVideoData.strVideoNmae[0], strVideoName.c_str());
        // //CUniqueIdRecieverSingleton::GetInstance().GetFileTimeAndTimeStamp(sVideoData.llUniqueFiletime, sVideoData.dUniqueTimeStamp);
        // sVideoData.iTimeLen[0] = dVideoTime / 1000;
        // return Push(sVideoData);
    }
    else
    {
        // Logger->Error("push video data err, m_bTaskState = %d, hasVideoTable = %d", m_bTaskState, m_hasVideoTable);
    }
    return false;
}

bool DBCommunicationHandle::Push(SVideoData &sVideoData)
{
    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }

    static int iPushVideoCount = 0;
    if (m_bTaskState && m_hasVideoTable)
    {
        iPushVideoCount++;
        if (iPushVideoCount % 25 == 0)
            return m_pDBCommunication->Push(sVideoData);
        else
            return m_pDBCommunication->Push(sVideoData);
    }

    return false;
}

bool DBCommunicationHandle::Push(std::vector<SVideoData> &vecVideoData)
{
    if (m_bTaskState && m_hasVideoTable)
    {
        return m_pDBCommunication->Push(vecVideoData);
    }
    return false;
}

/***
int iFrameIndex,
double fAvgTemp,
double fMinTemp,
double fMaxTemp,
int iLeve,     报警等级
double dArmTemp,    报警温度
string strPicPathNameOutTemp   保存超限图片的绝对全路径名
***/
bool DBCommunicationHandle::Push(int iFrameIndex,
                                 double fAvgTemp,
                                 double fMinTemp,
                                 double fMaxTemp,
                                 int iLeve,
                                 double dArmTemp,
                                 std::string strPicPathNameOutTemp,
                                 SGlobalUniqueId &SGolbalUniqueID)
{
    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }

    if (m_bTaskState && m_hasIrTable)
    {
        // LogDebug("Push(int iFrameIndex, double fAvgTemp, double fMinTemp, double fMaxTemp,int iLeve,double dArmTemp,std::string
        // strPicPathNameOutTemp,SGlobalUniqueId& SGolbalUniqueID)");
        auto ret =
            m_pDBCommunication->Push(iFrameIndex, fAvgTemp, fMinTemp, fMaxTemp, iLeve, dArmTemp, strPicPathNameOutTemp, SGolbalUniqueID);
        // CUniqueIdRecieverSingleton::GetInstance().GetFileTimeAndTimeStamp(sIrData.llUniqueFiletime, sIrData.dUniqueTimeStamp);
        LogDebug("push sIrData:{}", ret);
    }
    return false;
}

bool DBCommunicationHandle::Push(SIRTemperatureData &sIrData)
{

    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }
    static int iPushIrCount = 0;
    if (m_bTaskState && m_hasIrTable)
    {
        iPushIrCount++;
        if (iPushIrCount % 50 == 0)
            return m_pDBCommunication->Push(sIrData);
        else
            return m_pDBCommunication->Push(sIrData);
    }

    return false;
}

bool DBCommunicationHandle::Push(std::vector<SIRTemperatureData> vecIrData)
{

    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }
    if (m_bTaskState && m_hasIrTable)
    {
        return m_pDBCommunication->Push(vecIrData);
    }
    return false;
}

bool DBCommunicationHandle::Push(SPantographData &sPantographData)
{

    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }
    if (m_bTaskState && m_hasPantoTable)
    {
        return m_pDBCommunication->Push(sPantographData);
    }
    return false;
}

bool DBCommunicationHandle::Push(std::vector<SPantographData> &vecSPantographData)
{

    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }
    if (m_bTaskState && m_hasPantoTable)
    {
        return m_pDBCommunication->Push(vecSPantographData);
    }
    return false;
}

bool DBCommunicationHandle::Push(SPressAndAccData &sPressAndAccData)
{

    if (nullptr == m_pDBCommunication)
    {
        LogError("m_pDBCommunication is nullptr");
        return false;
    }
    if (m_bTaskState && m_hasPressAndAccTable)
    {
        return m_pDBCommunication->Push(sPressAndAccData);
    }
    return false;
}
