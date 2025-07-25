#pragma once
#include "EnumPublic.h"
#include <QImage>
#include <QVariant>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <QDateTime>

namespace TIS_Info
{
    struct ImageBuffer
    {
        std::string name = "";
        std::string addr = ""; // ip or sn
        std::vector<uint8_t> data = {0};
        size_t width = 0;
        size_t height = 0;
        long timestamp = 0;
        int pixFormat = 0;

        ImageBuffer(size_t w, size_t h, size_t special) : width(w), height(h)
        {
            data.resize(w * h * special); // 简化，按需调整
        }

        bool clone(std::shared_ptr<ImageBuffer> new_buffer) const
        {
            // 复制所有成员变量
            new_buffer->name = name;
            new_buffer->data = data; // std::vector 的赋值操作是深拷贝
            new_buffer->width = width;
            new_buffer->height = height;
            new_buffer->timestamp = timestamp;
            new_buffer->pixFormat = pixFormat;

            return true;
        }
    };

    struct ImageInfo
    {
        std::string name;
        QImage image;
    };
    // 任务信息结构体
    struct TaskInfo
    {
        QString strTaskName = "";                        // 任务名称
        QString strCreateTime = "";                      // 创建时间
        QString strSubWayNum = "";                       // 列车编号
        QString strLineName = "";                        // 线路名称
        QString strStartStation = "";                    // 起始站
        QString strEndStation = "";                      // 终点站
        QString strStartPole = "";                       // 起始杆号
        double dKilometer = 0.0;                         // 公里标
        QString strTaskSavePath = "";                    // 任务路径
        int nLineDir = EnumPublic::TASK_UP_LINE;         // 0:上行 1:下行
        int nDirection = EnumPublic::TASK_FORWARD_TRAIN; // 0:正向 1:反向
        int nTaskstate = EnumPublic::TASK_STOPPED;       // 0:开始 1:结束
        int nTaskID = -1;
        void clear()
        {
            strTaskName = "";
            strCreateTime = "";
            strSubWayNum = "";
            strLineName = "";
            strStartStation = "";
            strEndStation = "";
            strStartPole = "";
            dKilometer = 0.0;
            nTaskstate = EnumPublic::TASK_STOPPED;
            nLineDir = EnumPublic::TASK_UP_LINE;
            nDirection = EnumPublic::TASK_FORWARD_TRAIN;
            nTaskID = -1;
        }
        bool isRun()
        {
            return nTaskstate == EnumPublic::TASK_RUNNING;
        }
    };

    struct PressureSpotData
    {
        float press[8] = {0.0f};
        float horAcc[2] = {0.0f}; // 横向
        float conAcc[2] = {0.0f}; // 前进
        float vidAcc[2] = {0.0f}; // 竖向
    };

    struct JiHeData
    {
        float fZig[4] = {8000, 8000, 8000, 8000};
        float fHei[4] = {8000, 8000, 8000, 8000};
        float fZigBc[4] = {8000, 8000, 8000, 8000}; //
        float fHeiBc[4] = {8000, 8000, 8000, 8000}; //
        float fMoHaoWidth[4] = {8000, 8000, 8000, 8000};
        float fMoHaoHeight[4] = {8000, 8000, 8000, 8000}; //
        float Diameter[4] = {8000, 8000, 8000, 8000};     //
        float fLeftBCX = 8000;
        float fLeftBCY = 8000;
        float fRightBCX = 8000;
        float fRightBCY = 8000;
        int lineNum = 0;
        int iImgPosType = 0; // 0:未知 1:导线 2:吊弦 3:支柱 5:锚 6：非支定位点
        int iTaskId = 0;
    };
    struct PositionData
    {
        uint32_t lineId = 0;
        uint32_t taskId = 0;
        QString stationName = "";
        uint32_t stationId = 0;
        QString maoduanName = "";
        QString poleName = "";
        double kiloMeter = -1.0;
        double moveDistance = -1.0;
        TIS_Info::LocationCommunication::PositionType positionType = TIS_Info::LocationCommunication::PositionType::PT_NONE;
        double speed = -1.0;
        double span = -1.0;
        bool isMaoDuan = false;
    };

    struct SpeedData
    {
        double mdble_speed;
        uint64_t mull_pulse;
        double mdble_kilometer;

        SpeedData operator+(const SpeedData &other)
        {
            return {mdble_speed + other.mdble_speed, mull_pulse + other.mull_pulse};
        }

        SpeedData operator/(int i)
        {
            return {mdble_speed / i, mull_pulse / i};
        }
    };

    struct HikLogInfo
    {
        std::string mstr_ipAddr;     // IP地址
        unsigned short mus_port;     // 端口号
        std::string mstr_userName;   // 用户名
        std::string mstr_password;   // 密码
        int mi_channel;              // 通道号
        std::string mstr_deviceName; // 设备名称
        std::string mstr_type;       // 设备类型
    };

    struct RecordInfo
    {
        QString mstr_record_path; // 录像路径
        bool mb_is_record;        // 是否录像
        QString mstr_camera_name; // 摄像头名称
    };

    struct DeviceSingleData
    {
      public:
        float mf_x;
        float mf_y;

        DeviceSingleData() = default;
        DeviceSingleData(float x, float y) : mf_x(x), mf_y(y)
        {
        }
        DeviceSingleData(const DeviceSingleData &data) : mf_x(data.mf_x), mf_y(data.mf_y)
        {
        }
    };

    struct IRImageBuffer
    {
        std::string name;
        std::vector<uint8_t> data;
        std::vector<uint8_t> tempdata;
        size_t iamgesize = 0;
        size_t width = 0;
        size_t height = 0;
        uint64_t timestamp = 0;
        float maxtemp = 0;
        float mintemp = 0;
        float avgtemp = 0;
        float maxpointx = 0;
        float maxpointy = 0;
        int pixFormat;

        IRImageBuffer(int imagesize, int tempsize)
        {
            data.resize(imagesize);
            tempdata.resize(tempsize);
        }

        bool clone(std::shared_ptr<IRImageBuffer> new_buffer) const
        {
            // 复制所有成员变量
            new_buffer->name = name;
            new_buffer->data = data; // std::vector 的赋值操作是深拷贝
            new_buffer->tempdata = tempdata;
            new_buffer->width = width;
            new_buffer->height = height;
            new_buffer->timestamp = timestamp;
            new_buffer->pixFormat = pixFormat;
            new_buffer->mintemp = mintemp;
            new_buffer->maxtemp = maxtemp;
            new_buffer->avgtemp = avgtemp;
            new_buffer->iamgesize = iamgesize;
            new_buffer->maxpointx = maxpointx;
            new_buffer->maxpointy = maxpointy;

            return true;
        }
    };
    struct DuagonCardData
    {
        bool m_bRead = false;                                                                 // 端口读写
        int m_portId = 0;                                                                     // 端口号
        int m_portSize = 0;                                                                   // 端口长度
        std::chrono::steady_clock::time_point m_nextClock = std::chrono::steady_clock::now(); // 下次执行时刻
        std::chrono::milliseconds m_interval;                                                 // 端口读写间隔
        std::vector<uint8_t> m_portData;                                                      // 端口数据
    };
    struct FileSystemData
    {
        std::string mstr_file_path;  // 文件路径
        uint64_t mull_file_size = 0; // 文件大小
        std::string m_ip;
        std::string m_port; // 文件传输的IP和端口
    };

    struct systemTime
    {
        int iYear;
        int iMonth;
        int iDay;
        int iHour;
        int iMinute;
        int iSecond;

        systemTime()
        {
            iYear = 0;
            iMonth = 0;
            iDay = 0;
            iHour = 0;
            iMinute = 0;
            iSecond = 0;
        }
        bool operator<(const systemTime &other) const
        {
            if (iYear != other.iYear)
                return iYear < other.iYear;
            if (iMonth != other.iMonth)
                return iMonth < other.iMonth;
            if (iDay != other.iDay)
                return iDay < other.iDay;
            if (iHour != other.iHour)
                return iHour < other.iHour;
            if (iMinute != other.iMinute)
                return iMinute < other.iMinute;
            return iSecond < other.iSecond;
        }

        bool operator>(const systemTime &other) const
        {
            if (iYear != other.iYear)
                return iYear > other.iYear;
            if (iMonth != other.iMonth)
                return iMonth > other.iMonth;
            if (iDay != other.iDay)
                return iDay > other.iDay;
            if (iHour != other.iHour)
                return iHour > other.iHour;
            if (iMinute != other.iMinute)
                return iMinute > other.iMinute;
            return iSecond > other.iSecond;
        }
    };

    // rfid_data适配老程序
    struct rfid_data
    {
        rfid_data()
        {
            iCount = 0;
            iItemLength = 0;
            strStation[0] = '\0';
            strMaoDuan[0] = '\0';
            strPole[0] = '\0';
            tTime = 0;
            strItem[0] = '\0';
        }

        int iCount;          // 计数
        int iItemLength;     // 条目长度
        time_t tTime;        // 时间
        char strStation[32]; // 站区
        char strMaoDuan[32]; // 锚段
        char strPole[32];    // 杆号
        char strItem[32];    // 条目
    };

    // mvb超限开关
    struct MvbAmifSwitch
    {
        bool temperature = false;  // 弓网接触线温度超限
        bool catOff = false;       // 拉出值超限
        bool catHei = false;       // 导高超限
        bool arc = false;          // 燃弧超限
        bool panStructure = false; // 受电弓结构超限
        bool bowSystem = false;    // 弓网系统
        bool abrval = false;       // 弓网接触线磨耗值超限
    };

    // mvb故障开关
    struct MvbAsatSwitch
    {
        bool geoparMode = false;           // 拉出值导高测量模块
        bool temperatureMode = false;      // 接触线温度测量模块
        bool arcMode = false;              // 燃弧模块
        bool pantoMode = false;            // 受电弓结构模块
        bool sysCommunicationMode = false; // 系统通信模块
        bool abrvalMode = false;           // 弓网接触线磨耗检测模块
    };

    struct RawData
    {
        uint64_t timestamp = 0;
        std::vector<uint8_t> data;
        RawData(int size)
        {
            data.resize(size);
        }

        bool clone(std::shared_ptr<RawData> new_buffer) const
        {
            // 复制所有成员变量
            new_buffer->timestamp = timestamp;
            new_buffer->data = data; // std::vector 的赋值操作是深拷贝
            return true;
        }
    };

    // trdp协议 header
    struct TrdpHeader
    {
        unsigned int sequenceCounter = 0;   // 序号计数器
        unsigned short protocolVersion = 0; // 协议版本
        unsigned short msgType = 0;         // 通信模式
        unsigned int comId = 0;             // 通信端口
        unsigned int etbTopoCnt = 0;        // 列车静态拓扑序列
        unsigned int opTrnTopoCnt = 0;      // 列车运行拓扑序列
        unsigned int datasetLength = 0;     // 应用数据长度
        unsigned int reserved01 = 0;        // 保留
        unsigned int replyComId = 0;        // 应答数据通信端口标识
        unsigned int replyIpAddress = 0;    // 应答数据通信ip地址
        unsigned int headerFcs = 0;         // 首部校验和
    };

    struct NvrDataInfo
    {
        long lChannel;       // 操作通道号
        QDateTime beginTime; // 开始时间
        QDateTime endTime;   // 结束时间
        QString strPath;     // 文件保存路径（包含文件名）
    };

    // liveNvr登录info
    struct LogInfo
    {
        QString mstr_ipAddr;     // IP地址
        unsigned short mus_port; // 端口号
        QString mstr_userName;   // 用户名
        QString mstr_password;   // 密码
    };

    struct _6017Info
    {
        std::unordered_map<QString, uint16_t> mmap_Channel_info;
        QString mstr_index = ""; // 6017设备索引
        QString mstr_ip = "";
        uint32_t mi_port = 0;
        uint16_t mui_interval = 0;
        bool mb_is_save_natural_data = false;
    };

    struct _6017NeturalData
    {
        int64_t mll_timestamp = 0;
        QByteArray mqb_data;

        _6017NeturalData(int64_t timestamp, QByteArray data) : mll_timestamp(timestamp), mqb_data(data)
        {
        }
    };

    struct arcData
    {
        std::string cameraName = "test";  // 相机名称
        std::string occurtime = "";       // 燃弧发生时间
        uint16_t arccount = 1;            // 燃弧总数
        uint16_t arctime = 1;             // 燃弧时间
        uint16_t arcpulse = 1;            // 燃弧脉冲
        QString fileName = "test";        // 燃弧文件名
        std::string arcDeviceIndex = "0"; // 燃弧设备索引
        uint16_t arcId = 0;               // 燃弧id
        uint64_t id = 0;                  // 数据库自增id
    };

    struct RemoteMessage
    {
        std::string topic;      // 话题 = Tis_info中的函数签名
        std::string message_id; // 消息流水号
        std::string json_data;
        std::vector<uint8_t> binary_data;
    };

} // namespace TIS_Info