#pragma once

#include "kits/camera_service/CameraBase.h"
#include "kits/camera_service/SaveVideoBase.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

class CameraCtrl : public QObject
{
    Q_OBJECT
  private:
    struct ImageData
    {
        QImage image;
        std::string camera_name;
    };

  public:
    CameraCtrl(/* args */);
    ~CameraCtrl();
    // void initAllCameraFactory();
    int createObjectByType(const std::string &type);
    int start(const YAML::Node &config);
    int setSubtitle(int x_StartPos, int y_StartPos, int offset_dis);
    int addVideoSave();
  public slots:
    int onGetImage(const QImage image, const std::string &camera_name);
    void onStartRecord(const std::string &filename);
    void onStopRecord();
    void onSubtitleChanged(std::vector<std::string> &subtitle);
    void onCacheStartRecord(const QVariant &data);
    void onVideoSavePathChanged(std::string path);

  signals:
    void popImageSignals(const QVariant &var);
    void imageOutPut(const QImage &image, const std::string &camera_name);
    void flagRecord(bool flag);

    void sendPathToArcuv(const QVariant &path); // 发送保存路径至燃弧
  private:
    struct arcData
    {
        std::string duration;  // 燃弧持续时间
        std::string occurtime; // 燃弧发生时间
        std::string arccount;  // 燃弧总数
        std::string arctime;   // 燃弧时间
        std::string arcpulse;  // 燃弧脉冲
        uint16_t arcId = 0;    // 燃弧id
    };

    void setOSD(QImage &image);
    void imageConsumerThread();
    std::string arcFileNameSet(const QVariant &data, arcData &arc_data);

  private:
    std::shared_ptr<_Kits::CameraBase> m_camera_base = nullptr;
    std::shared_ptr<_Kits::SaveVideoBase> m_save_video_base;
    std::shared_ptr<_Kits::SaveVideoBase> m_cachesave_video_base;
    // std::vector<std::shared_ptr<_Kits::SaveVideoBase>> m_saveVideoList;
    std::atomic_bool m_isclose = true;

    std::deque<ImageData> m_imageQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    std::atomic_bool m_consumer_running = false;
    std::vector<std::thread> m_vec_consumerThread;
    std::atomic_bool m_saveVideoFlag = false;

    std::mutex m_mutexSubAdd;
    std::vector<std::string> m_vec_subtitle;

    int m_iyStartPos = 80;
    int m_ioffsetdis = 30;
    int m_ixStartPos = 3;

    std::string m_str_Video_SavePath = "";
    std::string m_str_CameraName = "";
    std::string m_str_SaveVideoType = "";
};
