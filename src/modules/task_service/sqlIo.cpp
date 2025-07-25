#include "sqlIo.h"
#include <set>

using namespace _Modules;
using namespace _Kits;

_Modules::sqlio::sqlio()
{
}

_Modules::sqlio::~sqlio()
{
}

bool _Modules::sqlio::selectlinename(std::vector<std::string> &line_name)
{
    QStringList line_name_list;
    line_name_list.push_back("line_name");
    line_name_list.push_back("line_dir");

    auto select = SqlSelect<_Kits::_Orm::line_data>().select(line_name_list).distinct("line_name");
    select.exec();
    auto res = std::move(select.getResults());

    for (auto &item : res)
    {
        auto select = SqlSelect<_Kits::_Orm::line_data>()
                          .select(line_name_list)
                          .where("line_name", _Kits::OperatorComparison::Equal, item.line_name)
                          .distinct("line_dir");

        if (select.exec())
        {
            auto results = select.getResults();
            if (results.empty())
            {
                continue;
            }

            if (results.size() > 1)
            {
                line_name.push_back(item.line_name.toStdString() + "_下行");
                line_name.push_back(item.line_name.toStdString() + "_上行");
                continue;
            }

            line_name.push_back(item.line_name.toStdString() + "_" + (results[0].line_dir == 0 ? "上行" : "下行"));
        }
    }

    return true;
}

bool _Modules::sqlio::selectlineByLineName(const QString &line_name, std::vector<_Kits::_Orm::line_data> &lineData)
{
    auto select = SqlSelect<_Kits::_Orm::line_data>()
                      .select(QStringList())
                      .where("line_name", _Kits::OperatorComparison::Equal, line_name)
                      .orderBy("id");

    if (!select.exec())
    {
        return false;
    }

    lineData = std::move(select.getResults());

    return true;
}

bool _Modules::sqlio::selectStationByLineName(const QString &line_name,
                                              const int &line_dir,
                                              std::vector<_Kits::_Orm::line_data> &stationData)
{
    auto select = SqlSelect<_Kits::_Orm::line_data>()
                      .select(QStringList{"station_name", "id"})
                      .where("line_name", _Kits::OperatorComparison::Equal, line_name)
                      .where("line_dir", _Kits::OperatorComparison::Equal, line_dir)
                      .distinct("station_name");

    if (!select.exec())
    {
        return false;
    }

    stationData = std::move(select.getResults());

    return true;
}

bool _Modules::sqlio::selectlineByStationNameLinedir(const QString &station_name,
                                                     const int &line_dir,
                                                     std::vector<_Kits::_Orm::line_data> &lineData)
{
    auto select = SqlSelect<_Kits::_Orm::line_data>()
                      .select(QStringList())
                      .where("station_name", _Kits::OperatorComparison::Equal, station_name)
                      .where("line_dir", _Kits::OperatorComparison::Equal, line_dir)
                      .orderBy("id");

    if (!select.exec())
    {
        return false;
    }

    lineData = std::move(select.getResults());

    return true;
}

int _Modules::sqlio::searchIdByTaskName(const QString &task_name)
{
    auto select = SqlSelect<_Kits::_Orm::task_data>()
                      .select(QStringList())
                      .where("task_name", _Kits::OperatorComparison::Equal, task_name)
                      .orderBy("id");

    if (!select.exec())
    {
        return -1;
    }

    auto res = select.getResults();

    if (res.empty())
    {
        return -1;
    }

    return res[0].id;
}

bool _Modules::sqlio::updataTaskTime(const QString &task_time, int task_id)
{
    auto update = SqlUpdate<_Kits::_Orm::task_data>()
                      .set("updated_time", task_time)
                      .set("end_time", task_time)
                      .where("id", _Kits::OperatorComparison::Equal, task_id);
    if (!update.exec())
    {
        return false;
    }

    return true;
}
