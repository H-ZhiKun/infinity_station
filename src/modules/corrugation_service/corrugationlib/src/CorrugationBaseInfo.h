#pragma once
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "bdaqctrl.h"

using namespace Automation::BDaq;

// 定义数据结构
struct CorrugationData {
    std::string timestamp;
    float heights[3];
    
    CorrugationData() : heights{0.0f, 0.0f, 0.0f} {}
};

class CorrugationBaseInfo {
public:
    CorrugationBaseInfo();
    ~CorrugationBaseInfo();

    bool init(const YAML::Node& config);
    bool start();
    void stop();
    
    // 数据访问方法
    bool getLatestData(CorrugationData& data);
    std::vector<CorrugationData> getDataBuffer(size_t count);

private:
    float adjustData(float fVoltage);
    void updateData(const double* scaledData, int channelCount);
    void dataCollectionThread();
    void writeCSV(const CorrugationData& data);
    std::string getCurrentTimestamp();

private:
    // 设备相关
    std::unique_ptr<InstantAiCtrl> m_instantAiCtrl;
    
    // 配置参数
    std::wstring m_deviceDescription;
    std::wstring m_profilePath;
    int32 m_startChannel;
    int32 m_channelCount;
    
    // 数据缓存
    static constexpr size_t MAX_BUFFER_SIZE = 1000;
    std::queue<CorrugationData> m_dataBuffer;
    std::mutex m_bufferMutex;
    
    // CSV文件
    std::ofstream m_csvFile;
    std::string m_csvPath;
    
    // 线程控制
    std::atomic<bool> m_isRunning;
    std::unique_ptr<std::thread> m_collectionThread;
};