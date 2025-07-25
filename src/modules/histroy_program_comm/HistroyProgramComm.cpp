#include "HistroyProgramComm.h"
#include "kits/common/log/CRossLogger.h"
#include <filesystem>
#include <functional>
#include <json/value.h>
#include <qobject.h>
#include <qtimer.h>
#include "kits/common/histroy_uniqueidrecv/HistoryUniqueIDRecviever.h"

// using namespace _Modules;

using namespace _Kits;
namespace _Modules
{

    HistroyProgramComm::HistroyProgramComm(QObject *parent)
    {
        m_startFlag.store(false);
    }

    HistroyProgramComm::~HistroyProgramComm() noexcept
    {
    }

    void HistroyProgramComm::startUniqueIDRecviever(const YAML::Node &config)
    {
        auto strserverip = config["uniqueidserverip"].as<std::string>();
        auto serverport = config["uniqueidserverport"].as<uint32_t>();
        auto strlocalip = config["uniqueidlocalip"].as<std::string>();

        HistoryUniqueIDRecviever::Instance().start(strserverip, serverport, strlocalip);
    }

    bool HistroyProgramComm::init(const YAML::Node &config)
    {
        if (!config["config_path"])
        {
            return false;
        }
        std::string fullPath = std::filesystem::current_path().string() + config["config_path"].as<std::string>();
        try
        {
            m_details = YAML::LoadFile(fullPath);
        }
        catch (const YAML::BadFile &e)
        {
            LogError("load file path error {}", fullPath);
            return false;
        }
        return true;
    }
    bool HistroyProgramComm::start()
    {
        if (m_startFlag.load())
        {
            LogInfo("history program comm is already started.");
            return false;
        }
        startDB(m_details);
        startMVbCommunication(m_details);
        if (nullptr == m_taskCommunicationHandle)
        {
            m_taskCommunicationHandle = std::make_unique<TaskCommunicationHandle>();
        }
        connect(m_taskCommunicationHandle.get(), &TaskCommunicationHandle::sendTaskInfo, this, &HistroyProgramComm::receiveTaskInfoSlots);

        m_taskCommunicationHandle->init();

        auto taskIP = m_details["taskip"].as<std::string>();
        auto taskport = m_details["taskport"].as<uint16_t>();

        auto ret = m_taskCommunicationHandle->start(taskIP, taskport);
        if (0 == ret)
        {
            m_startFlag.store(true);
            LogInfo("task communication start success.");
        }
        else
        {
            LogError("task communication start failed.ip:{},prot:{}", taskIP, taskport);
            m_startFlag.store(false);
        }
        startUniqueIDRecviever(m_details);
        return true;
    }

    bool HistroyProgramComm::startDB(const YAML::Node &config)
    {
        auto irconfig = config["sql"];
        auto strsqlserviceip = irconfig["serviceip"].as<std::string>();
        auto struser = irconfig["user"].as<std::string>();
        auto strpassword = irconfig["password"].as<std::string>();

        if (nullptr == m_dbCommunicationHandle)
        {
            m_dbCommunicationHandle = std::make_unique<DBCommunicationHandle>();
        }

        auto ret = m_dbCommunicationHandle->connect(strsqlserviceip, struser, strpassword, 10);
        if (0 != ret)
        {
            LogError("connect to db failed:{},{},{},{}", ret, strsqlserviceip, struser, strpassword);
            return false;
        }

        auto arctableflag = config["writearctableflag"].as<bool>();
        auto videotableflag = config["writevideotableflag"].as<bool>();
        auto irtableflag = config["writeirtableflag"].as<bool>();
        auto pantotableflag = config["writepantotableflag"].as<bool>();
        auto pressandacctableflag = config["writepressandacctableflag"].as<bool>();
        m_dbCommunicationHandle->initDB(arctableflag, irtableflag, pantotableflag, pressandacctableflag, videotableflag);

        LogInfo("connect to db success");
        return true;
    }

    bool HistroyProgramComm::startMVbCommunication(const YAML::Node &config)
    {
        if (nullptr == m_mvbCommunicationHandle)
        {
            m_mvbCommunicationHandle = std::make_unique<MvbCommunicationHandle>();
        }
        m_mvbCommunicationHandle->start(config);
        return true;
    }
    bool HistroyProgramComm::stop()
    {

        m_startFlag.store(false);
        HistoryUniqueIDRecviever::Instance().stop();
        return true;
    }

    void HistroyProgramComm::receiveTaskInfoSlots(const QVariant &taskInfo)
    {

        auto taskInfoMap = taskInfo.toMap();
        auto taskName = taskInfoMap.value("taskname").toByteArray().toStdString();
        auto taskStatus = taskInfoMap.value("taskstate").toInt();
        auto taskDBName = taskInfoMap.value("taskdbname").toByteArray().toStdString();
        emit notifyTaskInfo(taskInfo);
        if (nullptr != m_dbCommunicationHandle)
        {
            m_dbCommunicationHandle->startDB(taskDBName);
        }
    }

    void HistroyProgramComm::initrecvJCDBDataTime()
    {
        auto qtimer = new QTimer(nullptr);
        qtimer->setInterval(35);

        connect(qtimer, &QTimer::timeout, [this]() {
            if (nullptr == m_mvbCommunicationHandle)
            {
                LogError("m_mvbCommunicationHandle is nullprt");
                return;
            }
            stJCDBInfo jcdbDataInfo;
            m_mvbCommunicationHandle->getJcdbData(jcdbDataInfo);

            QVariantMap qvarmap;
            qvarmap["locationstation"] = QByteArray::fromStdString(jcdbDataInfo.strLocationStation);
            qvarmap["locationlinedir"] = jcdbDataInfo.iLocationLineDirection;
            qvarmap["locationpole"] = QByteArray::fromStdString(jcdbDataInfo.strLocationPole);
            qvarmap["locationmovedistance"] = jcdbDataInfo.dLocationMoveDistance;
            qvarmap["locationspeed"] = jcdbDataInfo.fLocationSpeed;
            qvarmap["linedir"] = jcdbDataInfo.linedir;

            emit updateLocationInfo(qvarmap);
        });
        // 启动定时器
        qtimer->start();
    }

    void HistroyProgramComm::sendAmifData(const QVariant &data)
    {
        auto amifData = data.toMap();
        auto iamiftype = amifData.value("amiftype").toInt();
        auto ilevel = amifData.value("amiflevel").toInt();
        auto llTimeStamp = amifData.value("lltimestamp").toLongLong();
        auto dTimeStamp = amifData.value("dtimestamp").toDouble();
        LogTrace("send amif data:{},{},{},{}", iamiftype, ilevel, llTimeStamp, dTimeStamp);
        if (nullptr != m_mvbCommunicationHandle)
        {
            m_mvbCommunicationHandle->sendAmif(iamiftype, ilevel, llTimeStamp, dTimeStamp);
            LogInfo("send amif data success:{},{},{},{}", iamiftype, ilevel, llTimeStamp, dTimeStamp);
        }
    }

    void HistroyProgramComm::sendAsatData(const QVariant &data)
    {
        if (nullptr != m_mvbCommunicationHandle)
        {
            m_mvbCommunicationHandle->sendAsat();
        }
    }

    void HistroyProgramComm::writeArcData(const QVariant &data)
    {
        auto arcDatamap = data.toMap();
        SArcData ranhuDataCell;
        ranhuDataCell.lId = 1;
        ranhuDataCell.llUniqueFiletime = arcDatamap.value("lltimestamp").toLongLong();
        ;
        ranhuDataCell.dUniqueTimeStamp = arcDatamap.value("dtimestamp").toDouble();
        strcpy(ranhuDataCell.strTime, arcDatamap.value("timestring").toString().toUtf8());
        ranhuDataCell.iArcID = arcDatamap.value("golbalarcid").toInt();
        ranhuDataCell.fArctime = arcDatamap.value("arctime").toFloat();
        ranhuDataCell.iArcLevel = arcDatamap.value("arclevel").toInt();
        ;
        ranhuDataCell.fArcRate = 0;
        strcpy(ranhuDataCell.strVideoName, arcDatamap.value("currentvideoname").toByteArray().data());
        ranhuDataCell.iPulseStart = arcDatamap.value("pulsestart").toInt();
        ranhuDataCell.iPulseStop = arcDatamap.value("pulsestop").toInt();
        LogTrace("Push arc data:{},{},{},{},{},{},{}",
                 ranhuDataCell.llUniqueFiletime,
                 ranhuDataCell.dUniqueTimeStamp,
                 ranhuDataCell.strTime,
                 ranhuDataCell.iArcID,
                 ranhuDataCell.fArctime,
                 ranhuDataCell.iArcLevel,
                 ranhuDataCell.strVideoName);
        if (nullptr != m_dbCommunicationHandle)
        {
            LogInfo("Push arc success data:{},{},{},{},{},{},{}",
                    ranhuDataCell.llUniqueFiletime,
                    ranhuDataCell.dUniqueTimeStamp,
                    ranhuDataCell.strTime,
                    ranhuDataCell.iArcID,
                    ranhuDataCell.fArctime,
                    ranhuDataCell.iArcLevel,
                    ranhuDataCell.strVideoName);
            m_dbCommunicationHandle->Push(ranhuDataCell);
        }
    }
    void HistroyProgramComm::writePressAndAccData(const QVariant &data)
    {
        SPressAndAccData sPressAndAccData;
        auto pressAndAccDatamap = data.toMap();
        sPressAndAccData.fVid[0] = pressAndAccDatamap.value("accout").toDouble();
        sPressAndAccData.fPress[0] = pressAndAccDatamap.value("pressout").toInt();
        LogTrace("Push press and acc data:{},{}", sPressAndAccData.fVid[0], sPressAndAccData.fPress[0]);
        // auto llTimeStamp = pressAndAccDatamap.value("lltimestamp").toLongLong();
        // auto dTimeStamp = pressAndAccDatamap.value("dtimestamp").toDouble();
        sPressAndAccData.fCon[0] = 0;
        if (nullptr != m_dbCommunicationHandle)
        {
            m_dbCommunicationHandle->Push(sPressAndAccData);
        }
    }
    void HistroyProgramComm::writeIRData(const QVariant &data)
    {
        SGlobalUniqueId SGolbalUniqueID;
        auto irDatamap = data.toMap();
        auto davgtemperature = irDatamap.value("avgtemperature").toDouble();
        auto ilevel = irDatamap.value("irlevel").toInt();
        auto dmintemperature = irDatamap.value("mintemperature").toDouble();
        auto dmaxtemperature = irDatamap.value("maxtemperature").toDouble();
        SGolbalUniqueID.llUniqueFiletime = irDatamap.value("lltimestamp").toLongLong();
        SGolbalUniqueID.dUniqueTimeStamp = irDatamap.value("dtimestamp").toDouble();
        auto qPicPath = irDatamap.value("picpath").toByteArray().toStdString();

        if (nullptr != m_dbCommunicationHandle)
        {
            LogDebug("Push ir success data:{},{},{},{},{},{},{}",
                    SGolbalUniqueID.llUniqueFiletime,
                    SGolbalUniqueID.dUniqueTimeStamp,
                    davgtemperature,
                    ilevel,
                    dmintemperature,
                    dmaxtemperature,
                    qPicPath);
            m_dbCommunicationHandle->Push(
                1, davgtemperature, dmintemperature, dmaxtemperature, ilevel, dmaxtemperature, qPicPath, SGolbalUniqueID);
        }
    }

} // namespace _Modules