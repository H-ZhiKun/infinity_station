#include "CorrugationBaseInfo.h"
#include <windows.h>
#include <conio.h>
#include <iomanip>
#include <sstream>

CorrugationBaseInfo::CorrugationBaseInfo()
    : m_startChannel(0)
    , m_channelCount(3)
    , m_isRunning(false)
{
    m_instantAiCtrl = std::unique_ptr<InstantAiCtrl>(InstantAiCtrl::Create());
    m_logger = std::make_unique<PerformanceLog>();
}

CorrugationBaseInfo::~CorrugationBaseInfo()
{
    stop();
}

bool CorrugationBaseInfo::init(const YAML::Node& config)
{
    try {
        // 从YAML配置读取参数
        m_deviceDescription = config["device_description"].as<std::wstring>();
        m_profilePath = config["profile_path"].as<std::wstring>();
        m_startChannel = config["start_channel"].as<int>();
        m_channelCount = config["channel_count"].as<int>();

        // 初始化日志
        std::string logPath = config["log_path"].as<std::string>();
        m_logger->init(logPath.c_str(), "height");

        // 初始化CSV文件
        m_csvPath = config["csv_path"].as<std::string>() + "/corrugation_" + 
                    getCurrentTimestamp() + ".csv";
        m_csvFile.open(m_csvPath);
        if (!m_csvFile.is_open()) return false;

        // 写入CSV头
        m_csvFile << "Timestamp,Height1,Height2,Height3\n";
        
        // 初始化设备
        DeviceInformation devInfo(m_deviceDescription.c_str());
        ErrorCode ret = m_instantAiCtrl->setSelectedDevice(devInfo);
        if (BioFailed(ret)) return false;
        
        ret = m_instantAiCtrl->LoadProfile(m_profilePath.c_str());
        if (BioFailed(ret)) return false;

        return true;
    }
    catch (...) {
        return false;
    }
}

bool CorrugationBaseInfo::start()
{
    if (m_isRunning) return false;
    
    m_isRunning = true;
    m_collectionThread = std::make_unique<std::thread>(&CorrugationBaseInfo::dataCollectionThread, this);
    return true;
}

void CorrugationBaseInfo::stop()
{
    if (!m_isRunning) return;
    
    m_isRunning = false;
    if (m_collectionThread && m_collectionThread->joinable()) {
        m_collectionThread->join();
    }
    
    if (m_instantAiCtrl) {
        m_instantAiCtrl->Dispose();
    }
}

// 电压转换为高度
float CorrugationBaseInfo::adjustData(float fVoltage)
{
    return ((fVoltage - (-10.0f)) / 20.0f) * (80.0f) + (-40.0f);
}

void CorrugationBaseInfo::updateData(const double* scaledData, int channelCount)
{
    CorrugationData data;
    data.timestamp = getCurrentTimestamp();
    
    for (int32 i = 0; i < channelCount; ++i) {
        data.heights[i] = adjustData(scaledData[i]);
    }

    // 更新数据缓存
    {
        std::lock_guard<std::mutex> lock(m_bufferMutex);
        m_dataBuffer.push(data);
        if (m_dataBuffer.size() > MAX_BUFFER_SIZE) {
            m_dataBuffer.pop();
        }
    }

    // 写入CSV文件
    writeCSV(data);
}

void CorrugationBaseInfo::writeCSV(const CorrugationData& data)
{
    if (m_csvFile.is_open()) {
        m_csvFile << data.timestamp << ","
                  << data.heights[0] << ","
                  << data.heights[1] << ","
                  << data.heights[2] << "\n";
        m_csvFile.flush();
    }
}

bool CorrugationBaseInfo::getLatestData(CorrugationData& data)
{
    std::lock_guard<std::mutex> lock(m_bufferMutex);
    if (m_dataBuffer.empty()) return false;
    
    data = m_dataBuffer.back();
    return true;
}

std::vector<CorrugationData> CorrugationBaseInfo::getDataBuffer(size_t count)
{
    std::lock_guard<std::mutex> lock(m_bufferMutex);
    std::vector<CorrugationData> result;
    size_t size = std::min(count, m_dataBuffer.size());
    
    auto temp = m_dataBuffer;
    while (result.size() < size) {
        result.push_back(temp.front());
        temp.pop();
    }
    
    return result;
}

std::string CorrugationBaseInfo::getCurrentTimestamp()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    std::stringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << st.wYear << "-"
       << std::setw(2) << st.wMonth << "-"
       << std::setw(2) << st.wDay << " "
       << std::setw(2) << st.wHour << ":"
       << std::setw(2) << st.wMinute << ":"
       << std::setw(2) << st.wSecond << "."
       << std::setw(3) << st.wMilliseconds;
    
    return ss.str();
}

void CorrugationBaseInfo::dataCollectionThread()
{
    std::vector<double> scaledData(m_channelCount);
    
    while (m_isRunning) {
        ErrorCode ret = m_instantAiCtrl->Read(m_startChannel, m_channelCount, scaledData.data());
        if (BioFailed(ret)) {
            break;
        }

        updateData(scaledData.data(), m_channelCount);
        Sleep(1000); // 每秒读取一次数据
    }
}