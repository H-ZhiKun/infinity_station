#pragma once
#include <string>
#include <yaml-cpp/yaml.h>
#include <qtmetamacros.h>
#include "kits/common/read_csv/ReadCSVFile.h"
#include <memory>
#include <map>
#include <array>
#include <list>
#include <atomic>
#include "tis_global/Struct.h"
#include "kits/common/database/CppBatis.h"
#include "sqlIo.h"

/***************************************************************************
 * @file    PreviousTaskCtrl.h
 * @brief   任务控制创建类
 *
 *
 *
 * @note
 ***************************************************************************/

namespace _Modules
{

    struct StationInfo
    {
        int nStationID = 0;                     // i站ID
        std::string strStationName = "";        // 站名
        std::string strEndStationName_UP = "";  //	【上行】终点站名
        std::string strEndStationName_DN = "";  //	【下行】终点站名
        std::string strPole_UP = "";            //	【上行】时站对应的杆号
        std::string strPole_DN = "";            //	【下行】时站对应的杆号
        std::string strNextStationName_UP = ""; //	【上行】下一站
        std::string strNextStationName_DN = ""; //	【下行】下一站
        std::string strPreStationName_UP = "";  //	【上行】上一站
        std::string strPreStationName_DN = "";  //	【下行】上一站
    };

    struct LineDataBaseInfo
    {

        int id = 0;
        std::string strLineName = "";    // 线路
        std::string strSationName = "";  // 站区
        std::string strTunnelName = "";  // 隧道名称
        std::string strMaoduanName = ""; // 锚段
        std::string strPoleName = "";    // 杆号
        float fPoleDistance = 0;         // 跨距
        int nStructure = 0;              // 结构
        float fTrainKilometer = 0;       // 公里标
        int fZig = 0;                    // 拉出值
        int fHei = 0;                    // 导高
        int nLineDir = 0;                // 线路方向
        int nLineType = 0;               // 线路类型：柔性还是刚性
        std::string sOther = "";         // 其它
        double fGpsLatitude = 0;         // GPS纬度
        double fGpsLongitude = 0;        // GPS经度
        // int iStationOnlineId = 0;//当前杆
        // int iNextStationOnlineId = 0;//下一杆
        // bool bOnlineStopPole = 0;//在线检测停止杆
    };

    struct LindeDataInfo
    {
        std::string strLineName = "";
        bool bLineDirection = false;
        LineDataBaseInfo lineDataBaseInfo;
    };

    struct OnlineTask
    {
        std::string strSubWayNum = "";
        std::string strLineName = "";
        std::string strDirection = "";
        std::string strStartStation = "";
        std::string strEndStation = "";
        std::string strStartPole = "";
        bool bOnlineTask = false;
        int nLineDir = 0;
    };

    const std::unordered_map<std::string, int> lineSignList = {{"锚段关节", 1},
                                                               {"中心柱", 2},
                                                               {"分向标识", 3},
                                                               {"分段标识", 4},
                                                               {"保留", 5},
                                                               {"定位点", 6},
                                                               {"线岔", 7},
                                                               {"膨胀元件", 8},
                                                               {"中心锚结", 9}};

    const std::unordered_map<std::string, int> lineTypeList = {
        {"刚性", 0},
        {"柔性单支", 1},
        {"柔性", 2},
        {"柔性双支", 3},
        {"刚柔过渡", 4},
        {"上接触三轨", 5},
        {"下接触三轨", 6},
    };

    class PreviousTaskCtrl
    {
      private:
        /* data */
      public:
        PreviousTaskCtrl(/* args */);
        ~PreviousTaskCtrl();
        /**********************
         * @brief   初始化
         * @param   config    配置文件
         * @return
         ************************ */
        void initConfigInfo(const YAML::Node &config);
        /**********************
         * @brief   处理任务数据
         * @param   data     状态数据
         * @param   taskData 任务数据
         * @return  0 表示成功
         ************************ */
        int createTask(const QVariant &data, TIS_Info::TaskInfo &taskData);

      private:
        /**********************
         * @brief  写入linedata数据
         * @param   LineName
         * @return  0 表示成功
         ************************ */
        int insertLineData();
        /**********************
         * @brief 读取站区信息数据
         * @param   filePath    站区数据文件路径
         * @return  0 表示成功
         ************************ */
        int readStationInfo(const std::string &filePath);
        /**********************
         * @brief 读取linedata数据
         * @param   filePath    linedata文件路径
         * @return  0 表示成功
         ************************ */
        int readLineDataInfo(const std::string &filePath);
        /**********************
         * @brief 读取在线任务消息
         * @param   filePath    在线任务战区信息路径
         * @return  0 表示成功
         ************************ */
        int readOnlineTask(const std::string &filePath);
        /**********************
         * @brief 创建db数据
         * @param   taskinfo    任务信息
         * @return  0 表示成功
         ************************ */
        int createTaskDataBase();
        /**********************
         * @brief 创建在线任务
         * @param   data    线路站点数据
         * @param   taskData 任务数据
         * @return  0 表示成功
         ************************ */
        int createOnlineTask(const QVariant &data, TIS_Info::TaskInfo &taskData);
        /**********************
         * @brief 手动创建任务
         * @param   data    线路站点数据
         * @param   taskData 任务数据
         * @return 0 表示成功
         ************************ */
        int createTestTaskData(const QVariant &data, TIS_Info::TaskInfo &taskData);

        /**********************
         * @brief 开始任务
         * @param
         * @return 0 表示成功
         ************************ */
        int startTask();
        /**********************
         * @brief 停止任务
         * @param
         * @return
         ************************ */
        void stopTask();
        std::map<std::string, std::list<LineDataBaseInfo>> m_lineDataList; // linedata数据
        std::string m_SavePath = "d:/gwkj/";                               // 文件保存路径
        std::unordered_map<int, StationInfo> m_stationInfoList;            // 站区数据
        std::array<OnlineTask, 2> m_arrOnlineTask;                         // 在线任务数据
        TIS_Info::TaskInfo m_currentTask;                                  // 当前任务数据
        std::atomic<bool> m_bStartTaskFlag = false;                        // 任务开始标志
        _Kits::_Orm::task_data m_taskData;                                 // 任务sql数据
    };

} // namespace _Modules