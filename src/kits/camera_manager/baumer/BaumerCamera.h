
#pragma once

#include <QDebug>
#include <QImage>
#include <QObject>
#include <mutex>
#include <queue>
#include <string>
#include <yaml-cpp/yaml.h>

namespace _Kits
{
    class BaumerCamera : public QObject
    {
        Q_OBJECT
      public:
        explicit BaumerCamera(BGAPI2::Device *bgapi_device, const YAML::Node &config);
        ~BaumerCamera();
        bool getInitialized();
        void storeImg(unsigned char *bayerRG8Data, const std::string &pixelFormat, uint64_t width, uint64_t height, uint64_t frameId);
        QImage getCurrentImage();
        std::list<QImage> getImgBuffer();
        void startGrab(); // 开始抓图
        void stopGrab();  // 停止抓图
        // bool setParams(const std::string &key, uint64_t value);
        void initialize();        // 初始化入口
        bool openDevice();        // 打开相机
        bool addBufferToStream(); // 添加缓存
        void deinitialized();
        void clearBufferFromStream();
        void stopDevice();
        void initParams();
        void setSnStr(const std::string &);

        QImage mono10ToQImage(unsigned char *data, int width, int height);
      signals:
        void sendImage(const std::string &, const QImage &); // 发送图像至相机管理类

      private:
        bool bActive_ = false; // camera 是否开始采集
        bool bOpen_ = false;   // camera  device是否打开
        std::mutex mtxcrt;
        std::queue<QImage> matBuffer_; // 图像缓冲区
        BGAPI2::DataStream *pStream_ = nullptr;
        BGAPI2::Device *pDev_ = nullptr;
        std::unordered_map<std::string, std::string> mapCameraParam_; // 相机参数
        std::list<BGAPI2::Buffer *> streamBuffers_;
        YAML::Node param_;
        uint32_t imageNum{0}; // 图像计数
        std::string snStr{};
        BGAPI2::BufferList *bufferList;
        // std::thread worker_thread_; // 常驻线程
    };

} // namespace _Kits
