#include "JiHeService.h"
#include "kits/common/log/CRossLogger.h"
#include "kits/common/module_base/ModuleBase.h"
#include "tis_global/EnumPublic.h"

using namespace _Kits;

namespace _Modules
{
    JiHeService::JiHeService(QObject *parent) : _Kits::ModuleBase(parent)
    {
        m_pJiHeDataBaseOpt = std::make_shared<JiHeDataBaseOpt>();
        // m_pUdpSocket=std::make_shared<QUdpSocket>(this);
        m_pUdpSocket = std::make_shared<QUdpSocket>();

        m_pToAppTimer = std::make_shared<QTimer>();

        m_pToLocationTimer=std::make_shared<QTimer>();
    }
    JiHeService::~JiHeService() noexcept
    {
        stop();
        if (m_pUdpSocket != nullptr)
        {
            m_pUdpSocket->close();
            m_pUdpSocket = nullptr;
        }
    }
    bool JiHeService::init(const YAML::Node &config)
    {
        if (!config["open_adjust"])
        {
            _Kits::LogError(" [Jihe Server][init], Missing 'open_adjust' in config");
           // return false;
        }
        else
        {
            m_openAdjust = config["open_adjust"].as<bool>();
        }

        if (!config["adjust_refresh_time"])
        {
            _Kits::LogError(" [Jihe Server][init], Missing 'adjust_refresh_time' in config");
            //return false;
        }
        else
        {
            m_adjustRefreshTime = config["adjust_refresh_time"].as<int>();
        }

        if (!config["app_refresh_time"])
        {
            _Kits::LogError(" [Jihe Server][init], Missing 'app_refresh_time' in config");
            //return false;
        }
        else
        {
            m_appRefreshTime = config["app_refresh_time"].as<int>();
        }
        return true;
    }
    bool JiHeService::start()
    {
        if (m_openAdjust)
        {
            connect(m_pToLocationTimer.get(), &QTimer::timeout, this, &JiHeService::onDataToLocationTimer);
            m_pToLocationTimer->start(m_adjustRefreshTime);//实际时间要根据现场环境来调整 极端条件50=1.25m
        }
    
        connect(m_pToAppTimer.get(), &QTimer::timeout, this, &JiHeService::onDataToAppTimer);
        m_pToAppTimer->start(m_appRefreshTime);

        if (!m_pUdpSocket->bind(QHostAddress::AnyIPv4, 6465))
        {
            _Kits::LogDebug(" [JiHeService Server][start], bind error");
            m_runThreadFlag.store(false);
            return false;
        }

        connect(m_pUdpSocket.get(), &QUdpSocket::readyRead, this, &JiHeService::readData);

        // 检查线程是否已经在运行
        if (m_runThread.joinable())
        {
            _Kits::LogDebug(" [JiHeService Server][start], thread already running");
            return false;
        }

        m_runThreadFlag.store(true);
        m_runThread = std::thread(&JiHeService::run, this);

        _Kits::LogDebug(" [JiHeService Server][start], finish");
        return true;
    }

    void JiHeService::readData()
    {
        while (m_pUdpSocket->hasPendingDatagrams())
        {
            // 获取当前数据报大小
            qint64 datagramSize = m_pUdpSocket->pendingDatagramSize();
            if (datagramSize < 0)
            {
                _Kits::LogDebug(" [JiHeService Server][readData] Invalid datagram size ={}");
                continue;
            }

            QByteArray datagram;
            datagram.resize(static_cast<int>(datagramSize)); // 显式转换为int
   

            qint64 readResult = m_pUdpSocket->readDatagram(datagram.data(), datagram.size());
            if (readResult < 0)
            {
                _Kits::LogDebug(" [JiHeService Server][readData] Read datagram failed");
                continue;
            }

            if (static_cast<size_t>(readResult) != sizeof(TIS_Info::JiHeData))
            {
                _Kits::LogDebug(" [JiHeService Server][readData] Datagram too small for JiHeData ={},{}",readResult,sizeof(TIS_Info::JiHeData));
                continue;
            }

            if (isDetection())
            {
                const TIS_Info::JiHeData *jhData = reinterpret_cast<const TIS_Info::JiHeData *>(datagram.constData());
                std::lock_guard<std::mutex> lock(m_dataMutex); // 2ms
                JiheTimeData timeData;
                timeData.strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
                timeData.data = std::move(*jhData);
                timeData.data.iTaskId = m_currentTask.nTaskID;
                m_vecJiHeData.push_back(std::move(timeData));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));//保证时间戳不重复
        }
    }

    std::vector<_Kits::_Orm::jihe_data> JiHeService::convertToDBData(const std::vector<JiheTimeData> &vecData)
    {
            std::vector<_Kits::_Orm::jihe_data> res;
        for(const auto&item : vecData)
        {
            const auto&data = item.data;
            _Kits::_Orm::jihe_data dbItem;
            dbItem.task_id = data.iTaskId;
            dbItem.zig0 = data.fZig[0];
            dbItem.hei0 = data.fHei[0];
            dbItem.zig_bc0=data.fZigBc[0];
            dbItem.hei_bc0=data.fHeiBc[0];
            dbItem.mohao_width0 = data.fMoHaoWidth[0];
            dbItem.mohao_height0=data.fMoHaoHeight[0];

            dbItem.zig1 = data.fZig[1];
            dbItem.hei1 = data.fHei[1];
            dbItem.zig_bc1=data.fZigBc[1];
            dbItem.hei_bc1=data.fHeiBc[1];
            dbItem.mohao_width1 = data.fMoHaoWidth[1];
            dbItem.mohao_height1=data.fMoHaoHeight[1];

            dbItem.zig2 = data.fZig[2];
            dbItem.hei2 = data.fHei[2];
            dbItem.zig_bc2=data.fZigBc[2];
            dbItem.hei_bc2=data.fHeiBc[2];
            dbItem.mohao_width2 = data.fMoHaoWidth[2];
            dbItem.mohao_height2=data.fMoHaoHeight[2];

            dbItem.zig3 = data.fZig[3];
            dbItem.hei3 = data.fHei[3];
            dbItem.zig_bc3=data.fZigBc[3];
            dbItem.hei_bc3=data.fHeiBc[3];
            dbItem.mohao_width3 = data.fMoHaoWidth[3];
            dbItem.mohao_height3=data.fMoHaoHeight[3];

            dbItem.left_bc_x = data.fLeftBCX;
            dbItem.left_bc_y = data.fLeftBCY;
            dbItem.right_bc_x = data.fRightBCX;
            dbItem.right_bc_y = data.fRightBCY;

            dbItem.diameter0=data.Diameter[0];
            dbItem.diameter1=data.Diameter[1];
            dbItem.diameter2=data.Diameter[2];
            dbItem.diameter3=data.Diameter[3];
            
            dbItem.created_time = item.strTime;
            dbItem.updated_time = item.strTime;
            res.push_back(std::move(dbItem));
        }
        return res;
    }

    TIS_Info::JiHeData JiHeService::convertToTisData( _Kits::_Orm::jihe_data  dbItem)
    {
        TIS_Info::JiHeData data;
        data.fZig[0] = dbItem.zig0;
        data.fHei[0] = dbItem.hei0;
        data.fMoHaoWidth[0] = dbItem.mohao_width0;
        data.fZig[1] = dbItem.zig1;
        data.fHei[1] = dbItem.hei1;
        data.fMoHaoWidth[1] = dbItem.mohao_width1;
        data.fZig[2] = dbItem.zig2;
        data.fHei[2] = dbItem.hei2;
        data.fMoHaoWidth[2] = dbItem.mohao_width2;
        data.fZig[3] = dbItem.zig3;
        data.fHei[3] = dbItem.hei3;
        data.fMoHaoWidth[3] = dbItem.mohao_width3;
        data.fLeftBCX = dbItem.left_bc_x;
        data.fLeftBCY = dbItem.left_bc_y;
        data.fRightBCX = dbItem.right_bc_x;
        data.fRightBCY = dbItem.right_bc_y;
        data.iTaskId = dbItem.task_id;
        return data;
    }


    bool JiHeService::stop()
    {
        release();
        return true;
    }
    void JiHeService::release()
    {
        m_runThreadFlag.store(false);
        if (m_runThread.joinable())
        {
            m_runThread.join();
        }
    }
    bool JiHeService::isDetection()
    {
        return m_currentTask.isRun();
    }
    void JiHeService::notifyTask(TIS_Info::TaskInfo data)
    {
        m_currentTask = data;

        switch (m_currentTask.nTaskstate)
        {
        case TIS_Info::EnumPublic::TASK_RUNNING:
            _Kits::LogDebug("[JiHe Server][Task start]");
            break;
        case TIS_Info::EnumPublic::TASK_STOPPED:
            _Kits::LogDebug("[JiHe Server][Task stop]");
            break;
        case TIS_Info::EnumPublic::TASK_PAUSED:
            _Kits::LogDebug("[JiHe Server][Task paused]");
            break;
        default:
            _Kits::LogWarn("[JiHe Server][notifyTask] Unknown task state: %d", m_currentTask.nTaskstate);
            break;
        }
    }

    void JiHeService::sendJiHeData(TIS_Info::JiHeData JiHeData)
    {
        QVariantMap JiHeDataVariant;
        JiHeDataVariant["zig0"] = JiHeData.fZig[0];
        JiHeDataVariant["hei0"] = JiHeData.fHei[0];
        JiHeDataVariant["zig1"] = JiHeData.fZig[1];
        JiHeDataVariant["hei1"] = JiHeData.fHei[1];
        JiHeDataVariant["zig2"] = JiHeData.fZig[2];
        JiHeDataVariant["hei2"] = JiHeData.fHei[2];
        JiHeDataVariant["zig3"] = JiHeData.fZig[3];
        JiHeDataVariant["hei3"] = JiHeData.fHei[3];
        JiHeDataVariant["moHaoLen"] = JiHeData.fMoHaoHeight[0];
        JiHeDataVariant["leftBCX"] = JiHeData.fLeftBCX;
        JiHeDataVariant["leftBCY"] = JiHeData.fLeftBCY;
        JiHeDataVariant["rightBCX"] = JiHeData.fRightBCX;
        JiHeDataVariant["rightBCY"] = JiHeData.fRightBCY;
        JiHeDataVariant["taskId"] = m_currentTask.nTaskID;
        JiHeDataVariant["positionType"] = JiHeData.iImgPosType;
        JiHeDataVariant["lineNum"] = JiHeData.lineNum;
        //_Kits::LogDebug("[JiHe Server][test] to controller zig={}",JiHeData.fZig[0] );
        emit sendJiHeData(JiHeDataVariant);
    }
    void JiHeService::run()
    {
        while (m_runThreadFlag)
        {
            if (!m_vecJiHeData.empty())
            {
                std::vector<_Kits::_Orm::jihe_data> vecData;
                //TIS_Info::JiHeData sendData;
                {
                    std::lock_guard<std::mutex> lock(m_dataMutex);
                    //sendData = m_vecJiHeData.back().data;
                    vecData = convertToDBData(m_vecJiHeData);
                    m_vecJiHeData.clear();
                }
                m_pJiHeDataBaseOpt->DataBaseInsert(vecData);
                //sendJiHeData(sendData);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } 
    }

void JiHeService::onDataToLocationTimer()
{
    static std::atomic<int16_t> lastPositionType = TIS_Info::LocationCommunication::PositionType::PT_NONE;
    if (m_vecJiHeData.empty())
    {
        return;
    }
    TIS_Info::JiHeData sendData;
    {
        std::lock_guard<std::mutex> lock(m_dataMutex);
        sendData = m_vecJiHeData.back().data;
    }

    if (sendData.iImgPosType == TIS_Info::LocationCommunication::PositionType::PT_IMG)
    {
        emit sendJiHeDatatoLocation(sendData);
        lastPositionType = TIS_Info::LocationCommunication::PositionType::PT_IMG;
    }
    else
    {
        if (lastPositionType == TIS_Info::LocationCommunication::PositionType::PT_IMG)
        {
            emit sendJiHeDatatoLocation(sendData);
        }
        lastPositionType = sendData.iImgPosType;
    }
}

void JiHeService::onDataToAppTimer()
{
    if (!m_vecJiHeData.empty())
    {
        TIS_Info::JiHeData sendData;
        {
            std::lock_guard<std::mutex> lock(m_dataMutex);
            sendData = m_vecJiHeData.back().data;
        }
        sendJiHeData(sendData);
    }
}
    

} // namespace _Modules
