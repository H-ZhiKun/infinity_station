#pragma once
#include "kits/camera_service/encodevideoffmpeg.h"
#include "kits/camera_service/muxingvideo.h"
#include "kits/camera_service/encodevideo.h"
#include <QImage>
#include <queue>
#include <memory>
#include <atomic>
#include <QObject>
#include <thread>
#include <condition_variable>
#include "SaveVideoBase.h"

namespace _Kits
{


class SaveVideo:public SaveVideoBase
{

    Q_OBJECT

  public:
    SaveVideo();
    ~SaveVideo();
    int initSaveVideo(const QVariant &var) override;
    int setSubtitle(int x_StartPos, int y_StartPos, int offset_dis) override { return 0;};
private:
    void threadRecoed();
    
private:
    void writeVideofunc(int no,AVPacket& pkt,int flag);

public slots:
    void pushImage(const QImage& img);
    int startRecord(const QVariant &var, std::string& filepath);
    void stopRecord();
private:
    std::unique_ptr<MuxingVideo> m_muxingVideo = nullptr;
    std::unique_ptr<EncodeVideo> m_encodeVideo = nullptr;

    std::atomic_bool m_isrecordvideo = false;
    std::vector<std::thread> m_vec_thread_record;
    std::condition_variable m_cond_record;
    std::mutex m_mutex_record;
    std::deque<QImage> m_queue_record;
    EncodeVideo::InputInfo m_videoinputinfo;

};

}

