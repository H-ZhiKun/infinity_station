#include "TaskService.h"
#include "kits/common/log/CRossLogger.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <codecvt>
#include <functional>
#include <json/value.h>
#include <locale>
#include <qobject.h>
#include <qtimer.h>
#include <algorithm>

// #include "UniqueIDTime.h"
// using namespace _Modules;

using namespace _Kits;
namespace _Modules
{

    TaskService::TaskService(QObject *parent) : m_sqlio(std::make_unique<sqlio>())
    {
        m_startFlag.store(false);
    }

    TaskService::~TaskService() noexcept
    {
    }

    bool TaskService::init(const YAML::Node &config)
    {
        if (m_startFlag.load())
        {
            return false;
        }

        m_taskCtrlptr = std::make_unique<PreviousTaskCtrl>();

        if (nullptr != m_taskCtrlptr)
        {
            m_taskCtrlptr->initConfigInfo(config);
        }

        // initUniqueTimer();
        m_startFlag.store(true);
        return true;
    }
    bool TaskService::start()
    {
        return true;
    }
    bool TaskService::stop()
    {
        return true;
    }

    void TaskService::recvMBLKData(const QVariant &data)
    {
        TIS_Info::TaskInfo taskinfodata;

        if (nullptr != m_taskCtrlptr)
        {
            if (m_taskCtrlptr->createTask(data, taskinfodata) != 0)
            {
                LogError("createTask error");
                return;
            }

            emit notifyTaskData(taskinfodata);
            emit sendTaskNameToQml(taskinfodata.strTaskName); // 发送任务名称到QML
        }
    }

    void TaskService::onRecvTaskChooseSig(const QVariant &ChooseSig)
    {
        std::vector<std::string> line_name;

        if (!m_sqlio->selectlinename(line_name))
        {
            qDebug() << "selectlinename error";
            return;
        }

        // 转换为 QStringList
        QStringList qlineNames;
        for (const auto &name : line_name)
        {
            QString qname = QString::fromUtf8(name.c_str());
            qlineNames.append(qname);
        }

        // 发送 QStringList
        emit sendLineName(QVariant::fromValue(qlineNames));
    }

    void TaskService::recvStationName(const QString &station_name, const int &line_dir)
    {
        std::vector<_Kits::_Orm::line_data> staionData;

        if (!m_sqlio->selectlineByStationNameLinedir(station_name, line_dir, staionData))
        {
            LogError("selectlineByStationName error for station: %s", station_name.toStdString().c_str());
            return;
        }

        convertLinedata(staionData); // 对外的json信号
    }

    bool TaskService::convertLinedata(const std::vector<_Kits::_Orm::line_data> &line_info)
    {
        Json::Value root(Json::arrayValue);
        std::map<std::string, Json::Value> stationMap;
        std::vector<std::string> stationOrder;
        std::string currentStation;

        for (const auto &data : line_info)
        {
            std::string stationKey = data.station_name.toStdString();

            Json::Value item;
            item["line_name"] = data.line_name.toStdString();
            item["maoduan_name"] = data.maoduan_name.toStdString();
            item["kilo_meter"] = data.kilo_meter;
            item["pole_name"] = data.pole_name.toStdString();
            item["line_dir"] = data.line_dir;

            if (stationMap.find(stationKey) == stationMap.end())
            {
                if (!currentStation.empty())
                {
                    Json::Value stationObj;
                    stationObj["station_name"] = currentStation;
                    stationObj["items"] = stationMap[currentStation];
                    emit sendLineData(stationObj);
                }

                currentStation = stationKey;
                stationMap[stationKey] = Json::arrayValue;
                stationOrder.push_back(stationKey);
            }
            stationMap[stationKey].append(std::move(item));
        }

        // 处理最后一个站区数据
        if (!currentStation.empty() && !stationMap[currentStation].empty())
        {
            Json::Value stationObj;
            stationObj["station_name"] = currentStation;
            stationObj["items"] = stationMap[currentStation];
            emit sendLineData(stationObj);

            // 清空当前站区标记
            currentStation.clear();
        }

        return true;
    }
    void TaskService::sortLineDataById(std::vector<_Kits::_Orm::line_data> &it)
    {
        std::sort(it.begin(), it.end(), [](const _Kits::_Orm::line_data &a, const _Kits::_Orm::line_data &b) {
            return a.id < b.id; // 升序排列
        });
    }

    void TaskService::OnrecvTaskName(const QString task_name)
    {
        QStringList parts = task_name.split("_"); // 按下划线分割

        QString stationPrefix = parts[0]; // "太原s1"
        QString direction = parts[1];     // "上行"
        int ndirection = direction == "上行" ? 0 : 1;

        std::vector<_Kits::_Orm::line_data> stationData;
        stationData.reserve(64);

        m_sqlio->selectStationByLineName(stationPrefix, ndirection, stationData);

        sortLineDataById(stationData);

        for (const auto &station : stationData)
        {
            recvStationName(station.station_name, ndirection);
        }
    }

} // namespace _Modules