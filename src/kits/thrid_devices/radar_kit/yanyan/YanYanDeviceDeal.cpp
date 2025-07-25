
#include "YanyanDeviceDeal.h"
#include <thread>
#include <chrono>
#include "zeroMQ/zmq.h"
using namespace _Modules;
using namespace _Kits;
// #define MAX_RECEIVE_BUFFER 100000000
#define MAX_RECEIVE_BUFFER 100000

YanYanDeviceDeal::YanYanDeviceDeal()
    : m_bConnected(false), m_dBandWidth(0.0), m_dReceiveTime(0.0),
      m_YYResultBufferPools(std::make_shared<_Kits::ObjectPool<DeviceSurfaceData>>())
{

    // m_Revthread = std::thread(&YanYanDeviceDeal::Run, this);
    m_consumerThread = std::thread(&YanYanDeviceDeal::consumerThreadFunc, this);
}
YanYanDeviceDeal::~YanYanDeviceDeal()
{
    close();
}

void YanYanDeviceDeal::close()
{
    m_bExitThread.store(true);      // 接收数据线程退出
    stopConsumerThread.store(true); // 消费者线程退出
    m_cond_var.notify_all();

    if (m_Revthread.joinable())
        m_Revthread.join();
    if (m_consumerThread.joinable())
        m_consumerThread.join();

    if (m_pSocketRecv)
    {
        zmq_close(m_pSocketRecv);
        m_pSocketRecv = nullptr;
    }

    if (m_pZmq)
    {
        zmq_ctx_destroy(m_pZmq);
        m_pZmq = nullptr;
    }
}
bool YanYanDeviceDeal::Init(const YAML::Node &config)
{
    m_bExitThread.store(false);
    stopConsumerThread.store(false);

    m_config = config;
    bool bRet = false;
    int iRet = 0;
    if (m_config["ip"])
        m_device_config.mstr_ip = config["ip"].as<std::string>();
    if (m_config["port"])
        m_device_config.mus_port = config["port"].as<unsigned short>();
    if (m_config["device_index"])
        m_device_config.mus_device_index = config["device_index"].as<unsigned short>();
    // 设置超时
    if (config["timeOut"])
        m_iTimeOut = config["timeOut"].as<int>();

    m_pZmq = zmq_ctx_new();

    if (nullptr == m_pZmq)
    {
        _Kits::LogInfo("YYinit m_pZmq zmq_ctx_new error");
        return false;
    }

    // zmq初始化
    m_pSocketRecv = zmq_socket(m_pZmq, ZMQ_PULL);

    if (m_pSocketRecv == nullptr)
    {
        zmq_ctx_destroy(m_pZmq);
        m_pZmq = nullptr;
        _Kits::LogInfo("YYinit m_pSocketRecv zmq_socket error");
        return false;
    }
    iRet = zmq_setsockopt(m_pSocketRecv, ZMQ_RCVTIMEO, &m_iTimeOut, sizeof(m_iTimeOut));
    if (0 > iRet)
    {
        char strError[256] = {0};
        sprintf(strError, "zmq set time out error:%s", zmq_strerror(zmq_errno()));

        // m_strError = strError;
        _Kits::LogInfo("YYinit iRet zmq_setsockopt error:{}", strError);

        return false;
    }
    zmq_setsockopt(m_pSocketRecv, ZMQ_RCVHWM, &m_iRecvHw, sizeof(m_iRecvHw));
    char strConnectStr[256] = {0};

    sprintf(strConnectStr, "tcp://%s:%d", m_device_config.mstr_ip.data(), m_device_config.mus_port);

    if (zmq_connect(m_pSocketRecv, strConnectStr) != 0)
    {
        zmq_close(m_pSocketRecv);
        m_pSocketRecv = nullptr;
        zmq_ctx_destroy(m_pZmq);
        m_pZmq = nullptr;
        return false;
    }

    _Kits::LogInfo("YYinit  zmq_connect strConnectStr:{}", strConnectStr);

    if (0 == iRet)
    {
        m_bConnected = true;
        mb_device_state = true; // 设备状态
        bRet = true;
        m_Revthread = std::thread(&YanYanDeviceDeal::Run, this);
    }
    else
    {
        _Kits::LogError("YYinit  zmq_connect iRet:{}", iRet);
        mb_device_state = false;
        m_bConnected = false;
        return false;
    }

    return bRet;
}
void YanYanDeviceDeal::Run()
{

    int iRet = -1;
    std::vector<char> pReceiveBuffer(MAX_RECEIVE_BUFFER);
    if (NULL == m_pSocketRecv)
    {
        _Kits::LogInfo("YYrev  m_pSocketRecv error:{}", "NULL == m_pSocketRecv");
        return;
    }

    m_dMaxReceiveTimeSecond = -99999.0;

    while (!m_bExitThread.load() && nullptr != m_pSocketRecv)
    {
        try
        {
            if (m_bConnected) // 已经连接
            {
                iRet = zmq_recv(m_pSocketRecv, pReceiveBuffer.data(), MAX_RECEIVE_BUFFER, 0);

                if (0 > iRet)
                {
                    char strError[256] = {0};
                    int ierrorNo = zmq_errno();
                    m_strError = strError;
                    mb_device_state = false;
                }
                else
                {
                    DataReceiveDeal(pReceiveBuffer.data(), iRet);
                    std::this_thread::sleep_for(std::chrono::milliseconds(35));
                }
            }
            else // 重新进行连接
            {
                char strConnectStr[256] = {0};
                sprintf(strConnectStr, "tcp://%s:%d", m_device_config.mstr_ip.data(), m_device_config.mus_port);
                iRet = zmq_connect(m_pSocketRecv, strConnectStr);
                _Kits::LogInfo("reconnect  zmq_recv conncttcp:{}", strConnectStr);
                if (0 == iRet)
                {
                    m_bConnected = true;
                    mb_device_state = true;
                }
            }
        }
        catch (...)
        {
            _Kits::LogError("YYrev  m_pSocketRecv error:{}", "catch...");
        }
    }
}
void YanYanDeviceDeal::DataReceiveDeal(void *pData, int iDataLength)
{
    if (iDataLength <= 0)
    {
        return;
    }

    std::vector<char> buffer;
    buffer.reserve(iDataLength); // 预分配内存
    buffer.assign(static_cast<char *>(pData), static_cast<char *>(pData) + iDataLength);

    {
        std::lock_guard<std::mutex> lock(m_char_mutex);
        m_char_data.push_back(std::move(buffer)); // 使用移动语义
    }
    m_cond_var.notify_one();
    // 通知消费者线程有新数据可用
}

void YanYanDeviceDeal::consumerThreadFunc()
{
    while (!stopConsumerThread.load())
    {
        std::unique_lock<std::mutex> lock(m_char_mutex);
        m_cond_var.wait(lock, [this] { return !m_char_data.empty() || stopConsumerThread.load(); });
        if (stopConsumerThread.load())
        {
            break;
        }

        auto data = m_char_data.front();
        m_char_data.pop_front();
        // 处理数据
        lock.unlock(); // 解锁以允许其他线程访问 m_char_data
        DecodeFromString(data);
    }
}
void YanYanDeviceDeal::DecodeFromString(const std::vector<char> &strRead)
{
    if (strRead.empty())
    {
        LogInfo("数据为空");
        return;
    }

    // 解析雷达数据的逻辑
    const char *ptr = strRead.data();

    static int i_count = 0;
    if (i_count++ % 1000 == 0)
    {
        _Kits::LogInfo("YYDecodeFromString strRead size:{}", strRead.size());
    }
    CYY3dResultStruct m_YYResult;
    // 最小包 长度判断
    auto headerSize = sizeof(m_YYResult.m_ui64DevTime) + sizeof(m_YYResult.m_ft) + sizeof(m_YYResult.m_uiFrameNo) +
                      sizeof(m_YYResult.m_iCameraID) + sizeof(m_YYResult.m_iDataType);
    if (strRead.size() < headerSize)
    {
        //_Kits::LogInfo("YYDecodeFromString strRead.size() < headerSize");
        return;
    }

    ptr += headerSize;

    auto data = m_YYResultBufferPools->getObject(7600); // 从对象池获得数据对象
    data->mvec_data_surface.clear();                    // 清空之前的数据
    size_t remainingBytes = strRead.size() - (ptr - strRead.data());
    size_t pointsCount = remainingBytes / sizeof(YYPOINT_2F);

    //_Kits::LogInfo("YYDecodeFromString data:{}", "process in DecodeFromString");
    for (size_t i = 0; i < pointsCount; ++i)
    {
        TIS_Info::DeviceSingleData singlePoint;

        memcpy(&singlePoint, ptr, sizeof(TIS_Info::DeviceSingleData));
        ptr += sizeof(TIS_Info::DeviceSingleData);

        if (singlePoint.mf_x == 0.0f && singlePoint.mf_y == 0.0f)
        {
            continue; // 跳过无效点
        }

        // 然后将解析后的数据存储到 mvec_one_point 中
        data->mvec_data_surface.push_back(singlePoint);
    }
    // 将数据对象放入队列
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);

        static int i_count = -1;
        if (++i_count % 400 == 0)
        {
            LogDebug("ip{}, PointSize:{}", m_device_config.mstr_ip, std::to_string(data->mvec_data_surface.size()));
        }

        m_data_queue.push_back(data);

        while (m_data_queue.size() > 5)
        {
            m_data_queue.pop_front();
        }

        emit deviceDataGet(m_device_config.mus_device_index);
    }
}

void _Modules::YanYanDeviceDeal::reLoad()
{
    close();

    m_bExitThread.store(false);
    stopConsumerThread.store(false);

    // zmq初始化
    m_pSocketRecv = zmq_socket(m_pZmq, ZMQ_PULL);

    if (m_pSocketRecv == nullptr)
    {
        zmq_ctx_destroy(m_pZmq);
        m_pZmq = nullptr;
        _Kits::LogInfo("YYinit m_pSocketRecv zmq_socket error");
        return;
    }
    int iRet = zmq_setsockopt(m_pSocketRecv, ZMQ_RCVTIMEO, &m_iTimeOut, sizeof(m_iTimeOut));
    if (0 > iRet)
    {
        char strError[256] = {0};
        sprintf(strError, "zmq set time out error:%s", zmq_strerror(zmq_errno()));

        // m_strError = strError;
        _Kits::LogInfo("YYinit iRet zmq_setsockopt error:{}", strError);

        return;
    }
    zmq_setsockopt(m_pSocketRecv, ZMQ_RCVHWM, &m_iRecvHw, sizeof(m_iRecvHw));
    char strConnectStr[256] = {0};

    sprintf(strConnectStr, "tcp://%s:%d", m_device_config.mstr_ip.data(), m_device_config.mus_port);

    if (zmq_connect(m_pSocketRecv, strConnectStr) != 0)
    {
        zmq_close(m_pSocketRecv);
        m_pSocketRecv = nullptr;
        zmq_ctx_destroy(m_pZmq);
        m_pZmq = nullptr;
        return;
    }

    _Kits::LogInfo("YYinit  zmq_connect strConnectStr:{}", strConnectStr);

    if (0 == iRet)
    {
        m_bConnected = true;
        mb_device_state = true; // 设备状态
        m_Revthread = std::thread(&YanYanDeviceDeal::Run, this);
    }
    else
    {
        _Kits::LogError("YYinit  zmq_connect iRet:{}", iRet);
        mb_device_state = false;
        m_bConnected = false;
        return;
    }
}

_Kits::DeviceSurfaceData YanYanDeviceDeal::GetDeviceData()
{
    _Kits::DeviceSurfaceData data;
    if (!m_data_queue.empty())
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        data = *m_data_queue.front();
        m_data_queue.pop_front();
    }
    else
    {
        LogError("GetDeviceData is empty, 真的是空的");
        return {};
    }

    static int i_count = 0;
    if (i_count++ % 1000 == 0)
    {
        LogInfo("YanYanDeviceDeal::GetDeviceData()");
    }

    return data;
}

_Kits::Err YanYanDeviceDeal::LastErrorCode()
{
    return _Kits::Err(0, m_strError);
}
bool YanYanDeviceDeal::StartDetect()
{
    return true;
}
bool YanYanDeviceDeal::StopDetect()
{
    return true;
}
void YanYanDeviceDeal::ReloadDevice()
{
    // 重新加载设备的逻辑
    // 例如，重新连接、重新初始化等
}

void YanYanDeviceDeal::ChangeCenterPoint(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData,
                                         const ChangeCenterData &change_center_data)
{
}
void YanYanDeviceDeal::SkewDeviceData(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees)
{
}
float YanYanDeviceDeal::IsDeviceSkew(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees)
{
    return 0.0f;
}
std::vector<TIS_Info::DeviceSingleData> YanYanDeviceDeal::GetParallelData(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                                                          float &height)
{
    return std::vector<TIS_Info::DeviceSingleData>();
}
void YanYanDeviceDeal::CompensateData()
{
}
void YanYanDeviceDeal::LoginDevice()
{
}