#include "ConnectivityManager6051.h"
#include "speed6051Net.h"
#include <algorithm>
#include <QThread>
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;
ConnectivityManager6051::ConnectivityManager6051(QObject *parent) : ConnectivityManagerBase(parent)
{
    mvec_lastpulse.reserve(10);
    mvec_nowpulse.reserve(10);

    mvec_lastpulse = {0};
    mvec_nowpulse = {0};

    mvec_nowpulse.push_back(0);
}

ConnectivityManager6051::~ConnectivityManager6051()
{
    stop();
}

bool ConnectivityManager6051::init(const YAML::Node &config)
{
    m_ip = config["ip"].as<std::string>().c_str();
    m_port = config["port"].as<int>();
    mb_isSimulation = config["simulation"].as<bool>(false);
    mi_simulationPulse = config["simulation_pulse_add"].as<int>(0);

    mp_tcp_client = std::make_unique<TcpClient>(m_ip, m_port);
    m_Speed = std::make_unique<speed6051Net>();

    mus_wheel_pusle = config["wheel_pulse"].as<uint16_t>();
    md_wheel_diameter = config["wheel_diameter"].as<float>();

    mll_time_interval = config["time_interval"].as<long long>(100);

    m_elapsedTimer.start();
    mb_IsStop.store(false);

    return true;
}

bool ConnectivityManager6051::start()
{
    mp_tcp_client->start();
    m_thread_speedCalc = std::thread([this]() {
        while (!mb_IsStop.load())
        {
            mp_tcp_client->sendData(QByteArray(reinterpret_cast<const char *>(CMD_GETPULSE), sizeof(CMD_GETPULSE)));
            // 在函数调用前添加计时
            // auto start = std::chrono::high_resolution_clock::now();
            this->sendSpeedData();
            // auto end = std::chrono::high_resolution_clock::now();

            // 计算并输出耗时（毫秒）
            // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            //_Kits::LogInfo("sendSpeedData() execution time: {} ms", duration.count());

            std::this_thread::sleep_for(std::chrono::milliseconds(mll_time_interval));
        }
    });

    connect(mp_tcp_client.get(), &TcpClient::recvData, this, &ConnectivityManager6051::onRecvData);
    return true;
}

bool ConnectivityManager6051::stop()
{
    mb_IsStop.store(true);
    if (m_thread_speedCalc.joinable())
    {
        m_thread_speedCalc.join();
    }

    return true;
}

QString _Kits::ConnectivityManager6051::getName() const
{
    return "6051";
}

void ConnectivityManager6051::onRecvData(const QByteArray &data)
{
    if (mb_IsStop.load())
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex_recvArray);
        mvec_qbt_recvArray.push_back(data);
    }

    m_cond_recvArray.notify_one();
}

void ConnectivityManager6051::onControlSingleDo(const QVariant &data)
{
}

void ConnectivityManager6051::onRecvYYJiheTrigger()
{
    // 设置开启命令
    m_sSendData = "00 00 00 00 00 06 01 05 00 10 ff 00";
    GetSendData();
    memcpy(m_arraySendStart, m_pSendData.data(), 12);

    // 设置关闭命令
    m_sSendData = "00 00 00 00 00 06 01 05 00 10 00 00";
    GetSendData();
    memcpy(m_arraySendClose, m_pSendData.data(), 12);

    mp_tcp_client->sendData(QByteArray(m_arraySendClose, 12));

    QThread::msleep(5); // 睡眠5毫秒

    mp_tcp_client->sendData(QByteArray(m_arraySendStart, 12));
}

void _Kits::ConnectivityManager6051::onisSaveNaturalData(bool isSaveNaturalData)
{
    _mb_isSaveNaturalData = isSaveNaturalData;
}

void ConnectivityManager6051::GetSendData()
{
    m_sSendData = m_sSendData.trimmed();

    QByteArray hexBytes = m_sSendData.toLatin1();
    const char *pBuf = hexBytes.constData();

    m_pSendData.clear();
    m_dwHexDataLen = 0;

    uint i = 0;
    while (*pBuf)
    {
        if (i == 0)
        {
            if (*pBuf == ' ')
            {
                pBuf++;
                continue;
            }

            m_pSendData.append(hexCharToValue(*pBuf));
            m_dwHexDataLen++;
            i++;
            pBuf++;
        }
        else
        {
            if (*pBuf != ' ')
            {
                m_pSendData[m_dwHexDataLen - 1] = (m_pSendData[m_dwHexDataLen - 1] << 4) | hexCharToValue(*pBuf);
            }
            i = 0;
            pBuf++;
        }
    }
}

uint8_t ConnectivityManager6051::hexCharToValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    return 0; // 无效字符处理
}

QByteArray _Kits::ConnectivityManager6051::recvDataGet()
{
    std::unique_lock<std::mutex> lock(m_mutex_recvArray);
    m_cond_recvArray.wait_for(lock, std::chrono::milliseconds(100));
    if (mvec_qbt_recvArray.empty())
    {
        return QByteArray();
    }

    QByteArray recvArray = mvec_qbt_recvArray.front();
    mvec_qbt_recvArray.pop_front();
    lock.unlock();

    return recvArray;
}

void ConnectivityManager6051::onRecvSpeedSingle(uint8_t channel)
{
}

std::vector<uint32_t> ConnectivityManager6051::readAllCounters()
{
    if (mb_IsStop.load())
    {
        return std::vector<uint32_t>();
    }

    QByteArray recvData = recvDataGet();

    if (recvData.isEmpty())
    {
        return std::vector<uint32_t>();
    }

    QVariantMap recvDataMap;
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    long long timestamp = duration.count();

    recvDataMap["data"] = recvData;
    recvDataMap["timestamp"] = timestamp;
    emit sendNaturalData(recvDataMap, getName());

    std::vector<uint8_t> response(recvData.begin(), recvData.end());

    if (m_Speed != nullptr)
    {
        return m_Speed->parseDynamicCounterResponse(response, 1);
    }

    return std::vector<uint32_t>();
}

// 添加定时触发方法
void ConnectivityManager6051::sendSpeedData()
{
    m_speedData = calcSpeedData();

    emit sendOutSpeedDataTotal(m_speedData);
}

// 移植计算逻辑
TIS_Info::SpeedData ConnectivityManager6051::calcSpeedData()
{
    static int needCalculate_interval = 0;

    TIS_Info::SpeedData speedData = {0};

    // 计算实际时间间隔
    const qint64 currentElapsed = m_elapsedTimer.elapsed();
    if (m_lastElapsed != 0)
    {
        m_actualInterval = (currentElapsed - m_lastElapsed) / 1000.0;
    }
    m_lastElapsed = currentElapsed;

    if (m_actualInterval <= 0)
    {
        _Kits::LogError("[speed]:m_actualInterval <= 0");
        return speedData;
    }

    // 更新脉冲计数
    mvec_lastpulse = mvec_nowpulse;

    if (mb_isSimulation)
    {
        for (auto &pulse : mvec_nowpulse)
        {
            pulse += mi_simulationPulse; // 模拟脉冲增加
        }
    }
    else
    {
        mvec_nowpulse = readAllCounters();

        if (mvec_nowpulse.size() == 0)
        {
            _Kits::LogError("[speed]:mvec_nowpulse.size() == 0");
            needCalculate_interval += 100;
            return speedData;
        }
    }

    if (mvec_lastpulse.empty())
    {
        _Kits::LogError("[speed]:mvec_lastpulse.empty()");
        return speedData;
    }

    // 计算脉冲差
    uint64_t totalPulse = 0; // 总脉冲
    double pulseDiff = 0;    // 脉冲差
    for (size_t i = 0; i < mvec_nowpulse.size(); ++i)
    {
        if (i < mvec_lastpulse.size())
        {
            pulseDiff += (mvec_nowpulse[i] - mvec_lastpulse[i]);
            totalPulse += mvec_nowpulse[i];
        }
    }

    totalPulse /= std::max(1, (int)mvec_nowpulse.size());

    // 计算平均值（6051只有2个通道）
    speedData.mull_pulse = totalPulse; // 记录总脉冲

    // 计算里程
    speedData.mdble_kilometer = static_cast<double>(totalPulse) / static_cast<double>(mus_wheel_pusle) * md_wheel_diameter / 1000.0;

    // 计算速度（米/秒）
    // 6051net的速度计算公式：速度 = (里程差) / 实际时间间隔

    speedData.mdble_speed =
        (speedData.mdble_kilometer - md_kilometer) * 1000 / (mll_time_interval + needCalculate_interval) * 1000 * 3.6; // km / h
    md_kilometer = speedData.mdble_kilometer;

    static int nCount = -1;

    if (++nCount % 250 == 0)
    {
        _Kits::LogInfo("6051: 速度：{}, 里程：{}, 脉冲差：{}, 脉冲总数：{}, 时间间隔：{}",
                       std::to_string(speedData.mdble_speed),
                       std::to_string(speedData.mdble_kilometer),
                       std::to_string(pulseDiff),
                       std::to_string(totalPulse),
                       std::to_string((mll_time_interval + needCalculate_interval) / 1000.0));
    }

    if (nCount >= INT32_MAX)
    {
        nCount = -1;
    }

    needCalculate_interval = 0;
    return speedData;
}