#include "kits/camera_service/SaveVideoCache.h"
#include <QDebug>
#include <filesystem>
#include <memory>

namespace _Kits
{
    SaveVideoCache::SaveVideoCache(QObject *parent) : SaveVideoBase(nullptr)
    {
    }

    SaveVideoCache::~SaveVideoCache()
    {
        m_isrecordvideo.store(false);
        stopRecord();
    }

    int SaveVideoCache::initSaveVideo(const QVariant &var)
    {

        auto varmap = var.toMap();
        int num = varmap.value("num", 1).toInt();
        m_videoinputinfo.avcodeid = AV_CODEC_ID_H264;
        m_videoinputinfo.bit_rate = 2500000;
        m_videoinputinfo.width = 1280;
        m_videoinputinfo.height = 760;
        m_videoinputinfo.rkencode = 1;
        m_videoinputinfo.fps = 20;
        m_videoinputinfo.pixfmt = AV_PIX_FMT_YUV420P;
        m_videoinputinfo.codecname = "h264_qsv";

        m_muxingVideo = std::make_unique<MuxingVideo>();
        m_encodeVideo = std::make_unique<FFMPEGEncodeVideo>();
        m_isrecordvideo.store(true);
        // for (size_t i = 0; i < num; i++)
        // {
        //     /* code */
        //     m_vec_thread_record.emplace_back(
        //         std::thread(&SaveVideoCache::threadRecoed, this));
        // }
        return 0;
    }

    void SaveVideoCache::stopRecord()
    {

        if (nullptr != m_encodeVideo && nullptr != m_muxingVideo)
        {
            m_encodeVideo->EndEncode();
            m_muxingVideo->finished();
        }
        if (m_save_thread.joinable())
        {
            m_save_thread.join();
        }

    }

    int SaveVideoCache::startRecord(const QVariant &var, std::string &filepath)
    {
        static std::string now_filename;

        if (m_isRecording.load())
        {
            qWarning() << "Recording is already in progress";
            filepath = now_filename; // 返回当前文件名
            return 3;
        }

        m_isRecording.store(true);
        if (m_recordThread.joinable())
        {
            m_recordThread.join();
        }
        now_filename = filepath; // 更新当前文件名

        if (m_save_thread.joinable())
        {
            m_save_thread.join();
        }

        if (nullptr != m_encodeVideo && nullptr != m_muxingVideo)
        {
            m_save_thread = std::thread([this, filepath]() {
                try
                {
                    // 获取预录缓存（仅复制智能指针）
                    std::deque<std::shared_ptr<QImage>> preFrames;
                    {
                        std::lock_guard<std::mutex> lock(m_preCacheMutex);
                        m_isPreRecording.store(true);
                        m_isBeforeRecording.store(true);
                        preFrames = m_preRecordCache; // 浅拷贝
                    }

                    // 初始化编码器
                    auto writeCallback = std::bind(&SaveVideoCache::writeVideofunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

                    if (0 != m_encodeVideo->Initencoder(m_videoinputinfo, 0, writeCallback))
                    {
                        m_isRecording.store(false);
                        m_isPreRecording.store(false);
                        m_isBeforeRecording.store(false);
                        m_postRecordCache.clear();
                        return 1;
                    }

                    // 初始化混流器
                    std::vector<EncodeVideo::InputInfo> vecVideoInfo{m_videoinputinfo};
                    if (0 != m_muxingVideo->Init(filepath.c_str(), vecVideoInfo))
                    {
                        m_isRecording.store(false);
                        m_isPreRecording.store(false);
                        m_isBeforeRecording.store(false);
                        m_postRecordCache.clear();
                        return 2;
                    }

                    // 编码预录帧
                    for (const auto &frame_ptr : preFrames)
                    {
                        if (frame_ptr)
                        {
                            encodeMergedVideo(*frame_ptr); // 解引用智能指针
                        }
                    }
                    m_isPreRecording.store(false);

                    // 等待后5秒数据
                    std::deque<std::shared_ptr<QImage>> postFrames;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex_record);
                        m_cond_record.wait_for(lock, std::chrono::seconds(5), [&] { return m_postRecordCache.size() >= m_videoinputinfo.fps * 5; });
                        m_isBeforeRecording.store(false);
                        postFrames = m_postRecordCache; // 浅拷贝
                    }

                    // 编码后续帧
                    for (const auto &frame_ptr : postFrames)
                    {
                        if (frame_ptr)
                        {
                            encodeMergedVideo(*frame_ptr);
                        }
                    }

                    // 清理缓存
                    {
                        std::lock_guard<std::mutex> lock(m_mutex_record);
                        m_postRecordCache.clear();
                    }

                    m_encodeVideo->EndEncode();
                    m_muxingVideo->finished();
                    m_isRecording.store(false);
                }
                catch (const std::exception &e)
                {
                    qCritical() << "Recording thread crashed:" << e.what();
                    m_isRecording.store(false);
                    m_isPreRecording.store(false);
                    m_isBeforeRecording.store(false);
                    m_postRecordCache.clear();
                }

                return 0;
            });
        }
        return 0;
    }

    void SaveVideoCache::writeVideofunc(int no, AVPacket &pkt, int flag)
    {
        if (nullptr != m_muxingVideo && m_isrecordvideo.load())
        {
            m_muxingVideo->writeVideo(0, pkt, 0);
            // m_muxingVideo->writeVideo(0, pkt, 0);
        }
    }

    void SaveVideoCache::pushImage(const QImage &img)
    {
        if (!m_isrecordvideo.load())
            return;

        // 创建共享指针并深拷贝图像（保证线程安全）
        auto frame_ptr = std::make_shared<QImage>(img.copy());

        // 写入预录缓存
        if (!m_isPreRecording.load())
        {
            std::lock_guard<std::mutex> lock(m_preCacheMutex);
            m_preRecordCache.push_back(frame_ptr);

            // 保持5秒缓存量
            const int max_pre_frames = m_videoinputinfo.fps * 5;
            while (m_preRecordCache.size() > max_pre_frames)
            {
                m_preRecordCache.pop_front();
            }
        }

        // 写入正式录制队列
        if (m_isBeforeRecording.load())
        {
            std::lock_guard<std::mutex> lock(m_mutex_record);
            m_postRecordCache.push_back(frame_ptr); // 共享同一份数据

            const int max_post_frames = m_videoinputinfo.fps * 5;
            if (m_postRecordCache.size() >= max_post_frames)
            { // 达到阈值时通知
                m_cond_record.notify_one();
            }
        }
    }

    // 新增合并编码方法
    void SaveVideoCache::encodeMergedVideo(const QImage &image)
    {
        switch (image.format())
        {
        case QImage::Format_RGB888:
            m_encodeVideo->WriteRGBData(image.constBits(), image.width(), AV_PIX_FMT_RGB24);
            break;
        case QImage::Format_BGR888:
            m_encodeVideo->WriteRGBData(image.constBits(), image.width(), AV_PIX_FMT_BGR24);
            break;
        case QImage::Format_Grayscale8:
            m_encodeVideo->WriteRGBData(image.constBits(), image.width(), AV_PIX_FMT_GRAY8);
            //m_encodeVideo->WriteRGBData(image.constBits(), image.width(), AV_PIX_FMT_GRAY16);
            break;
        default:
            qWarning() << "Unsupported image format:" << image.format();
            break;
        }
    }

    void SaveVideoCache::threadRecoed()
    {

        return;
    }
    int SaveVideoCache::setSubtitle(int x_StartPos, int y_StartPos, int offset_dis)
    {
        return 0;
    }

} // namespace _Kits
