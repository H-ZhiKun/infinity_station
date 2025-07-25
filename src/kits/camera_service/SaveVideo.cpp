#include "SaveVideo.h"
#include <QDebug>
#include <filesystem>

namespace _Kits
{
SaveVideo::SaveVideo()
: SaveVideoBase(nullptr)
{
}

SaveVideo::~SaveVideo()
{
    m_isrecordvideo.store(false);
    m_cond_record.notify_all();

    for (auto &thread : m_vec_thread_record)
    {
        if (thread.joinable())
        {
            thread.join();
        }
        
    }
}

int SaveVideo::initSaveVideo(const QVariant &var)
{
    auto varmap = var.toMap();
    int num = varmap.value("num",1).toInt();

    m_videoinputinfo.avcodeid =  AV_CODEC_ID_H264;
    m_videoinputinfo.bit_rate = 2500000;
    m_videoinputinfo.width = 1280;
    m_videoinputinfo.height = 760;
    m_videoinputinfo.rkencode = 1;
    m_videoinputinfo.fps = 20;
    m_videoinputinfo.pixfmt = AV_PIX_FMT_YUV420P;
    m_videoinputinfo.codecname = "h264";

    m_muxingVideo = std::make_unique<MuxingVideo>();
    m_encodeVideo = std::make_unique<FFMPEGEncodeVideo>();
    m_isrecordvideo.store(true);
    for (size_t i = 0; i < num; i++)
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
}

int SaveVideo::startRecord(const QVariant &var, std::string& filepath)
{
    if (nullptr != m_encodeVideo && nullptr != m_muxingVideo)
	{
        stopRecord();
        
		auto writecallfunck = std::bind(&SaveVideo::writeVideofunc, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3); 
		if(0 != m_encodeVideo->Initencoder(m_videoinputinfo,0,writecallfunck))
        {
            return 1;
        }
		std::vector<EncodeVideo::InputInfo> vecVideoInfo;
		vecVideoInfo.emplace_back(m_videoinputinfo);
      //  auto filepath = m_cameraInfo.mstr_Video_SavePath + "/" + m_cameraInfo.mstr_CameraName + "/" + filename + "." + m_cameraInfo.mstr_SaveVideoType;
		if(0 != m_muxingVideo->Init(filepath.c_str(), vecVideoInfo))
        {
            return 2;
        }
	}
    return 0;
}

void SaveVideo::writeVideofunc(int no,AVPacket& pkt,int flag)
{
    if (nullptr != m_muxingVideo && m_isrecordvideo.load())
    {
        m_muxingVideo->writeVideo(0,pkt,0);
    }
}

void SaveVideo::pushImage(const QImage& img)
{
    if (m_isrecordvideo.load())
    {
        std::unique_lock<std::mutex> lock(m_mutex_record);
        m_queue_record.push_back(img);
        m_cond_record.notify_one();
    }
}

void SaveVideo::threadRecoed()
{
    while (m_isrecordvideo)
    {
        std::unique_lock<std::mutex> lock(m_mutex_record);
        m_cond_record.wait(lock, [this] { return !m_queue_record.empty() || !m_isrecordvideo; });

        if (!m_isrecordvideo)
        {
            return;
        }
        
        auto image = m_queue_record.front();
        m_queue_record.pop_front();

        while(m_queue_record.size() > 20)
        {
            m_queue_record.pop_back();
        }

        if (nullptr != m_encodeVideo)
        {
            switch (image.format())
            {
            case QImage::Format_RGB888:
                m_encodeVideo->WriteRGBData(image.constBits(),
                image.width(),
                AV_PIX_FMT_RGB24);
                break;
            case QImage::Format_BGR888:
                m_encodeVideo->WriteRGBData(image.constBits(),
                image.width(),
                AV_PIX_FMT_BGR24);
                break;
            case QImage::Format_Grayscale8:
                m_encodeVideo->WriteRGBData(image.constBits(),
                image.width(),
                AV_PIX_FMT_GRAY16);
                break;
            
            default:
                break;
            }

            // qDebug() << "write video:" << m_queue_record.size();
        }

    }
    
}

}