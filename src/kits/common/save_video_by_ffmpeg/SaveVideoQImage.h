#pragma once
#include "encodevideoffmpeg.h"
#include "muxingvideo.h"
#include "encodevideo.h"
#include <QImage>
#include <queue>
#include <memory>
#include <atomic>
#include <QObject>
#include <thread>
#include <condition_variable>

namespace _Kits
{
    namespace SaveVideoNew
    {

        class SaveVideoQImage : public QObject
        {

            Q_OBJECT

          public:
            SaveVideoQImage(QObject *parent = nullptr);
            ~SaveVideoQImage();
            int initSaveVideo(const int &threadNum);

          private:
            void threadRecoed();

          private:
            void writeVideofunc(int no, AVPacket &pkt, int flag);

          public slots:
            void pushImage(const QImage &img);
            int startRecord(QString &filepath);
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

    } // namespace SaveVideoNew
} // namespace _Kits
