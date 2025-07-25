// #pragma once

// #include "DeviceData.hpp"
// #include"DeviceDeal.h"
// #include<queue>
// #include"kits/object_pool/ObjectPool.h"
// #include <Qtimer>
// namespace _Modules
// {
// class YanyanTCPDeviceDeal : public DeviceDeal
// {
//     Q_OBJECT

//     public:
//         YanyanTCPDeviceDeal();
//         YanyanTCPDeviceDeal(const YAML::Node &config);
//         YanyanTCPDeviceDeal(const YanyanTCPDeviceDeal &) = delete;
//         YanyanTCPDeviceDeal &operator=(const YanyanTCPDeviceDeal &) = delete;

//         ~YanyanTCPDeviceDeal() override;

//         bool Init(const YAML::Node & config_) override;
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
//                             float &angle_degrees) override;
//         std::vector<DeviceSingleData> GetParallelData(
//             std::vector<DeviceSingleData> &device_singledata, float &height) override;
//         void CompensateData() override;
//         void LoginDevice(const YAML::Node &config) override;
//         bool GetDeviceState() override
//         {
//             return mb_device_state;
//         }

//     signals:
//         void dataReceived(const QByteArray &data); // 信号，数据接收

//     public slots:
//         void onReadyRead(const QByteArray &data); // 槽函数，处理接收到的数据

//     private:
//         std::deque<DeviceSurfaceData> mvec_one_point; // 存储点云数据 完整一帧数据
//         std::unique_ptr<_Kits::ObjectPool<DeviceSurfaceData>> m_point_pool; // 对象池，
//         std::mutex m_point_mutex; // 互斥锁，保护点云数据的访问
//         DeviceConfig m_device_config;//设备配置
//         std::shared_ptr<_Kits::TcpServer> m_tcp_server;
//         void consumerThreadFunc(); // 消费者线程函数
//         std::queue<QByteArray> m_data_queue; // 数据队列
//         std::mutex m_queue_mutex; // 互斥锁
//         std::condition_variable m_cond_var; // 条件变量，用于通知消费者线程
//         std::thread consumerThread; // 消费者线程
//         std::atomic<bool> stopConsumerThread{false}; // 停止消费者线程的标志
//         bool mb_device_state = false; // 设备状态标志

// };
// }