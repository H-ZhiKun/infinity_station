#pragma once

#include "SaveVideoBase.h"
#include "kits/camera_service/SaveVideoBase.h"
#include "kits/camera_service/encodevideo.h"
#include "kits/camera_service/encodevideoffmpeg.h"
#include "kits/camera_service/muxingvideo.h"
#include <QImage>
#include <QObject>
#include <atomic>
#include <memory>
#include <queue>
#include <thread>
#include <condition_variable>

namespace _Kits
{
class SaveVideoCache : public SaveVideoBase
{
    Q_OBJECT

  public:
    SaveVideoCache(QObject *parent = nullptr);

    virtual ~SaveVideoCache();

    virtual int initSaveVideo(const QVariant &var);
    virtual int setSubtitle(int x_StartPos, int y_StartPos, int offset_dis);

  private:
    void threadRecoed();
    void writeVideofunc(int no, AVPacket &pkt, int flag);
    void encodeMergedVideo(const QImage& image);

  public slots:
    // 存入缓存区
    // 不能停止缓存
    virtual void pushImage(const QImage &img);

    // 开始录像
    // 如果有重复则需要写进一条视频中
    virtual int startRecord(const QVariant &var, std::string &filepath);

    // 停止录像
    // 需要再将后几秒给写入
    virtual void stopRecord();

  private:
    static constexpr short _M_CACHE_COUNT = 2;

    std::unique_ptr<MuxingVideo> m_muxingVideo = nullptr;
    std::unique_ptr<EncodeVideo> m_encodeVideo = nullptr;

    std::atomic_bool m_isrecordvideo = false;
    std::vector<std::thread> m_vec_thread_record;
    std::condition_variable m_cond_record;
    std::mutex m_mutex_record;

    EncodeVideo::InputInfo m_videoinputinfo;

    // 修改缓存队列类型为智能指针
    std::deque<std::shared_ptr<QImage>> m_preRecordCache;  // 预录缓存
    std::deque<std::shared_ptr<QImage>> m_postRecordCache; // 正式录制缓存
    // 录制状态标记
    std::atomic<bool> m_isRecording{false};
    std::atomic<bool> m_isPreRecording{false};
    std::atomic<bool> m_isBeforeRecording{false};
    // 预录缓存同步锁
    std::mutex m_preCacheMutex;

    std::thread m_save_thread;
    std::thread m_recordThread;
};

} // namespace _Kits