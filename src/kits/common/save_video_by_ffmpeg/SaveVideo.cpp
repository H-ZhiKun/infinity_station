#include "SaveVideo.h"
#include <QDebug>
#include <filesystem>

namespace _Kits::SaveVideoNew
{
    SaveVideo::SaveVideo() : SaveVideoBase(nullptr), m_isrecordvideo(false), m_isrecord(false)
    {
    }

    SaveVideo::~SaveVideo()
    {
        m_isrecordvideo.store(false);
        m_cond_record.notify_all();
        stopRecord();

        for (auto &thread : m_vec_thread_record)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

    int SaveVideo::initSaveVideo(const int &threadNum)
    {
        m_videoinputinfo.avcodeid = AV_CODEC_ID_H264;
        m_videoinputinfo.bit_rate = 2500000;
        m_videoinputinfo.width = 1280;
        m_videoinputinfo.height = 1024;
        m_videoinputinfo.rkencode = 1;
        m_videoinputinfo.fps = 10;
        m_videoinputinfo.pixfmt = AV_PIX_FMT_YUV420P;
        m_videoinputinfo.codecname = "h264";

        m_muxingVideo = std::make_unique<MuxingVideo>();
        m_encodeVideo = std::make_unique<FFMPEGEncodeVideo>();
        m_isrecordvideo.store(true);
        for (size_t i = 0; i < threadNum; i++)
        {
            /* code */
            m_vec_thread_record.emplace_back(std::thread(&SaveVideo::threadRecoed, this));
        }
        return 0;
    }
    void SaveVideo::stopRecord()
    {

        if (nullptr != m_encodeVideo && nullptr != m_muxingVideo)
        {
            m_encodeVideo->EndEncode();
            m_muxingVideo->finished();
        }

        m_isrecord.store(false);
    }

    int SaveVideo::startRecord(QString &filepath)
    {
        if (nullptr != m_encodeVideo && nullptr != m_muxingVideo)
        {
            m_isrecord.store(true);

            auto writecallfunck =
                std::bind(&SaveVideo::writeVideofunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            if (0 != m_encodeVideo->Initencoder(m_videoinputinfo, 0, writecallfunck))
            {
                return 1;
            }
            std::vector<EncodeVideo::InputInfo> vecVideoInfo;
            vecVideoInfo.reserve(4);
            vecVideoInfo.push_back(std::move(m_videoinputinfo));
            if (0 != m_muxingVideo->Init(filepath.toUtf8().data(), vecVideoInfo))
            {
                return 2;
            }
        }
        return 0;
    }

    void SaveVideo::writeVideofunc(int no, AVPacket &pkt, int flag)
    {
        if (nullptr != m_muxingVideo && m_isrecordvideo.load())
        {
            m_muxingVideo->writeVideo(0, pkt, 0);
        }
    }
    void SaveVideo::pushOriginalImage(std::shared_ptr<TIS_Info::ImageBuffer> img)
    {
        if (m_isrecordvideo.load())
        {
            std::unique_lock<std::mutex> lock(m_mutex_record);
            m_queue_original_record.push_back(img);
            m_cond_record.notify_one();
        }
    }

    void SaveVideo::threadRecoed()
    {
        while (m_isrecordvideo)
        {
            std::unique_lock<std::mutex> lock(m_mutex_record);
            m_cond_record.wait(lock, [this] { return !m_queue_original_record.empty() || !m_isrecordvideo; });

            if (!m_isrecordvideo)
            {
                return;
            }

            auto image = m_queue_original_record.front();
            m_queue_original_record.pop_front();

            if (m_videoinputinfo.width != image->width || m_videoinputinfo.height != image->height)
            {
                m_videoinputinfo.width = image->width;
                m_videoinputinfo.height = image->height;
            }

            if (!m_isrecord.load())
            {
                continue;
            }

            while (m_queue_original_record.size() > 20)
            {
                m_queue_original_record.pop_front();
            }

            if (nullptr != m_encodeVideo)
            {

                m_encodeVideo->WriteRGBData(image);

                // qDebug() << "write video:" << m_queue_record.size();
            }
        }
    }

} // namespace _Kits::SaveVideoNew