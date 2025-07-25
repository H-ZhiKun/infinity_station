#ifndef _CAMERA_USB_H_
#define _CAMERA_USB_H_

#include "kits/camera_service/CameraBase.h"
#include <mutex>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

}

#include "yaml-cpp/yaml.h"
#include <QImage>
#include <QSharedPointer>
#include <atomic>
#include <memory>
#include <QObject>
#include <string>
#include <thread>
#include "kits/camera_service/CameraFactory.h"

namespace _Kits
{

class CameraUsb : public CameraBase
{
    Q_OBJECT

  public:
    CameraUsb();
    virtual ~CameraUsb();

    bool openCamera(const YAML::Node &config) override;
    bool closeCamera() override;
    bool startGrab() override;
    bool stopGrab() override;
    bool getImage() override;
    std::string getCameraName() override;
  private:
    void grabFrames(); // 视频采集线程函数
    bool ReadConfig(const YAML::Node &config) override;

    // FFmpeg相关成员
    AVFormatContext *m_formatCtx = nullptr;
    AVCodecContext *m_codecCtx = nullptr;
    int m_videoStreamIndex = -1;
    std::atomic<bool> m_grabRunning{false};
    std::thread m_grabThread;

  private:
    // class CameraUsbRegister
    // {
    //   public:
    //     CameraUsbRegister()
    //     {
    //         ICameraFactory::RegisterCmaeraCrteatFactory<CameraUsb>("FFMPEGUSB");
    //     }
    // };

    // inline static CameraUsbRegister s_register;

  protected:
    void ImageHeightWidth() override;
};

} // namespace _Kits
#endif