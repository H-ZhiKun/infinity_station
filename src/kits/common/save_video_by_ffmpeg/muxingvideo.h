#pragma once
#include "encodevideo.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};

#include <memory>
#include <mutex>
#include <vector>

namespace _Kits
{
    namespace SaveVideoNew
    {
        class MuxingVideo
        {
          public:
            typedef struct OutputStream
            {
                AVCodec *encodecodec = nullptr;
                AVCodecContext *codecContext = nullptr;
                AVStream *stream = nullptr;
                uint8_t extradata[256];
                int rkflag = 0;
            } OutputStream;

          private:
            AVFormatContext *m_outformt_ctx = nullptr;
            bool m_initialized = false;
            std::vector<std::shared_ptr<OutputStream>> m_outputstream;
            std::mutex m_mtx;
            int m_fps = 30; // fps

          public:
            MuxingVideo() = default;
            ~MuxingVideo();

            /** * @brief 结束存储
             * @param
             * @return  ** **/
            void finished();

            /** * @brief 初始化编码信息
             * @param   filename   编码后文件名**/
            int Init(const char *filename, std::vector<EncodeVideo::InputInfo> inputinfo);

            void writeVideo(int no, AVPacket &pkt, int rkflag);

          private:
            /** * @brief 写入视频头部信息
             * @return ** **/
            int WriteHeader();

            /** * @brief 释放资源
             * @return ** **/
            void Release();

            /** * @brief rk添加视频流信息
             * @param   optstream   视频流信息
             * @param   InputInfo   视频输入参数
             * @return   ** **/
            void RKAddStream(std::shared_ptr<OutputStream> optstream, EncodeVideo::InputInfo &InputInfo);

            /** * @brief ffmpeg添加视频流信息
             * @param   optstream   视频流信息
             * @param   InputInfo   视频输入参数
             * @return   ** **/
            void AddStream(std::shared_ptr<OutputStream> optstream, EncodeVideo::InputInfo &InputInfo);
        };
    } // namespace SaveVideoNew
} // namespace _Kits