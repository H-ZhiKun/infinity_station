#include "MVBService.h"
#include "MVBAbstractFactory.h"
#include "kits/mvb_communication/src/MvbBase.h"
#include "kits/mvb_communication/src/MvbFactory.h"
#include "kits/common/log/CRossLogger.h"
#include <filesystem>
#include <functional>
#include <json/value.h>
#include <qobject.h>
#include <qtimer.h>

// using namespace _Modules;

using namespace _Kits;
namespace _Modules
{

    MVBService::MVBService(QObject *parent)
    {
        m_startFlag.store(false);
        MVBAbstractFactory factory;
        m_mvbParseMsgptr = factory.createParseMsg();
        // m_mvbMsgHandlePtr = factory.createMsgHandle();
        m_mvbPmsDataGernerator = std::make_unique<MVBPmsDataGenerator>(factory.createDataGenerator());
    }

    MVBService::~MVBService() noexcept
    {
    }

    bool MVBService::start(const YAML::Node &config)
    {
        if (m_startFlag.load())
        {
            return false;
        }

        auto configPath = config["config_path"].as<std::string>();

        if (configPath.empty())
        {
            // std::cerr << "No configuration path found." << std::endl;
            return false;
        }
        YAML::Node config_node = YAML::LoadFile(std::filesystem::current_path().string() + configPath);

        int type = config_node["mvb_type"].as<int>();
        if (nullptr == m_mvbBaseptr)
        {
            MvbFactory mvbFactory;
            m_mvbBaseptr = mvbFactory.createMvbBase(type);
        }

        auto ret = m_mvbBaseptr->init(config_node);
        if (0 == ret)
        {

            auto recvCallback = std::bind(&MVBService::recvMVBData, this, std::placeholders::_1);
            m_mvbBaseptr->setCallBack(recvCallback);
            m_mvbBaseptr->start();
        }

        if (nullptr != m_mvbPmsDataGernerator)
        {
            m_mvbPmsDataGernerator->initConfigInfo(configPath);
        }

        initTimer(config_node);
        m_startFlag.store(true);
        return true;
    }

    bool MVBService::stop()
    {
        if (nullptr == m_mvbBaseptr)
        {
            return false;
        }
        m_mvbBaseptr->stop();
        return true;
    }

    void MVBService::recvMVBData(const MvbMsgData &mvbMsgData)
    {
        // 处理mvb消息
        if (nullptr != m_mvbParseMsgptr)
        {
            QVariant parseData;
            m_mvbParseMsgptr->parseMsg(mvbMsgData.data, mvbMsgData.type, parseData);
            emit updateData(parseData);
        }
    }

    bool MVBService::sendData(const std::string &addressIP, int port, const std::string &data)
    {
        if (nullptr == m_mvbBaseptr)
        {
            LogError("m_mvbBaseptr is nullprt");
            return false;
        }
        auto ret = m_mvbBaseptr->sendData(data, data.length(), addressIP, port);

        // Iterate through each byte in the string

        if (0 != ret)
        {
            std::ostringstream oss;
            for (unsigned char c : data)
            {
                // Format each byte as a two-digit hex number (0x00 to 0xFF)
                oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
            }
            LogError("sendData data:{},len:{},addressIP:{},port:{} faild:{}", oss.str(), data.length(), addressIP, port, ret);
            return false;
        }

        std::ostringstream oss;
        for (unsigned char c : data)
        {
            // Format each byte as a two-digit hex number (0x00 to 0xFF)
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        }
        LogDebug("sendData data:{},len:{},addressIP:{},port:{} success", oss.str(), data.length(), addressIP, port);
        return true;
    }

    void MVBService::updateModulStatus(const QVariant &data)
    {
        if (nullptr == m_mvbPmsDataGernerator)
        {
            LogError("mvbPmsDataGernerator is nullprt");
            return;
        }
        auto datamap = data.toMap();
        int modulno = datamap.value("modulno", -1).toInt();
        int value = datamap.value("value", -1).toInt();
        LogDebug("setModulStatus modulno:{},value:{}", modulno, value);
        m_mvbPmsDataGernerator->setModulstatusInfo(modulno, value);
    }

    void MVBService::updateAlarmInfo(const QVariant &data)
    {
        if (nullptr == m_mvbPmsDataGernerator)
        {
            LogError("mvbPmsDataGernerator is nullprt");
            return;
        }

        auto datamap = data.toMap();
        int armno = datamap.value("armno", -1).toInt();
        int alarmlevel = datamap.value("alarmlevel", -1).toInt();
        LogDebug("setAlarmInfo modulno:{},value:{}", armno, alarmlevel);
        m_mvbPmsDataGernerator->setAlarmInfo(armno, alarmlevel);
    }

    void MVBService::initTimer(const YAML::Node &config)
    {

        for (auto portinfo : config["portinfo"])
        {
            int type = portinfo["type"].as<int>();
            if (0 == type)
            {
                int port = portinfo["port"].as<int>();
                std::string addressIP = portinfo["ip"].as<std::string>();
                int interval = portinfo["interval"].as<int>();
                auto qtimer = new QTimer(nullptr);
                qtimer->setInterval(interval);
                // connect(qtimer, &QTimer::timeout, this,
                // &MVBService::heartbeatTimer);

                connect(qtimer, &QTimer::timeout, [this, addressIP, port]() {
                    // 这里可以调用一个函数，并传递参数
                    if (nullptr == m_mvbPmsDataGernerator)
                    {
                        LogError("mvbPmsDataGernerator is nullprt");
                        return;
                    }
                    auto strdata = m_mvbPmsDataGernerator->getPmsData();
                    sendData(addressIP, port, strdata);
                    m_mvbPmsDataGernerator->decreaseModulesAliveTime();
                });
                // 启动定时器
                qtimer->start();
            }
        }
    }
    void MVBService::notifyUniqueTime(const QVariant &data)
    {
        auto mapdata = data.toMap();
        int64_t llFileTime = mapdata.value("llfiletime", 0).toLongLong();
        double dTimeStamp = mapdata.value("dtimestamp", 0.0).toDouble();
    }

} // namespace _Modules