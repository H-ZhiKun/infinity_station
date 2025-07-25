#include "LocationService.h"
#include "kits/common/thread_pool/ConcurrentPool.h"
#include "locationlib/LocationFactory.h"
#include <functional>
#include <json/value.h>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qobject.h>
#include <qvariant.h>

#include "locationlib/RoughPositionBase.h"

#include "kits/common/database/CppBatis.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/common/log/CRossLogger.h"
#include "tis_global/Struct.h"
#include "tis_global/EnumClass.h"
// using namespace _Modules;
using namespace _Kits;
namespace _Modules
{

    LocationService::LocationService(QObject *parent)
    {
    }
    LocationService::~LocationService() noexcept
    {
        release();
    }

    bool LocationService::init(const YAML::Node &config)
    {
        // 防止重复启动
        if (m_runThreadFlag.load())
        {
            _Kits::LogError(" [Location Server][start], Service is already running");
            return false;
        }

        if (!config["send_rate"])
        {
            _Kits::LogError(" [Location Server][start], Missing 'send_rate' in config");
            return false;
        }
        else
        {
            m_sendRate = config["send_rate"].as<int>();
        }

        if (!config["open_img_adjust"])
        {
            _Kits::LogError(" [Location Server][start], Missing 'open_img_adjust' in config");
            return false;
        }
        else
        {
            m_openImgAdjust = config["open_img_adjust"].as<bool>();
        }

        if (!config["open_rfid_adjust"])
        {
            _Kits::LogError(" [Location Server][start], Missing 'open_rfid_adjust' in config");
            return false;
        }
        else
        {
            m_openRfidAdjust = config["open_rfid_adjust"].as<bool>();
        }

        if (!config["open_maoduan_adjust"])
        {
            _Kits::LogError(" [Location Server][start], Missing 'open_maoduan_adjust' in config");
            return false;
        }
        else
        {
            m_openMaoDuanAdjust = config["open_maoduan_adjust"].as<bool>();
        }

        if (!config["location_type"])
        {
            _Kits::LogError(" [Location Server][start], Missing 'location_type' in config");
            return false;
        }
        int type = config["location_type"].as<int>();

        _Kits::LogDebug(" [Location Server][start],send_rate:{},type{}", m_sendRate, type);
        if (nullptr == m_locationBaseptr)
        {
            LocationFactory locationFactory;
            m_locationBaseptr = locationFactory.createLocationBase(type);
            if (!m_locationBaseptr)
            {
                _Kits::LogError(" [Location Server][start], Failed to create LocationBaseInterface");
                return false;
            }
        }
        else
        {
            _Kits::LogError(" [Location Server][start],LocationBaseInterface is exist ");
        }
        // 业务
        if (nullptr == m_positionBaseptr)
        {
            m_positionBaseptr = std::make_shared<RoughPositionBase>();
        }
        else
        {
            _Kits::LogError(" [Location Server][start],PositionBaseInterface is exist ");
        }

        // 数据库
        m_positionDataBaseptr = std::make_shared<PositionDataBase>();

        auto ret = m_locationBaseptr->init(config);
        if (0 == ret)
        {
            m_locationBaseptr->start();
        }

        ret = m_positionBaseptr->init(config);
        if (0 == ret)
        {
            //
        }

        m_triggerSaveDBptr = std::make_unique<TriggerSaveDB>(25.0);
        // 开始运行
        m_runThreadFlag.store(true);
        m_runThread = std::thread(&LocationService::run, this);
        _Kits::LogDebug(" [Location Server][start],finish");
        return true;
    }
    bool LocationService::start()
    {
        return true;
    }
    bool LocationService::stop()
    {
        release();
        _Kits::LogDebug(" [Location Server][stop],finish");
        return true;
    }

    void LocationService::release()
    {
        m_runThreadFlag.store(false);
        if (m_runThread.joinable())
        {
            m_runThread.join();
        }
    }

    bool LocationService::isDetection()
    {
        return m_currentTask.isRun();
    }

    bool LocationService::isSaveOneData()
    {
        if(isDetection() && m_triggerSaveDBptr->isTrigger())
        {
            return true;
        }
        return false;
    }
    void LocationService::recvSpeedDisData(const TIS_Info::SpeedData &data)
    {
        if (!isDetection())
        {
            return;
        }

        double dis = data.mdble_kilometer;
        double speed = data.mdble_speed;
        if (speed >= 0)
        {
            m_locationBaseptr->adjustData(-1, speed);
        }
        if (dis >= 0)
        {
            m_locationBaseptr->setMoveDis(dis);
            //m_triggerSaveDBptr->setcurDis(dis * 100000.0);
        }
    }

    void LocationService::recvRfidData(const QString &data)
    {
        if (!isDetection() || !m_openRfidAdjust)
        {
            return;
        }
        QString RfidId = data;
        double dis = -1;
        if (RfidId.size() > 0)
        {
            // RfidId="6465";
            _Kits::LogDebug("[Location Server][Rfid Adjust],begin {}", RfidId.toStdString());
            int ret = m_positionDataBaseptr->DataBaseQuery(RfidId,m_currentTask.nLineDir, dis);
            if (ret == 0)
            {
                _Kits::LogDebug("[Location Server][Rfid Adjust],end {}-{}", dis, m_currentTask.dKilometer);
            
                dis = m_lineLir*(dis - m_currentTask.dKilometer);
                m_locationBaseptr->adjustData(dis, -1);

                auto positionData = std::move(getCurrentPositionData());
                positionData.positionType = TIS_Info::LocationCommunication::PositionType::PT_RFID;
                WriteData2DB(positionData);
                sendPositionData(positionData);
            }
        }
    }

    void LocationService::recvMvbData(const QVariant &data) // todo line表还没有stationid字段
    {
        if (!isDetection())
        {
            return;
        }
        QVariantMap mapData = data.toMap();
        QVariantMap LocationData = mapData.value("data", QVariantMap()).toMap();
        int stationId = LocationData.value("stationId", -1).toInt();

        if (stationId > 0)
        {
            _Kits::LogDebug("[Location Server][Station Adjust],begin");
            auto positionData = m_positionBaseptr->getPositionData(stationId);
            m_locationBaseptr->adjustData(positionData.kiloMeter, -1);
            _Kits::LogDebug("[Location Server][Station Adjust],end {}", positionData.kiloMeter);
            positionData.positionType = TIS_Info::LocationCommunication::PositionType::PT_STATION;
            WriteData2DB(positionData);
            sendPositionData(positionData);
        }
        
    }
    void LocationService::recvHandData(const QVariant &data)
    {
        if (!isDetection())
        {
            return;
        }
        QVariantMap mapData = data.toMap();
        // QString stationName=mapData.value("station_name", "").toString();
        QString poleName = mapData.value("pole_name", "").toString();
        double kilo_metor = -1;

        if (poleName.isEmpty())
        {
            return;
        }

        int ret = m_positionDataBaseptr->DataBaseQuery(poleName.toStdString(), m_currentTask.nLineDir, kilo_metor);
        if (ret == 0)
        {
            kilo_metor=m_lineLir*(kilo_metor-m_currentTask.dKilometer);
            m_locationBaseptr->adjustData(kilo_metor, -1);
        }
        auto positionData = std::move(getCurrentPositionData());
        positionData.positionType = TIS_Info::LocationCommunication::PositionType::PT_HAND;
        WriteData2DB(positionData);
        sendPositionData(positionData);
    }


    void LocationService::recvImageData(const TIS_Info::JiHeData &data)//todo 老版的只能杆号校正
    {
        static std::atomic_bool isAdjust = false;
        static float preKilometor = 0.0;
        if (!isDetection() || !m_openImgAdjust)
        {
            return;
        }
        _Kits::LogInfo("[Location Server][Image Adjust],begin");
        if (data.iImgPosType==TIS_Info::LocationCommunication::PositionType::PT_IMG && isAdjust==false)//开始校正
        {
            isAdjust = true;
            preKilometor =m_locationBaseptr->getDistance();
        }

        if (data.iImgPosType!=TIS_Info::LocationCommunication::PositionType::PT_IMG &&isAdjust ==true)//校正结束
        {
            isAdjust=false;
            float curKilometor=m_locationBaseptr->getDistance();
            float distance=std::fabs(curKilometor-preKilometor);
            if(distance>0.0001 &&distance < 0.0005)//在一定范围，要根据现场调整
            {

                TIS_Info::PositionData PositionData=getCurrentPositionData();
                double kilo_metor = -1;
                double span=PositionData.span;
                int ret = m_positionDataBaseptr->DataBaseQuery(PositionData.poleName.toStdString(), m_currentTask.nLineDir, kilo_metor);
                if (ret == 0 && abs(PositionData.kiloMeter-kilo_metor)<(0.3*span))//距离杆不能太远
                {
                    kilo_metor = m_lineLir*(kilo_metor - m_currentTask.dKilometer +curKilometor-preKilometor);
                    m_locationBaseptr->adjustData(kilo_metor, -1);

                    PositionData.positionType = TIS_Info::LocationCommunication::PositionType::PT_IMG;
                    WriteData2DB(PositionData);
                    sendPositionData(PositionData);
                    _Kits::LogInfo("[Location Server][Image Adjust],finish");
                }
            }

        }
    }

    void LocationService::recvMaoDuanData(const QVariant &data)//todo
    {
        if (!isDetection() || !m_openMaoDuanAdjust)
        {
            return;
        }
        auto positionData = getCurrentPositionData();
        double kilo_metor = -1;
        int ret = m_positionDataBaseptr->DataBaseQueryMaoDuan(positionData.kiloMeter, m_currentTask.nLineDir, kilo_metor);
        if (ret == 0 )
        {
            kilo_metor =m_lineLir*( kilo_metor - m_currentTask.dKilometer);
            m_locationBaseptr->adjustData(kilo_metor, -1);
        }

    }

    void LocationService::notifyTask(TIS_Info::TaskInfo data)
    {
        m_currentTask = data;
        if(m_currentTask.nLineDir==0)
        {
            m_lineLir=1;
        }
        if(m_currentTask.nLineDir==1)
        {
            m_lineLir=-1;
        }

        if (m_currentTask.nTaskstate == TIS_Info::EnumPublic::TASK_RUNNING) 
        {
            _Kits::LogDebug("[Location Server][Task start],taskstartstaion:{},startpole:{},Kilometer:{}",
                            m_currentTask.strStartStation.toStdString(),
                            m_currentTask.strStartPole.toStdString(),
                            m_currentTask.dKilometer);
            m_locationBaseptr->adjustData(0, 0);
            m_positionBaseptr->setTaskInfo(m_currentTask);

            //发送定位状态true
            sendLocationStatus(true);
        }
        else if (m_currentTask.nTaskstate == TIS_Info::EnumPublic::TASK_STOPPED) // 任务结束？
        {
            _Kits::LogDebug("[Location Server][Task stop]");
            m_locationBaseptr->adjustData(0, 0);
            m_positionBaseptr->setTaskInfo(m_currentTask);
            //发送定位状态false
            sendLocationStatus(false);
        }
        else // 任务暂停
        {
            _Kits::LogError("[Location Server][Task error]");
        }
    }

    TIS_Info::PositionData LocationService::getCurrentPositionData()
    {
        if (m_locationBaseptr != nullptr && m_positionBaseptr != nullptr)
        {
            double kilometer = 0, speed = 0, dis = 0;
            kilometer = m_lineLir *m_locationBaseptr->getDistance();
            speed = m_locationBaseptr->getSpeed();
            dis = m_locationBaseptr->getMoveDis();

            auto positionData = m_positionBaseptr->getPositionData(kilometer, speed);
            positionData.moveDistance = dis;
            return positionData;
        }
        return TIS_Info::PositionData();
    }

    void LocationService::sendPositionData(const TIS_Info::PositionData &posInfo)
    {
        QVariantMap PositionData1;
        PositionData1["stationName"] = posInfo.stationName;
        PositionData1["maoduanName"] = posInfo.maoduanName;
        PositionData1["poleName"] = posInfo.poleName;
        PositionData1["kiloMeter"] = posInfo.kiloMeter;
        PositionData1["speed"] = posInfo.speed;
        PositionData1["taskId"] = posInfo.taskId;
        PositionData1["lineId"] = posInfo.lineId;
        PositionData1["positionType"] = posInfo.positionType;
        PositionData1["moveDis"] = posInfo.moveDistance;
        //
        PositionData1["站区名"] = posInfo.stationName;
        PositionData1["锚段名"] = posInfo.maoduanName;
        PositionData1["杆号名"] = posInfo.poleName;
        PositionData1["公里标"] = posInfo.kiloMeter;
        PositionData1["速度"] = posInfo.speed;
        emit sendPositionData(PositionData1); // 给其他模块定时发送位置信息
    }

    void LocationService::WriteData2DB(TIS_Info::PositionData posInfo)
    {
        m_positionDataBaseptr->DataBaseInsert(posInfo);
    }

    void LocationService::run()
    {
        uint16_t sendNum=0;
        while (m_runThreadFlag)
        {
            if(!isDetection())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            auto positionData = getCurrentPositionData();
            m_triggerSaveDBptr->setcurDis(positionData.kiloMeter * 100000.0);
            if (isSaveOneData() || positionData.positionType== TIS_Info::LocationCommunication::PositionType::PT_DIST)
            {
                sendNum++;
                WriteData2DB(positionData);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }

            if (sendNum>8|| positionData.positionType== TIS_Info::LocationCommunication::PositionType::PT_DIST)
            {
                //数据库的频率是界面的8倍，极端条件发送频率10ms*8
                sendPositionData(positionData);
                sendNum=0;
            }

        }
    }

} // namespace _Modules
