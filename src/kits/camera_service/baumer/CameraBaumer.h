#pragma once

#include "bgapi2_genicam.hpp"
#include "kits/camera_service/CameraBase.h"
#include "kits/camera_service/CameraFactory.h"
#include<thread>
#include <chrono>
namespace _Kits
{

class CameraBaumer : public CameraBase
{
    Q_OBJECT
  public:
    explicit CameraBaumer(QObject *parent = nullptr);
    ~CameraBaumer();

    // 实现基类纯虚函数
    bool openCamera(const YAML::Node &config) override;
    bool closeCamera() override;
    bool startGrab() override;
    bool stopGrab() override;
    bool getImage() override;
    std::string getCameraName() override;
    bool downloadRecord(const DownloadTimeRange &timeRange,
                        const std::string &savePath);
    //用于寻找相机  删除相机                    
    //void searchCamera(const std::string &snNumber);
    void removeCamera(const std::string &snNumber);

    void initInterface();

    // void writeVideofunc(int no, AVPacket &pkt, int flag) override;
    bool ReadConfig(const YAML::Node &config) override;
    bool openDataStream();
    void addBufferToStream();
    void storeImg(BGAPI2::Buffer *pBufferFilled);

    void startcachevideo();

  public slots:
    // void OnSubtitleChanged(std::vector<std::string> &subtitle) override;
    // bool SetSubtitle(int x_StartPos = 20,
    //                  int y_StartPos = 100,
    //                  int offset_dis = 32) override;
    // void OnStartRecord(const std::string &filename) override;
    // void OnStopRecord() override;

  protected slots:
    // void OnImageReady(const QImage &image,
    //                   const std::string &camera_name) override;

  protected:
    void ImageHeightWidth() override;

  private:
    // 添加回调函数
    static void ImageCallback(void *callbackOwner, BGAPI2::Buffer *pBuffer);
    static void BGAPI2CALL PnPEventHandler(void *callBackOwner, BGAPI2::Events::PnPEvent *pBuffer);

    std::chrono::steady_clock::time_point convertToChronoTime(const DownloadTime& dt);

    void CacheVideo();

  private:
    // Baumer SDK 相关成员
    //BGAPI2::SystemList *m_systems;
    BGAPI2::System *m_systems; 
    //BGAPI2::InterfaceList *m_interfaces;
    //BGAPI2::Interface *m_interface;
    //BGAPI2::DeviceList *m_devices;
    BGAPI2::Device *m_device;
    BGAPI2::DataStream *m_stream;
    
    static inline QImage::Format m_color_channel =
        QImage::Format_Grayscale16; // 默认16位灰度图
    std::list<BGAPI2::Buffer *> streamBuffers_;
    // 相机状态
    std::atomic_bool m_bConnected;
    std::atomic_bool m_bStreaming;
    std::atomic_bool m_bIsGrabbing;
    std::atomic_bool bOpen_;

    // 图像缓冲
    std::vector<uint8_t> m_imageBuffer;

    CameraInfo m_cameraInfo;
    
    static bool mb_isinit; // 是否初始化SDK

    // 初始化和清理帮助函数
    bool initializeSdk();
    void releaseSdk();
    void clearBuffersFromStream();
    void stopDev();
    bool searchCamera(const std::string &needNumber);//仅支持一个堡盟相机
    void configureCamera();
    

private:
    class CameraBaumerRegister
    {
      public:
        CameraBaumerRegister()
        {
            ICameraFactory::RegisterCameraCreateFactory<CameraBaumer>("BAUMER");
        }
    };

    inline static CameraBaumerRegister s_register;
};

} // namespace _Kits