#include "SaveVideoCache.h"
#include <QDebug>
#include <filesystem>
#include <memory>

namespace _Kits::SaveVideoNew
{
    SaveVideoCache::SaveVideoCache(QObject *parent)
    {
    }

    SaveVideoCache::~SaveVideoCache()
    {
        m_isrecordvideo.store(false);
        stopRecord();
    }

    int SaveVideoCache::initSaveVideo(const int &threadNum)
    {

        int num = threadNum;
        m_videoinputinfo.avcodeid = AV_CODEC_ID_H264;
        m_videoinputinfo.bit_rate = 2500000;
        m_videoinputinfo.width = 1280;
        m_videoinputinfo.height = 1024;
        m_videoinputinfo.rkencode = 1;
        m_videoinputinfo.fps = 10;
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
        m_isRecording.store(false);

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

    int SaveVideoCache::startRecord(QString &filepath, std::function<void(int result)> callback)
    {
        static QString now_filename;

        if (m_isRecording.load())
        {
            qWarning() << "Recording is already in progress";
            filepath = now_filename; // 返回当前文件名
            return 3;
        }

        m_isRecording.store(true);
        now_filename = filepath; // 更新当前文件名

        if (m_save_thread.joinable()) // 先回收上一次的线程资源
        {
            m_save_thread.join();
        }

        if (nullptr != m_encodeVideo && nullptr != m_muxingVideo)
        {
            m_save_thread = std::thread([this, filepath, callback]() {
                try
                {
                    // 获取预录缓存（仅复制智能指针）
                    std::deque<std::shared_ptr<TIS_Info::ImageBuffer>> preFrames;
                    {
                        std::lock_guard<std::mutex> lock(m_preCacheMutex);
                        m_isPreRecording.store(true);
                        m_isBeforeRecording.store(true);
                        preFrames = m_pre_original_record; // 浅拷贝
                    }

                    // 初始化编码器
                    auto writeCallback = std::bind(
                        &SaveVideoCache::writeVideofunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

                    auto first_frame = preFrames.begin();
                    if (first_frame != preFrames.end())
                    {
                        m_videoinputinfo.width = (*first_frame)->width;
                        m_videoinputinfo.height = (*first_frame)->height;
                    }

                    if (0 != m_encodeVideo->Initencoder(m_videoinputinfo, 0, writeCallback))
                    {
                        m_isRecording.store(false);
                        m_isPreRecording.store(false);
                        m_isBeforeRecording.store(false);
                        m_post_original_record.clear();

                        if (callback)
                        {
                            callback(1);
                        }

                        return 1;
                    }

                    // 初始化混流器
                    std::vector<EncodeVideo::InputInfo> vecVideoInfo{m_videoinputinfo};
                    if (0 != m_muxingVideo->Init(filepath.toUtf8().data(), vecVideoInfo))
                    {
                        m_isRecording.store(false);
                        m_isPreRecording.store(false);
                        m_isBeforeRecording.store(false);
                        m_post_original_record.clear();

                        if (callback)
                        {
                            callback(2);
                        }

                        return 2;
                    }

                    // 编码预录帧
                    for (const auto &frame_ptr : preFrames)
                    {
                        if (frame_ptr)
                        {
                            encodeMergedVideo(frame_ptr); // 解引用智能指针
                        }
                    }
                    m_isPreRecording.store(false);

                    // 等待后5秒数据
                    std::deque<std::shared_ptr<TIS_Info::ImageBuffer>> postFrames;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex_record);
                        m_cond_record.wait_for(
                            lock, std::chrono::seconds(5), [&] { return m_post_original_record.size() >= m_videoinputinfo.fps * 5; });
                        m_isBeforeRecording.store(false);
                        postFrames = m_post_original_record; // 浅拷贝
                    }

                    // 编码后续帧
                    for (const auto &frame_ptr : postFrames)
                    {
                        if (frame_ptr)
                        {
                            encodeMergedVideo(frame_ptr);
                        }
                    }

                    // 清理缓存
                    {
                        std::lock_guard<std::mutex> lock(m_mutex_record);
                        m_post_original_record.clear();
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
                    m_post_original_record.clear();
                }

                if (callback)
                {
                    callback(0);
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
            m_muxingVideo->writeVideo(0, pkt, 0);
        }
    }

    void SaveVideoCache::pushOriginalImage(std::shared_ptr<TIS_Info::ImageBuffer> img)
    {
        if (!m_isrecordvideo.load())
            return;

        // 写入预录缓存
        if (!m_isPreRecording.load())
        {
            std::lock_guard<std::mutex> lock(m_preCacheMutex);
            m_pre_original_record.push_back(img);

            // 保持5秒缓存量
            const int max_pre_frames = m_videoinputinfo.fps * 5;
            while (m_pre_original_record.size() > max_pre_frames)
            {
                m_pre_original_record.pop_front();
            }
        }

        // 写入正式录制队列
        if (m_isBeforeRecording.load())
        {
            std::lock_guard<std::mutex> lock(m_mutex_record);
            m_post_original_record.push_back(img); // 共享同一份数据

            const int max_post_frames = m_videoinputinfo.fps * 5;
            if (m_post_original_record.size() >= max_post_frames)
            { // 达到阈值时通知
                m_cond_record.notify_one();
            }
        }
    }

    // 新增合并编码方法
    void SaveVideoCache::encodeMergedVideo(std::shared_ptr<TIS_Info::ImageBuffer> image)
    {
        m_encodeVideo->WriteRGBData(image);
    }

    void SaveVideoCache::threadRecoed()
    {

        return;
    }

} // namespace _Kits::SaveVideoNew
