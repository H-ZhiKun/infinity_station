#pragma once
#include <yaml-cpp/yaml.h>
#include "kits/thrid_devices/radar_kit/base/DeviceData.hpp"
#include "kits/thrid_devices/radar_kit/base/DeviceDeal.h"
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <deque>
#include "kits/common/object_pool/ObjectPool.h"
#include "kits/common/log/CRossLogger.h"
namespace _Modules
{
    class YanYanDeviceDeal : public DeviceDeal
    {
        Q_OBJECT
      public:
        YanYanDeviceDeal();
        YanYanDeviceDeal(const YAML::Node &config);
        YanYanDeviceDeal(const YanYanDeviceDeal &) = delete;
        YanYanDeviceDeal &operator=(const YanYanDeviceDeal &) = delete;

        ~YanYanDeviceDeal() override;

      public:
        bool Init(const YAML::Node &) override;
        _Kits::Err LastErrorCode() override;
        _Kits::DeviceSurfaceData GetDeviceData() override;
        bool StartDetect() override;
        bool StopDetect() override;
        void ReloadDevice() override;
        void ChangeCenterPoint(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData,
                               const _Kits::ChangeCenterData &change_center_data) override;
        void SkewDeviceData(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees) override;
        float IsDeviceSkew(std::vector<TIS_Info::DeviceSingleData> &DeviceSurfaceData, float &angle_degrees) override;
        std::vector<TIS_Info::DeviceSingleData> GetParallelData(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                                                float &height) override;
        void CompensateData() override;
        void LoginDevice() override;
        bool GetDeviceState() override
        {
            return mb_device_state;
        }

      protected:
        void CallStat();
        void DataReceiveDeal(void *pData, int iDataLength); // 接收数据
        void DecodeFromString(const std::vector<char> &strRead);
        void reLoad();
        void close();

      private:
        struct pReceiveData
        {
            std::vector<char> m_vecData; // zmq数据缓存
            int m_iDataLength;
            pReceiveData(int m_iDataLength)
            {
                m_vecData.resize(m_iDataLength);
            }
        };

        YAML::Node m_config;
        void *m_pZmq = nullptr; // zmq上下文
        void *m_pSocketRecv = nullptr;
        bool m_bConnected; // 连接状态
        double m_dBandWidth;
        double m_dReceiveTime;
        int m_iTimeOut;         // 超时
        std::string m_strError; // 错误信息
        int m_iRecvHw = 1000;
        double m_dMaxReceiveTimeSecond;
        // 用于保存一秒内最长等待时间
        // 数据包的fps
        double m_dFps;
        std::atomic_bool m_bExitThread{false};
        DeviceConfig m_device_config; // 设备配置
        unsigned short m_port;
        std::string m_type;
        std::atomic_bool m_isGetData;
        std::deque<std::vector<char>> m_char_data;  // 存储接收到的数据
        std::mutex m_char_mutex;                    // 互斥锁，保护数据访问
        std::condition_variable m_cond_var;         // 条件变量，用于通知消费者线程
        std::atomic_bool stopConsumerThread{false}; // 停止消费者线程的标志

        std::deque<std::shared_ptr<_Kits::DeviceSurfaceData>> m_data_queue; // 一帧全部数据队列
        std::vector<TIS_Info::DeviceSingleData> mvec_one_point;

        std::mutex m_queue_mutex;     // 互斥锁
        bool mb_device_state = false; // 设备状态
        void Run();                   // 接收数据线程函数
        std::thread m_Revthread;      // 接收数据线程
        std::thread m_consumerThread; // 消费者线程
        void consumerThreadFunc();    // 消费者线程函数

        std::shared_ptr<_Kits::ObjectPool<_Kits::DeviceSurfaceData>> m_YYResultBufferPools;
    };

} // namespace _Modules
