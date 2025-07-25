#pragma once
// #include "bgapi2_genicam.hpp"
// #include "kits/camera_service/CameraBase.h"
#include "BaumerCamera.h"
#include <QDebug>
#include <QObject>
#include <yaml-cpp/yaml.h>

namespace _Kits
{
class BaumerManager : public QObject
{
    Q_OBJECT
  public:
    explicit BaumerManager();
    ~BaumerManager();
    virtual void init(const YAML::Node &config);
    void searchCamera();
    void captureImageStart();
    void captureThreadFunc();

    void startCaptureThread();
    void stopCaptureThread();

  signals:
    void signalsSearch();                                            // 寻找相机
    void signalsImgGet(const std::string &, QImage &);               // 收到图像
    void stopCaptured(std::string snNumber, uint32_t frameCount);    // 停止采集
    void subtitleSet(int x_StartPo, int y_StartPos, int offset_dis); // 加字幕
    void subtitleChanged(std::vector<std::string> &subtitle);        // 字幕

  protected:
    void initializeBGAPI();
    void deinitializeBGAPI();
    void stop();
    bool addCamera(const std::string &snNumber, BGAPI2::Device *dev);
    bool forceIP(const std::string &snNumber, BGAPI2::NodeMap *nodeMap);

  private slots:
    void recvImage(const std::string &, const QImage &);

  private:
    std::atomic_bool bHold_{true};
    std::mutex mtxCamera_;
    std::string interfaceIp_;
    std::string interfaceMask_;
    std::string interfaceMAC_;
    uint32_t u32InterfaceIp_ = 0;
    uint32_t u32InterfaceMask_ = 0;
    YAML::Node nodeParams_;
    std::unordered_map<std::string, std::unique_ptr<BaumerCamera>>
        mapCameras_; // 存储每个baumerCamera 用SNnum对应
    std::mutex mtxCount_;
    std::unordered_map<std::string, uint32_t> mapCameraCounts_;
    // std::unordered_map<std::string, CameraInfo>
    //     mapCameraInfo_; // 保存每个baumerCamera参数，以相机SNNUM对应
    BGAPI2::System *pSystem_ = nullptr;
    std::unordered_map<std::string, std::queue<QImage>> m_mapImage;
    std::thread captureThread_;
    // std::atomic_bool captureThread_;
    std::atomic_bool bCaptureRunning_{false};
    // std::atomic_bool bCollect{false};
    // std::condition_variable conCollect_;

    // QTimer *timerSearch_;
};

} // namespace _Kits
