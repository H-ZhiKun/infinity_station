#include "ConnectivityManager7404.h"

using namespace _Kits;
_Kits::ConnectivityManager7404::ConnectivityManager7404(QObject *parent)
{
}

ConnectivityManager7404::~ConnectivityManager7404()
{
}

bool _Kits::ConnectivityManager7404::init(const YAML::Node &config)
{
    m_Speed = std::make_unique<speed7404Net>();

    std::string ip_str = config["ip"].as<std::string>();
    mqhost_address.setAddress(ip_str.c_str());
    mb_isSimulation = config["simulation"].as<bool>(false);
    mi_simulationPulse = config["simulation_pulse_add"].as<int>(0);

    mqull_port = config["port"].as<quint16>();

    m_time_interval = config["time_interval"].as<uint8_t>();

    mus_wheel_pusle = config["wheel_pusle"].as<uint16_t>();
    mf_wheel_diameter = config["wheel_diameter"].as<float>();

    if (!mqhost_address.isNull() || mqull_port == 0)
    {
        return false;
    }
    mb_IsStop.store(false);
    m_thread_speedCalc = std::thread([this]() {
        while (!mb_IsStop.load())
        {
            this->sendSpeedData();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    return true;
}

bool _Kits::ConnectivityManager7404::start()
{
    if (m_UdpClient == nullptr)
    {
        m_UdpClient = std::make_unique<UdpClient>(this);
    }

    mab_is_connect.store(m_UdpClient->bind(mqhost_address, mqull_port));

    connect(m_UdpClient.get(), &UdpClient::dataReceived, this, &ConnectivityManager7404::onRecvUdpData);

    resetAllCounters();

    m_elapsedTimer.restart();

    return mab_is_connect.load();
}

bool _Kits::ConnectivityManager7404::stop()
{
    if (m_UdpClient != nullptr)
    {
        m_UdpClient->close();
        m_UdpClient.reset();

        m_timer.stop();
    }

    m_lastElapsed = 0;

    return true;
}

QString _Kits::ConnectivityManager7404::getName() const
{
    return "7404";
}

void _Kits::ConnectivityManager7404::onControlSingleDo(const QVariant &data)
{
    QVariantMap map_data = data.toMap();

    controlSingleDO(map_data["channel"].toInt(), map_data["state"].toBool());

    ConcurrentPool::runAfter(1000, [this, map_data]() { controlSingleDO(map_data["channel"].toInt(), false); }); // 延时关闭报警
}

void _Kits::ConnectivityManager7404::onRecvUdpData(const QByteArray &data, const QHostAddress &sender, quint16 senderPort)
{
    std::lock_guard<std::mutex> lock(m_mutex_recvArray);
    mvec_qbt_recvArray.push_back(data);
}

// 读取所有4个通道的计数值
std::vector<uint32_t> ConnectivityManager7404::readAllCounters()
{
    // 构造发送指令 00 03 00 00 00 06 01 03 00 78 00 08
    /*
        其中 00 03 为事物处理标识符(事物处理标示符可以由用户设置)
        00 00 为协议标示符，固定为0
        00 06 代表后面字节的数量
        01 为模块地址，
        03 为读取保持型寄存器的功能码（计数值属于保持型寄存器）
        00 78 为计数通道0的寄存器地址
        00 08 为需要读取的寄存器数量。每一个通道占用两个寄存器地址，4个通道一共8个寄存
        器地址。
    */
    std::vector<uint8_t> sendData = {0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x78, 0x00, 0x08};

    // 发送数据
    QByteArray sendArray(reinterpret_cast<const char *>(sendData.data()), sendData.size());
    m_UdpClient->send(sendArray);

    QByteArray recvArray = recvUdpMessage();
    std::vector<uint8_t> response(recvArray.begin(), recvArray.end());

    if (m_Speed != nullptr)
    {
        return m_Speed->parseDynamicCounterResponse(response, 4);
    }

    return {0};
}

// 读取单个通道的计数值
std::vector<uint32_t> ConnectivityManager7404::readSingleCounter(uint8_t channel)
{
    // 构造发送指令 00 03 00 00 00 06 01 03 00 78 00 02
    /*
        00 00 为协议标示符，固定为0
        00 06 代表后面字节的数量
        01 为模块地址
        03 为读取保持型寄存器的功能码
        00 78 为计数通道0的寄存器地址，注意一个计数通道占用2个地址，所以计数通道1的寄
        存器为00 7a，通道2为00 7c,通道3为00 7e
        00 02 为读取的寄存器数量，这里02表示读取一个通道。
    */
    std::vector<uint8_t> sendData = {0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x78, 0x00, 0x02};

    if (channel < mui_channel_max_num)
    {
        sendData.push_back(mui_channel_start_num + channel * mui_channel_gap);
        sendData.push_back(0x00);
        sendData.push_back(0x02);
    }
    else
    {
        return {0};
    }

    // 发送数据
    QByteArray sendArray(reinterpret_cast<const char *>(sendData.data()), sendData.size());
    m_UdpClient->send(sendArray);

    // 接收数据
    QByteArray recvArray = recvUdpMessage();
    std::vector<uint8_t> response(recvArray.begin(), recvArray.end());

    if (m_Speed != nullptr)
    {
        return m_Speed->parseDynamicCounterResponse(response, channel);
    }

    return {0};
}

// 清零所有4个通道的计数值
bool ConnectivityManager7404::resetAllCounters()
{
    // 构造发送指令
    std::vector<uint8_t> sendData = {0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x78, 0x00, 0x08};

    // 发送数据
    QByteArray sendArray(reinterpret_cast<const char *>(sendData.data()), sendData.size());
    m_UdpClient->send(sendArray);

    // 接收数据
    QByteArray recvArray = recvUdpMessage();
    std::vector<uint8_t> response(recvArray.begin(), recvArray.end());

    // 检查返回数据
    if (response.size() >= 12)
    {
        return true;
    }

    return false;
}

// 清零单个通道的计数值
bool ConnectivityManager7404::resetSingleCounter(uint8_t channel)
{
    // 构造发送指令
    std::vector<uint8_t> sendData = {0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x78, 0x00, 0x02};

    // 发送数据
    QByteArray sendArray(reinterpret_cast<const char *>(sendData.data()), sendData.size());
    m_UdpClient->send(sendArray);

    // 接收数据
    QByteArray recvArray = recvUdpMessage();
    std::vector<uint8_t> response(recvArray.begin(), recvArray.end());

    // 检查返回数据
    if (response.size() >= 12)
    {
        return true;
    }

    return false;
}

// 读取所有4个通道的开关量采集值
std::vector<bool> ConnectivityManager7404::readAllDigitalInputs()
{
    // 构造发送指令
    std::vector<uint8_t> sendData = {0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x01, 0x0D};

    // 发送数据
    QByteArray sendArray(reinterpret_cast<const char *>(sendData.data()), sendData.size());
    m_UdpClient->send(sendArray);

    // 接收数据
    QByteArray recvArray = recvUdpMessage();
    std::vector<uint8_t> response(recvArray.begin(), recvArray.end());

    // 解析数据
    std::vector<bool> inputs(4, false);
    if (response.size() >= 10)
    {
        uint8_t value = response[9];
        for (int i = 0; i < 4; ++i)
        {
            inputs[i] = (value >> i) & 0x01;
        }
    }

    return inputs;
}

// 控制单个DO输出
bool ConnectivityManager7404::controlSingleDO(uint8_t channel, bool state)
{
    // 构造发送指令
    std::vector<uint8_t> sendData = {0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x01, 0x05, 0x02, 0x00, 0xFF, 0x00};

    // 发送数据
    QByteArray sendArray(reinterpret_cast<const char *>(sendData.data()), sendData.size());
    m_UdpClient->send(sendArray);

    // 接收数据
    QByteArray recvArray = recvUdpMessage();
    std::vector<uint8_t> response(recvArray.begin(), recvArray.end());

    // 检查返回数据
    if (response.size() >= 12)
    {
        return true;
    }

    return false;
}

// 一次控制4个DO输出
bool ConnectivityManager7404::controlAllDO(const std::vector<bool> &states)
{
    if (states.size() != 4)
    {
        return false;
    }

    // 构造发送指令
    std::vector<uint8_t> sendData = {0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x01, 0x0F, 0x02, 0x00, 0x00, 0x04};

    // 发送数据
    QByteArray sendArray(reinterpret_cast<const char *>(sendData.data()), sendData.size());
    // m_UdpClient->sendData(sendArray, mqhost_address, mqull_port);
    m_UdpClient->send(sendArray);

    // 接收数据
    QByteArray recvArray = recvUdpMessage();
    std::vector<uint8_t> response(recvArray.begin(), recvArray.end());

    // 检查返回数据
    if (response.size() >= 12)
    {
        return true;
    }

    return false;
}

QByteArray ConnectivityManager7404::recvUdpMessage()
{

    int count = 0;
    while (mvec_qbt_recvArray.empty())
    {
        if (count >= 100)
        {
            break;
        }

        count++;
    }

    if (mvec_qbt_recvArray.empty())
    {
        return QByteArray();
    }

    std::unique_lock<std::mutex> lock(m_mutex_recvArray);
    QByteArray recvArray = mvec_qbt_recvArray.front();
    mvec_qbt_recvArray.pop_front();
    lock.unlock();

    return recvArray;
}

void _Kits::ConnectivityManager7404::sendSpeedData()
{
    m_speedData = calcSpeedData();
    emit sendOutSpeedDataTotal(m_speedData);
}

ConnectivityManager7404::SpeedData _Kits::ConnectivityManager7404::calcSpeedData()
{
    SpeedData speedData = {0};

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
            return speedData;
        }
    }

    // 计算实际时间间隔
    const qint64 currentElapsed = m_elapsedTimer.elapsed(); // 获取累计毫秒数
    if (m_lastElapsed != 0)
    {
        m_actualInterval = (currentElapsed - m_lastElapsed) / 1000.0; // 转换为秒
    }
    m_lastElapsed = currentElapsed;

    for (size_t i = 0; i < mvec_nowpulse.size(); ++i)
    {
        speedData.mull_pulse += (mvec_nowpulse[i] - mvec_lastpulse[i]);
    }

    speedData.mull_pulse = speedData.mull_pulse / mvec_nowpulse.size();                            // 脉冲数
    speedData.mdble_kilometer = speedData.mull_pulse / mus_wheel_pusle * mf_wheel_diameter / 1000; // km
    speedData.mdble_speed = speedData.mdble_kilometer * 1000 / m_actualInterval;                   // 米每秒

    return speedData;
}

void ConnectivityManager7404::onRecvSpeedSingle(uint8_t channel)
{
    // emit sendOutSpeedDataSingle(m_Speed->GetSpeedData());
}

void _Kits::ConnectivityManager7404::onisSaveNaturalData(bool isSaveNaturalData)
{
    _mb_isSaveNaturalData = isSaveNaturalData;
}