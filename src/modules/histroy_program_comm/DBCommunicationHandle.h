#pragma once
#include <string>
// #include "DataBaseDefine.h"
#include <atomic>
#include "DBCommunication.h"
#include <QObject>



class DBCommunicationHandle:public QObject
{
    Q_OBJECT
private:
    /* data */
    DBCommunication* m_pDBCommunication = nullptr;
public:
    explicit DBCommunicationHandle(QObject *parent = nullptr);
    virtual ~DBCommunicationHandle();

    void initDB(bool bIsArc,bool bIsIR,bool bIsPanto,bool bIsPressAndAcc,bool bIsVideo);
    int connect(const std::string& url, const std::string& user, const std::string& pass,int max_conn);
    int startDB(const std::string& dataBaseName);
	void stopDB();

    bool Push(SArcData& sArcData);
	bool Push(std::vector<SArcData>& vecArcData);
	bool Push(std::string strVideoName, double dVideoTime);
	bool Push(SVideoData& sVideoData);
	bool Push(std::vector<SVideoData>& vecVideoData);
	bool Push(int iFrameIndex, double fAvgTemp, double fMinTemp, double fMaxTemp,int iLeve,double dArmTemp,std::string strPicPathNameOutTemp,SGlobalUniqueId& SGolbalUniqueID);
	bool Push(SIRTemperatureData& sIrData);
	bool Push(std::vector<SIRTemperatureData> vecIrData);
	bool Push(SPantographData& sPantographData);
	bool Push(std::vector<SPantographData>& vecSPantographData);
	bool Push(SPressAndAccData& sPressAndAccData);

private:
    std::atomic_bool m_bTaskState = false;
    bool m_hasArcTable = false;
	bool m_hasVideoTable = false;
	bool m_hasIrTable = false;
	bool m_hasPantoTable = false;
	bool m_hasPressAndAccTable = false;

};

