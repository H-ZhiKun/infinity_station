// #pragma once
// #include "DeviceData.hpp"
// #include"DeviceDeal.h"
// #include <QHostAddress>
// #include <QObject>
// #include <QUdpSocket>
// #include "yaml-cpp/yaml.h"
// #include <string>
// #include <vector>

// #include"kits/common/thread_pool/ConcurrentPool.h"
// #include <mutex>
// #include<queue>
// #include<thread>
// #include <atomic>
// #include<condition_variable>
// #include"kits/object_pool/ObjectPool.h"

// namespace _Modules
// {
// class YanyanDeviceDeal : public DeviceDeal
// {
//     Q_OBJECT

//     public:
//         YanyanDeviceDeal();
//         YanyanDeviceDeal(const YAML::Node &config);
//         YanyanDeviceDeal(const YanyanDeviceDeal &) = delete;
//         YanyanDeviceDeal &operator=(const YanyanDeviceDeal &) = delete;

//         ~YanyanDeviceDeal() override;

//     public:
//         bool Init(const YAML::Node &) override;
//         _Modules::Err LastErrorCode() override;
//         _Modules::DeviceSurfaceData GetDeviceData() override;
//         bool StartDetect() override;
//         bool StopDetect() override;
//         void ReloadDevice() override;
//         void ChangeCenterPoint(
//             std::vector<DeviceSingleData> &DeviceSurfaceData,
//             const ChangeCenterData &change_center_data) override;
//         void SkewDeviceData(std::vector<DeviceSingleData> &DeviceSurfaceData,
//                             float &angle_degrees) override;
//         float IsDeviceSkew(std::vector<DeviceSingleData> &DeviceSurfaceData,
//                            float &angle_degrees) override;
//         std::vector<DeviceSingleData> GetParallelData(
//             std::vector<DeviceSingleData> &device_singledata, float &height) override;
//         void CompensateData() override;
//         void LoginDevice(const YAML::Node &config) override;
//         bool GetDeviceState() override
//         {
//             return mb_device_state;
//         }
//         //解析雷达数据
//         void ParseRadarData(const QByteArray &data);

//     signals:
//         void deviceDataGet(unsigned short &device_index); // 准备接受数据了，通知外部主动获取，因为sick并没有数据回调

//     public slots:
//         void onReadyRead(); // 处理数据接收

//     private:
//         _Kits::ObjectPool<DeviceSurfaceData> m_point_pool; // 对象池，用于管理点云数据对象
//         void consumerThreadFunc(); // 线程函数
//         // 设备配置
//         DeviceConfig m_device_config;

//         // 设备状态
//         bool mb_device_state = false;
//         QUdpSocket *udpSocket = nullptr; // UDP 套接字
//         // 设备数据
//         std::vector<DeviceSurfaceData> mvec_one_point; //一帧全部数据
//         std::mutex m_point_mutex; // 互斥锁
//         std::mutex m_queue_mutex; // 互斥锁
//         std::queue<QByteArray> dataQueue;//点云数据队列
//         std::condition_variable m_cond_var; // 条件变量
//         std::atomic<bool> stopConsumerThread{false};
//         std::thread consumerThread; // 线程对象

// };
// }