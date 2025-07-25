
// #include "YanyanDeviceDeal.h"


// using namespace _Modules;
// using namespace _Kits;


// YanyanDeviceDeal::YanyanDeviceDeal()
// {
//     udpSocket = new QUdpSocket(this);

//     stopConsumerThread = false;
//     consumerThread = std::thread(&YanyanDeviceDeal::consumerThreadFunc, this);
  
// }

// YanyanDeviceDeal::YanyanDeviceDeal(const YAML::Node &config)
// {

// }

// YanyanDeviceDeal::~YanyanDeviceDeal()
// {
//     stopConsumerThread = true;
//     m_cond_var.notify_all(); // 通知所有等待的线程
//     if (consumerThread.joinable())
//     {
//         consumerThread.join();
//     }

//     if (udpSocket != nullptr)
//     {
//         udpSocket->close();
//         delete udpSocket;
//         udpSocket = nullptr;
//     }
// }
// bool YanyanDeviceDeal::Init(const YAML::Node &config)
// {
    
//     m_device_config.mstr_ip = config["ip"].as<std::string>();
//     m_device_config.mstr_name = config["name"].as<std::string>();
//     m_device_config.mus_port = config["port"].as<unsigned short>();
//     //绑定端口
//     quint16 port = static_cast<quint16>(m_device_config.mus_port);
//     QHostAddress address(QString::fromStdString(m_device_config.mstr_ip));
//     if(!udpSocket->bind(address, port))
//     {
//         qDebug() << "Failed to bind UDP socket to port" <<  m_device_config.mus_port;
//         return false;
      
//     }
//     connect(udpSocket, &QUdpSocket::readyRead, this, &YanyanDeviceDeal::onReadyRead);
//     mb_device_state = true;//设备状态
//     return true;
// }
// bool YanyanDeviceDeal::StartDetect()
// {

//     return true;
// }
// bool YanyanDeviceDeal::StopDetect()
// {
//     return true;
// }

// _Modules::Err YanyanDeviceDeal::LastErrorCode()
// {
//     return Err(0, "no error");
// }
// _Modules::DeviceSurfaceData YanyanDeviceDeal::GetDeviceData()
// {
//     std::lock_guard<std::mutex> lock(m_point_mutex);
//     if(!mvec_one_point.empty())
//     {
//         // auto surfacedata =DeviceSurfaceData(mvec_one_point.front(),
//         //     m_device_config.mus_device_index,
//         //     m_device_config.mus_angle_start,
//         //     m_device_config.mus_angle_end,
//         //     0);
//         DeviceSurfaceData surfacedata =mvec_one_point.front();
//         mvec_one_point.erase(mvec_one_point.begin()); // 移除已处理的数据
//         return surfacedata;
//     }
   
//     return DeviceSurfaceData();
// }

// void YanyanDeviceDeal::ReloadDevice()
// {

// }
// void YanyanDeviceDeal::ChangeCenterPoint(
//     std::vector<DeviceSingleData> &DeviceSurfaceData,
//     const ChangeCenterData &change_center_data)
// {

// }


// void YanyanDeviceDeal::SkewDeviceData(std::vector<DeviceSingleData> &DeviceSurfaceData,
//     float &angle_degrees)
// {

// }
// float YanyanDeviceDeal::IsDeviceSkew(std::vector<DeviceSingleData> &DeviceSurfaceData,
//     float &angle_degrees)
// {
//     return 0.0f;
// }
// std::vector<DeviceSingleData> YanyanDeviceDeal::GetParallelData(
//     std::vector<DeviceSingleData> &device_singledata, float &height)
// {
//     return std::vector<DeviceSingleData>();
// }
// void YanyanDeviceDeal::CompensateData()
// {

// }
// void YanyanDeviceDeal::LoginDevice(const YAML::Node &config)
// {

// }

// void YanyanDeviceDeal::onReadyRead()
// {
//     while (udpSocket->hasPendingDatagrams())
//     {
//         QByteArray buffer;
//         buffer.resize(udpSocket->pendingDatagramSize());

//         QHostAddress sender;
//         quint16 senderPort;

//         udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
//         qDebug() << "Received data from" << sender.toString() << ":" << senderPort;


//         // 将接收到的数据放入队列
//         {
//             std::lock_guard<std::mutex> lock(m_queue_mutex);
//             if(dataQueue.size() >= 100) // 队列大小限制
//             {
//                 continue;
//             }
//             dataQueue.push(std::move(buffer));
//         }
//         m_cond_var.notify_one(); // 通知消费者线程有新数据可用
//         // 处理接收到的数据
//         // 解析数据并存储到 mvec_one_point 中
//         // _Kits::threadPool().runAfter([this, buffer=std::move(buffer)]()
//         // {

//         //     //解析点云数据
//         //     int count = buffer.size() / sizeof(DeviceSingleData);
//         //     DeviceSurfaceData surfacedata;
    
//         //     surfacedata.mvec_data_surface.resize(count);
//         //     for (int i = 0; i < count; ++i)
//         //     {
//         //         const DeviceSingleData *data = reinterpret_cast< const DeviceSingleData *>(buffer.data() + i * sizeof(DeviceSingleData));
//         //         surfacedata.mvec_data_surface[i] = *data;
//         //     }
//         //     {
//         //         std::lock_guard<std::mutex> lock(m_mutex);
//         //         mvec_one_point.push_back(std::move(surfacedata));
//         //     }
//         // });
       

//     }
// }

// void YanyanDeviceDeal::ParseRadarData(const QByteArray &data)
// {
//     // 解析雷达数据的逻辑
//     // 这里可以根据具体的协议格式进行解析
//     // 例如，将数据转换为结构体或其他格式
//     // 然后将解析后的数据存储到 mvec_one_point 中
//     // 这里是一个示例，实际解析逻辑可能会有所不同

//     DeviceSurfaceData surfacedata;
//     int count = data.size() / sizeof(DeviceSingleData);
//     surfacedata.mvec_data_surface.resize(count);
//     for (int i = 0; i < count; ++i)
//     {
//         const DeviceSingleData *dataPtr = reinterpret_cast<const DeviceSingleData *>(data.data() + i * sizeof(DeviceSingleData));
//         surfacedata.mvec_data_surface[i] = *dataPtr;
//     }
    
//     mvec_one_point.push_back(std::move(surfacedata));
// }
// void YanyanDeviceDeal::consumerThreadFunc()
// {
//     while (!stopConsumerThread)
//     {
//        QByteArray buffer;
//        //从队列中取出数据
    
//         std::unique_lock<std::mutex> lock(m_queue_mutex);
//         m_cond_var.wait(lock, [this] { return !dataQueue.empty() || stopConsumerThread; });
//         if (stopConsumerThread && dataQueue.empty()) break; // 线程关闭标志
//         buffer = std::move(dataQueue.front());
//         dataQueue.pop();
        
//             // 处理数据
        
//         lock.unlock(); // 解锁以允许其他线程访问队列
//         auto deviceData = m_point_pool.getObject(); // 获取对象池中的对象
//         //deviceData->mvec_data_surface
//         //存储解析后的数据
//         {
//             // std::lock_guard<std::mutex> lock(m_mutex);
//             // mvec_one_point.push_back();
//         }
//     }
    
// }

