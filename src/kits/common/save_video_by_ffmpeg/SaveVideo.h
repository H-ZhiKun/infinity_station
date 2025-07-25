#pragma once
#include "SaveVideoBase.h"
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
        class SaveVideo : public SaveVideoBase
        {

            Q_OBJECT

          public:
            SaveVideo();
            virtual ~SaveVideo();
            int initSaveVideo(const int &threadNum) override;

          private:
            void threadRecoed();

          private:
            void writeVideofunc(int no, AVPacket &pkt, int flag);

          public slots:
            virtual void pushOriginalImage(std::shared_ptr<TIS_Info::ImageBuffer> img) override;
            virtual int startRecord(QString &filepath) override;
            virtual void stopRecord() override;

          private:
            std::unique_ptr<MuxingVideo> m_muxingVideo = nullptr;
            std::unique_ptr<EncodeVideo> m_encodeVideo = nullptr;

            std::atomic_bool m_isrecordvideo = false;
            std::vector<std::thread> m_vec_thread_record;
            std::condition_variable m_cond_record;
            std::mutex m_mutex_record;
            std::deque<std::shared_ptr<TIS_Info::ImageBuffer>> m_queue_original_record;
            EncodeVideo::InputInfo m_videoinputinfo;

            std::atomic_bool m_isrecord = false;
        };
    } // namespace SaveVideoNew
} // namespace _Kits
