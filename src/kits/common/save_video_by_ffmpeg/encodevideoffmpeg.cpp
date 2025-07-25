#include "encodevideoffmpeg.h"
#include "kits/common/log/CRossLogger.h"
#include <QDebug>

using namespace _Kits;
using namespace _Kits::SaveVideoNew;
FFMPEGEncodeVideo::FFMPEGEncodeVideo(/* args */)
{

    av_log_set_level(AV_LOG_QUIET);
}

FFMPEGEncodeVideo::~FFMPEGEncodeVideo()
{
    Release();
}

int FFMPEGEncodeVideo::Initencoder(InputInfo &encoderinfo, int srcindex, std::function<void(int, AVPacket &, int)> writefilecallback)
{
    avdevice_register_all();

    m_srcindex = srcindex;
    m_writefilecallback = writefilecallback;
    if (AV_CODEC_ID_H264 == (AVCodecID)encoderinfo.avcodeid)
    {
        m_encodecodec = const_cast<AVCodec *>(avcodec_find_encoder_by_name(encoderinfo.codecname.c_str()));
        if (nullptr == m_encodecodec)
        {
            qDebug() << "not find codec:" << encoderinfo.codecname.c_str();
            m_encodecodec = const_cast<AVCodec *>(avcodec_find_encoder((AVCodecID)encoderinfo.avcodeid));
            if (nullptr == m_encodecodec)
            {
                LogError("Could not find encoder for :%s", avcodec_get_name((AVCodecID)encoderinfo.avcodeid));
                qDebug() << "Could not find encoder for :%s" << avcodec_get_name((AVCodecID)encoderinfo.avcodeid);
                return -1;
            }
        }
    }
    else
    {
        m_encodecodec = const_cast<AVCodec *>(avcodec_find_encoder((AVCodecID)encoderinfo.avcodeid));
        if (nullptr == m_encodecodec)
        {
            LogError("Could not find encoder for 2:%s", avcodec_get_name((AVCodecID)encoderinfo.avcodeid));
            qDebug() << "Could not find encoder for 2:%s" << avcodec_get_name((AVCodecID)encoderinfo.avcodeid);
            return -1;
        }
    }

    m_codecContext = avcodec_alloc_context3(m_encodecodec);
    if (nullptr == m_codecContext)
    {
        LogError("Could not alloc an encoding context");
        qDebug() << "Could not alloc an encoding context";
        return -1;
    }
    switch ((m_encodecodec)->type)
    {
    case AVMEDIA_TYPE_VIDEO: {
        m_codecContext->codec_id = (AVCodecID)encoderinfo.avcodeid;
        m_codecContext->bit_rate = encoderinfo.bit_rate;
        m_codecContext->width = encoderinfo.width;
        m_codecContext->height = encoderinfo.height;
        // m_codecContext->time_base = (AVRational){1, encoderinfo.fps};
        m_codecContext->time_base.den = encoderinfo.fps;
        m_codecContext->time_base.num = 1;
        // m_codecContext->framerate = (AVRational){encoderinfo.fps, 1};

        m_codecContext->gop_size = encoderinfo.fps / 2; /* emit one intra frame every twelve frames at most */
        m_codecContext->pix_fmt = (AVPixelFormat)encoderinfo.pixfmt;
        if (m_codecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO)
        {
            //     /* just for testing, we also add B-frames */
            m_codecContext->max_b_frames = 2;
        }
        break;
    }
    default:
        break;
    }
    OpenVideo(nullptr);
    m_frameindex = 0;
    m_initialized = true;

    LogInfo("FFMPEGEncodeVideo Initecnoder successed!");
    qDebug() << "FFMPEGEncodeVideo Initecnoder successed!";
    return 0;
}

void FFMPEGEncodeVideo::OpenVideo(AVDictionary *opt_arg)
{
    AVDictionary *opt = NULL;
    av_dict_copy(&opt, opt_arg, 0);

    /* open the codec */
    int ret = avcodec_open2(m_codecContext, m_encodecodec, &opt);
    // int ret = avcodec_open2(m_codecContext, m_encodecodec, nullptr);
    av_dict_free(&opt);
    if (ret < 0)
    {
        LogError("Could not open video codec: %d", ret);
        qDebug() << "Could not open video codec: %d" << ret;
        return;
    }
    m_avframe = av_frame_alloc();
    if (nullptr == m_avframe)
    {
        return;
    }
    m_avframe->format = m_codecContext->pix_fmt;
    m_avframe->width = m_codecContext->width;
    m_avframe->height = m_codecContext->height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(m_avframe, 0);
    if (ret < 0)
    {
        LogError("Could not allocate frame data");
        qDebug() << "Could not allocate frame data";
        return;
    }
}

int FFMPEGEncodeVideo::WriteRGBData(const uint8_t *rgb, int width, int srcpixfmt)
{
    std::lock_guard<std::mutex> lock(m_write_mutex);
    //  std::cout << "FFMPEGEncodeVideo::WriteRGBData step1\r\n";
    if (!m_initialized)
    {
        return -1;
    }
    auto ret = av_frame_make_writable(m_avframe);
    if (ret < 0)
    {
        return -1;
    }

    if (nullptr == m_swscontext)
    {
        InitSwsContext((enum AVPixelFormat)srcpixfmt);
    }

    if (nullptr == rgb || nullptr == m_codecContext)
    {
        //    std::cout << "data is nullptr\r\n";
        return -1;
    }

    if (AV_PIX_FMT_YUV420P == m_codecContext->pix_fmt && AV_PIX_FMT_GRAY8 != srcpixfmt)
    {
        const int in_linesize[1] = {width * 3};
        sws_scale(m_swscontext,
                  &rgb,
                  in_linesize,
                  0,
                  m_avframe->height, // src
                  m_avframe->data,
                  m_avframe->linesize); // dst
    }
    else
    {

        sws_scale(m_swscontext,
                  &rgb,
                  m_avframe->linesize,
                  0,
                  m_avframe->height, // src
                  m_avframe->data,
                  m_avframe->linesize); // dst
    }

    m_avframe->pts = m_frameindex++;

    encode(m_avframe);
    //   std::cout << "FFMPEGEncodeVideo WriteRGBData step5\r\n";
    return 0;
}

int FFMPEGEncodeVideo::WriteRGBData(const uint8_t *buffer, int width, int height, int srcpixfmt)
{
    std::lock_guard<std::mutex> lock(m_write_mutex);

    if (!m_initialized)
    {
        // 处理未初始化的情况
        return -1;
    }

    int ret = av_frame_make_writable(m_avframe);
    if (ret < 0)
    {
        // 处理帧不可写的情况
        return -1;
    }

    if (nullptr == m_swscontext)
    {
        InitSwsContext((enum AVPixelFormat)srcpixfmt);
    }

    if (nullptr == buffer || nullptr == m_codecContext)
    {
        // 处理输入数据或编码器上下文为空的情况
        return -1;
    }

    if (AV_PIX_FMT_YUV420P == m_codecContext->pix_fmt && AV_PIX_FMT_GRAY8 != srcpixfmt)
    {
        const int in_linesize[1] = {width * height * 3};
        sws_scale(m_swscontext, &buffer, in_linesize, 0, m_avframe->height, m_avframe->data, m_avframe->linesize);
    }
    else
    {
        sws_scale(m_swscontext, &buffer, m_avframe->linesize, 0, m_avframe->height, m_avframe->data, m_avframe->linesize);
    }

    m_avframe->pts = m_frameindex++;

    // 这里可以添加编码逻辑，例如调用 encode(m_avframe);
    encode(m_avframe);
    return 0;
}

int FFMPEGEncodeVideo::WriteRGBData(std::shared_ptr<TIS_Info::ImageBuffer> imageData)
{
    std::lock_guard<std::mutex> lock(m_write_mutex);

    //--------------------------------------------------
    // 1. 基本有效性检查
    //--------------------------------------------------
    if (!m_initialized)
    {
        LogError("Encoder not initialized");
        return -1;
    }

    if (!m_codecContext || !m_avframe)
    {
        LogError("Codec context or frame is null");
        return -1;
    }

    if (imageData->data.empty())
    {
        LogError("Input buffer empty");
        return -1;
    }

    const int width = imageData->width;
    const int height = imageData->height;
    const AVPixelFormat srcFormat = static_cast<AVPixelFormat>(imageData->pixFormat);

    //--------------------------------------------------
    // 2. 像素格式合法性
    //--------------------------------------------------
    if (srcFormat <= AV_PIX_FMT_NONE || srcFormat >= AV_PIX_FMT_NB)
    {
        LogError("Invalid pixFormat: {}", static_cast<int>(srcFormat));
        return -1;
    }

    //--------------------------------------------------
    // 3. 分辨率一致性
    //--------------------------------------------------
    if (width != m_avframe->width || height != m_avframe->height)
    {
        LogError("Resolution mismatch: {}x{} vs {}x{}", width, height, m_avframe->width, m_avframe->height);
        return -1;
    }

    //--------------------------------------------------
    // 4. 缓冲区大小检查
    //--------------------------------------------------
    size_t requiredBytes = 0;
    switch (srcFormat)
    {
    case AV_PIX_FMT_YUV420P:
    case AV_PIX_FMT_YUVJ420P:
        requiredBytes = width * height * 3 / 2;
        break;
    case AV_PIX_FMT_GRAY8:
        requiredBytes = width * height;
        break;
    case AV_PIX_FMT_RGB24:
    case AV_PIX_FMT_BGR24:
        requiredBytes = width * height * 3;
        break;
    case AV_PIX_FMT_RGBA:
    case AV_PIX_FMT_BGRA:
        requiredBytes = width * height * 4;
        break;
    default:
        LogError("Unsupported pixel format in size check");
        return -1;
    }

    if (imageData->data.size() < requiredBytes)
    {
        LogError("Buffer too small: %zu < %zu", imageData->data.size(), requiredBytes);
        return -1;
    }

    //--------------------------------------------------
    // 5. 确保帧可写
    //--------------------------------------------------
    int ret = av_frame_make_writable(m_avframe);
    if (ret < 0)
    {
        char buf[AV_ERROR_MAX_STRING_SIZE]{};
        av_strerror(ret, buf, sizeof(buf));
        LogError("Frame not writable: {}", buf);
        return -1;
    }

    //--------------------------------------------------
    // 6. 初始化/更新 sws_context
    //--------------------------------------------------
    if (!m_swscontext || m_lastSrcFormat != srcFormat || m_lastWidth != width || m_lastHeight != height)
    {
        if (m_swscontext)
        {
            sws_freeContext(m_swscontext);
            m_swscontext = nullptr;
        }

        InitSwsContext(srcFormat);
        if (!m_swscontext)
        {
            LogError("SWS context init failed");
            return -1;
        }

        m_lastSrcFormat = srcFormat;
        m_lastWidth = width;
        m_lastHeight = height;
    }

    //--------------------------------------------------
    // 7. 准备 sws_scale 输入
    //--------------------------------------------------
    const uint8_t *src_data[AV_NUM_DATA_POINTERS]{};
    int src_linesize[AV_NUM_DATA_POINTERS]{};

    switch (srcFormat)
    {
    case AV_PIX_FMT_YUV420P:
    case AV_PIX_FMT_YUVJ420P:
        setupYUV420PInput(imageData->data.data(), width, height, src_data, src_linesize);
        break;
    case AV_PIX_FMT_GRAY8:
        src_data[0] = imageData->data.data();
        src_linesize[0] = width;
        break;
    case AV_PIX_FMT_RGB24:
    case AV_PIX_FMT_BGR24:
        src_data[0] = imageData->data.data();
        src_linesize[0] = width * 3;
        break;
    case AV_PIX_FMT_RGBA:
    case AV_PIX_FMT_BGRA:
        src_data[0] = imageData->data.data();
        src_linesize[0] = width * 4;
        break;
    default:
        LogError("Unsupported pixel format");
        return -1;
    }

    //--------------------------------------------------
    // 8. 执行颜色空间转换
    //--------------------------------------------------
    ret = sws_scale(m_swscontext, src_data, src_linesize, 0, height, m_avframe->data, m_avframe->linesize);

    if (ret < 0)
    {
        char buf[AV_ERROR_MAX_STRING_SIZE]{};
        av_strerror(ret, buf, sizeof(buf));
        LogError("sws_scale failed: {}", buf);
        return -1;
    }

    //--------------------------------------------------
    // 9. 时间戳 & 编码
    //--------------------------------------------------
    m_avframe->pts = m_frameindex++;
    m_avframe->pict_type = AV_PICTURE_TYPE_NONE;

    encode(m_avframe);
    return 0;
}

// YUV420P输入设置辅助函数
void FFMPEGEncodeVideo::setupYUV420PInput(const uint8_t *buffer, int width, int height, const uint8_t **src_data, int *src_linesize)
{
    const int y_size = width * height;
    const int uv_size = y_size / 4; // UV平面大小 (width/2 * height/2)

    src_data[0] = buffer;                    // Y平面
    src_data[1] = buffer + y_size;           // U平面
    src_data[2] = buffer + y_size + uv_size; // V平面

    src_linesize[0] = width;     // Y行大小
    src_linesize[1] = width / 2; // U行大小
    src_linesize[2] = width / 2; // V行大小
}

int FFMPEGEncodeVideo::InitSwsContext(enum AVPixelFormat srcpixfmat)
{
    m_swscontext = sws_getContext(m_avframe->width,
                                  m_avframe->height,
                                  srcpixfmat, // src
                                  m_avframe->width,
                                  m_avframe->height,
                                  m_codecContext->pix_fmt, // dst
                                  SWS_BICUBIC,
                                  NULL,
                                  NULL,
                                  NULL);
    if (nullptr == m_swscontext)
    {
        LogError("could not initialize the conversion context");
        qDebug() << "could not initialize the conversion context";
        // goto cleanup_name_context_stream_context_frame;
        return -1;
    }
    return 0;
}

void FFMPEGEncodeVideo::EndEncode()
{
    Release();
}

void FFMPEGEncodeVideo::Release()
{
    std::lock_guard<std::mutex> lock(m_write_mutex);
    if (nullptr != m_swscontext)
    {
        sws_freeContext(m_swscontext);
        m_swscontext = nullptr;
    }

    if (nullptr != m_avframe)
    {
        av_frame_free(&m_avframe);
        m_avframe = nullptr;
    }

    if (nullptr != m_codecContext)
    {
        avcodec_free_context(&m_codecContext);
        m_codecContext = nullptr;
    }

    if (nullptr != m_codecContext)
    {
        avcodec_close(m_codecContext);
        m_codecContext = nullptr;
    }
    m_initialized = false;
}

void FFMPEGEncodeVideo::encode(const AVFrame *frame)
{
    int ret = avcodec_send_frame(m_codecContext, m_avframe);
    if (ret < 0)
    {
        LogError("error sending a frame for encoding");
        qDebug() << "error sending a frame for encoding";
        return;
    }

    do
    {
        AVPacket pkt = {0};
        ret = avcodec_receive_packet(m_codecContext, &pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            break;
        }
        else if (ret < 0)
        {
            // fprintf(stderr, "error encoding a frame: %s\n", av_err2str(ret));
            LogError("error encoding a frame: %d", ret);
            qDebug() << "error encoding a frame: %d" << ret;
            return;
        }
        if (nullptr != m_writefilecallback)
        {
            m_writefilecallback(m_srcindex, pkt, 0);
        }
        av_packet_unref(&pkt);
    } while (ret >= 0);
}