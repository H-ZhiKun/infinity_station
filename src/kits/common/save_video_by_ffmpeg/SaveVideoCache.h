#pragma once

#include "encodevideo.h"
#include "encodevideoffmpeg.h"
#include "muxingvideo.h"
#include <QImage>
#include <QObject>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <queue>
#include <thread>

namespace _Kits
{
    namespace SaveVideoNew
    {
        class SaveVideoCache : public QObject
        {
            Q_OBJECT

          public:
            SaveVideoCache(QObject *parent = nullptr);

            virtual ~SaveVideoCache();

            virtual int initSaveVideo(const int &threadNum);

          private:
            void threadRecoed();
            void writeVideofunc(int no, AVPacket &pkt, int flag);
            void encodeMergedVideo(const std::shared_ptr<TIS_Info::ImageBuffer> image);

          public slots:

            // 开始录像
            // 这是一个一次性操作，只会录制一个短视频，包含前 * 后 * 秒
            // 如果有重复则需要写进一条视频中
            virtual int startRecord(QString &filepath, std::function<void(int result)> callback);

            // 停止录像
            // 一般来说只有结束时自己调用一次
            virtual void stopRecord();

            virtual void pushOriginalImage(std::shared_ptr<TIS_Info::ImageBuffer> img);

          private:
            static constexpr short _M_CACHE_COUNT = 2;

            std::unique_ptr<MuxingVideo> m_muxingVideo = nullptr;
            std::unique_ptr<EncodeVideo> m_encodeVideo = nullptr;

            std::atomic_bool m_isrecordvideo = false;
            std::vector<std::thread> m_vec_thread_record;
            std::condition_variable m_cond_record;
            std::mutex m_mutex_record;

            EncodeVideo::InputInfo m_videoinputinfo;

            std::deque<std::shared_ptr<TIS_Info::ImageBuffer>> m_pre_original_record;  // 预录缓存
            std::deque<std::shared_ptr<TIS_Info::ImageBuffer>> m_post_original_record; // 正式录制缓存

            // 录制状态标记
            std::atomic<bool> m_isRecording{false};
            std::atomic<bool> m_isPreRecording{false};
            std::atomic<bool> m_isBeforeRecording{false};
            // 预录缓存同步锁
            std::mutex m_preCacheMutex;

            std::thread m_save_thread;
        };
    } // namespace SaveVideoNew
} // namespace _Kits