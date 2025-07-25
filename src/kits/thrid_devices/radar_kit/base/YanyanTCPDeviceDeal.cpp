// #include "YanyanTCPDeviceDeal.h"

// using namespace _Modules;
// using namespace _Kits;

// YanyanTCPDeviceDeal::YanyanTCPDeviceDeal() :m_tcp_server( std::make_shared<_Kits::TcpServer>()),
//     stopConsumerThread(false),m_point_pool(std::make_unique<_Kits::ObjectPool<DeviceSurfaceData>>())

// {
    
//     //connect(m_tcp_server.get(), &TcpServer::dataReceived, this, &YanyanTCPDeviceDeal::ParseRadarData);
//     //启动消费着线程
    
//     consumerThread = std::thread(&YanyanTCPDeviceDeal::consumerThreadFunc, this);
   
   
// }
// YanyanTCPDeviceDeal::~YanyanTCPDeviceDeal()
// {
//     stopConsumerThread.store(true);
//     m_cond_var.notify_all(); // 通知所有等待的线程
//     if (consumerThread.joinable())
//     {
//         consumerThread.join();
//     }
//     if (m_tcp_server != nullptr)
//     {
//         m_tcp_server->stopServer();
//         m_tcp_server.reset();
//     }
//     m_data_queue = std::queue<QByteArray>(); // 清空队列
// }
// bool YanyanTCPDeviceDeal::Init(const YAML::Node &config_)
// {
//     m_device_config.mstr_ip = config_["ip"].as<std::string>();
//     m_device_config.mstr_name = config_["name"].as<std::string>();
//     m_device_config.mus_port = config_["port"].as<unsigned short>();

//     //绑定端口
//     quint16 port = static_cast<quint16>(m_device_config.mus_port);
//     QHostAddress address(QString::fromStdString(m_device_config.mstr_ip));
//     //m_tcp_server->setServerPort(port);
//     m_tcp_server->setPort(port);
//     if (!m_tcp_server->startServer())
//     {
//         qDebug() << "Failed to bind TCP socket to port" << m_device_config.mus_port;
//         return false;
//     }
    
//    connect(m_tcp_server.get(), &TcpServer::sendDataToOut, this, &YanyanTCPDeviceDeal::onReadyRead);
//    mb_device_state = true;//设备状态
    
//     return true;

// }

// void YanyanTCPDeviceDeal::onReadyRead(const QByteArray &data)
// {
//     // 将接收到的数据放入队列
//     {
//         std::lock_guard<std::mutex> lock(m_queue_mutex);
//         if (m_data_queue.size() >= 100) // 队列大小限制
//         {
//             return;
//         }
//         m_data_queue.push(data);
//     }
//     m_cond_var.notify_one(); // 通知消费者线程有新数据可用
// }

// void YanyanTCPDeviceDeal::consumerThreadFunc()
// {
//     while (!stopConsumerThread.load())
//     {
//         QByteArray data;
//         {
//             std::unique_lock<std::mutex> lock(m_queue_mutex);
//             m_cond_var.wait(lock, [this] { return !m_data_queue.empty() || stopConsumerThread.load(); });
//             if (stopConsumerThread.load())
//             {
//                 break;
//             }
//             if (!m_data_queue.empty())
//             {
//                 data = m_data_queue.front();
//                 m_data_queue.pop();
//             }
//         }
//         // 处理数据
//         auto deviceData = m_point_pool->getObject();
//         if (deviceData == nullptr)
//         {
//             continue;
//         }
//         mvec_one_point.push_back(std::move(*deviceData));


        
//     }

   
    
// }

// _Modules::DeviceSurfaceData YanyanTCPDeviceDeal::GetDeviceData()
// {
//     std::lock_guard<std::mutex> lock(m_point_mutex);
//     if (!mvec_one_point.empty())
//     {
//         DeviceSurfaceData surfacedata = mvec_one_point.front();
//         mvec_one_point.pop_front();
//         return surfacedata;
//     }
//     return DeviceSurfaceData(); // 返回空数据
// }

// bool YanyanTCPDeviceDeal::StartDetect()
// {
//     if (!m_tcp_server->startServer())
//     {
//         qDebug() << "Failed to start TCP server on port" << m_device_config.mus_port;
//         return false;
//     }
//     mb_device_state = true;
//     stopConsumerThread.store(false);
//     return true;
// }

// bool YanyanTCPDeviceDeal::StopDetect()
// {
//     stopConsumerThread.store(true);
//     m_cond_var.notify_all(); // 通知所有等待的线程
//     if (consumerThread.joinable())
//     {
//         consumerThread.join();
//     }
//     if (m_tcp_server != nullptr)
//     {
//         m_tcp_server->stopServer();
//         m_tcp_server.reset();
//     }
//     m_data_queue = std::queue<QByteArray>(); // 清空队列
//     return true;
// }
// void YanyanTCPDeviceDeal::ChangeCenterPoint(
//     std::vector<DeviceSingleData> &DeviceSurfaceData,
//     const ChangeCenterData &change_center_data)
// {

// }
// void YanyanTCPDeviceDeal::SkewDeviceData(std::vector<DeviceSingleData> &DeviceSurfaceData,
//     float &angle_degrees)
// {

// }void YanyanTCPDeviceDeal::CompensateData()
// {

// }
// void YanyanTCPDeviceDeal::LoginDevice(const YAML::Node &config)
// { 
// }


// void YanyanTCPDeviceDeal::ReloadDevice()
// {

// }