#include "kits/camera_service/ffmpeg/CameraUsb.h"
#include "CameraUsb.h"
#include "kits/common/log/CRossLogger.h"
#include <QSharedPointer>
#include <chrono>
#include <libavcodec/codec_id.h>
#include <libavcodec/packet.h>
#include <libavutil/pixfmt.h>
#include <qnamespace.h>
#include <qobject.h>
#include <thread>

#ifdef _WIN32
#include <objbase.h>
#include <qwindowdefs_win.h>
#endif

using namespace _Kits;
CameraUsb::CameraUsb() : CameraBase(nullptr)
{
    avdevice_register_all(); // 注册所有设备
    av_log_set_level(AV_LOG_QUIET);
}

CameraUsb::~CameraUsb()
{
    stopGrab();
    closeCamera();
}

bool CameraUsb::openCamera(const YAML::Node &config)
{
    ReadConfig(config);

    // 设置输入格式（根据操作系统不同）
#if defined(__linux__)
    const char *format = "video4linux2";
#elif defined(_WIN32)
    const char *format = "dshow";
#endif

    std::string input_string = "video=" + m_cameraInfo.mstr_Camera_ID;
    // 打开视频设备
    const AVInputFormat *inputFmt = av_find_input_format(format);
    if (avformat_open_input(&m_formatCtx, input_string.c_str(), inputFmt, nullptr) != 0)
    {
        LogError("Failed to open video device:" + input_string);
        return false;
    }

    // 查找流信息
    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0)
    {
        closeCamera();
        return false;
    }

    // 查找视频流
    for (unsigned i = 0; i < m_formatCtx->nb_streams; i++)
    {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_videoStreamIndex = i;
            break;
        }
    }

    // 获取解码器
    AVCodecParameters *codecPar = m_formatCtx->streams[m_videoStreamIndex]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecPar->codec_id);
    m_codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_codecCtx, codecPar);

    // 打开解码器
    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0)
    {
        closeCamera();
        return false;
    }
    // initSaveVideo(1);
    return true;
}

bool CameraUsb::startGrab()
{
    if (!m_formatCtx || m_grabRunning)
        return false;

    m_grabRunning = true;
    m_grabThread = std::thread(&CameraUsb::grabFrames, this);
    return true;
}

void CameraUsb::grabFrames()
{
    AVPacket packet;
    AVFrame *frame = av_frame_alloc();
    AVFrame *rgbFrame = av_frame_alloc();
    // int width = 2560;
    // int height = 1440;
    int width = 1280;
    int height = 760;

    AVPixelFormat format = m_codecCtx->pix_fmt;

    // 分配源图像和目标图像的内存
    int srcBufferSize = av_image_get_buffer_size(format, width, height, 1);
    uint8_t *srcBuffer = (uint8_t *)av_malloc(srcBufferSize);
    av_image_fill_arrays(frame->data, frame->linesize, srcBuffer, format, width, height, 1);

    int dstBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
    uint8_t *dstBuffer = (uint8_t *)av_malloc(dstBufferSize);
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, dstBuffer, AV_PIX_FMT_RGB24, 1280, 760, 1);

    // 设置转换上下文
    SwsContext *swsCtx = sws_getContext(width, height, format, width, height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

    static int count = 0;
    auto startTime = std::chrono::high_resolution_clock::now(); // 记录开始时间

    while (m_grabRunning)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        int frameInterval = 1000 / m_cameraInfo.mus_CameraFps; // 计算每帧间隔时间

        if (elapsed < frameInterval)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(frameInterval - elapsed)); // 控制帧率
        }

        startTime = std::chrono::high_resolution_clock::now(); // 更新开始时间

        if (av_read_frame(m_formatCtx, &packet) < 0)
        {
            if (count++ > 10)
            {
                break;
            }
            continue;
        }

        count = 0;

        if (packet.stream_index == m_videoStreamIndex)
        {
            // 解码视频帧
            avcodec_send_packet(m_codecCtx, &packet);
            if (avcodec_receive_frame(m_codecCtx, frame) == 0)
            {
                sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height, rgbFrame->data, rgbFrame->linesize);

                QImage img(rgbFrame->data[0], 1280, 760, rgbFrame->width * 3, QImage::Format_RGB888);

                // img.save("test.jpg");

                if (!m_grabRunning)
                {
                    break;
                }

                emit ImageOutPut(img, m_cameraInfo.mstr_CameraName);
            }
        }
        av_packet_unref(&packet);
    }

    // 清理资源
    sws_freeContext(swsCtx);
    av_free(srcBuffer);
    av_free(dstBuffer);
    av_frame_free(&frame);
    av_frame_free(&rgbFrame);
}

// 其他必要实现（根据基类要求）
bool CameraUsb::closeCamera()
{
    if (m_formatCtx)
        avformat_close_input(&m_formatCtx);
    if (m_codecCtx)
        avcodec_free_context(&m_codecCtx);
    return true;
}

// CameraUsb.cpp 新增实现
std::string CameraUsb::getCameraName()
{
    return m_cameraInfo.mstr_CameraName;
}

void CameraUsb::ImageHeightWidth()
{
    if (m_codecCtx)
    {
        m_imgInfo.m_img_width = m_codecCtx->width;
        m_imgInfo.m_img_height = m_codecCtx->height;
        emit imgInfoOut(m_imgInfo);
    }
}
// CameraUsb.cpp 添加实现
bool CameraUsb::stopGrab()
{

    m_grabRunning = false;
    if (m_grabThread.joinable())
    {
        m_grabThread.join();
    }
    return true;
}

bool CameraUsb::getImage()
{
    // USB摄像头通常不支持单帧抓取
    qDebug() << "USB camera does not support single frame grabbing";
    return false;
}

bool CameraUsb::ReadConfig(const YAML::Node &config)
{
    m_cameraInfo.mstr_Camera_Type = config["camera_type"].as<std::string>();
    m_cameraInfo.mstr_Camera_ID = config["camera_ip"].as<std::string>();
    m_cameraInfo.mus_CameraLeft = config["camera_left"].as<unsigned short>();
    m_cameraInfo.mus_CameraRight = config["camera_right"].as<unsigned short>();
    m_cameraInfo.mus_CameraTop = config["camera_top"].as<unsigned short>();
    m_cameraInfo.mus_CameraBottom = config["camera_bottom"].as<unsigned short>();
    m_cameraInfo.mus_CameraGain = config["camera_gain"].as<unsigned short>();
    m_cameraInfo.mus_CameraExposure = config["camera_exposure"].as<unsigned short>();
    m_cameraInfo.mus_CameraFps = config["camera_fps"].as<unsigned short>();
    m_cameraInfo.mi_CameraPixelFormat = static_cast<PixelFormat>(config["camera_pixelformat"].as<int>());
    m_cameraInfo.mus_CameraPort = config["camera_port"].as<unsigned short>();
    m_cameraInfo.mstr_CameraPassword = config["camera_pwd"].as<std::string>();
    m_cameraInfo.mstr_CameraUserName = config["camera_username"].as<std::string>();
    m_cameraInfo.mstr_CameraName = config["camera_name"].as<std::string>();
    m_cameraInfo.mus_IsShowImg = static_cast<ReturnCode>(config["isshow_img"].as<int>());
    m_cameraInfo.mi_IsShowData = static_cast<ReturnCode>(config["isshow_data"].as<int>());
    m_cameraInfo.mstr_SaveImageType = (config["save_img_type"].as<std::string>());
    m_cameraInfo.mi_SaveImageMod = static_cast<PicSaveType>(config["save_img_mod"].as<int>());
    m_cameraInfo.mui8_Channel = config["channel"].as<unsigned char>();
    m_cameraInfo.mstr_SaveVideoType = config["save_video_type"].as<std::string>();
    m_cameraInfo.mstr_Video_SavePath = config["save_video_path"].as<std::string>();

    return true;
}
