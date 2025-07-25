#pragma once
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/fifo.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/opt.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/hwcontext.h>
#include <libswscale/swscale.h>

// 	#include "libavcodec/qsv.h"
#include "libavformat/avformat.h"
//	#include "libavformat/url.h"
#include "libavformat/avio.h"
//	#include "libavfilter/avcodec.h"
// #include "libavfilter/avfiltergraph.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}

#include "encodevideo.h"
#include <mutex>
#include "tis_global/Struct.h"

namespace _Kits
{
    namespace SaveVideoNew
    {
        class FFMPEGEncodeVideo final : public EncodeVideo
        {
          private:
            AVCodec *m_encodecodec = nullptr;
            AVCodecContext *m_codecContext = nullptr;
            AVFrame *m_avframe = nullptr;
            int m_frameindex = 0;
            int m_srcindex = 0;
            SwsContext *m_swscontext = nullptr;
            std::function<void(int, AVPacket &, int)> m_writefilecallback;
            bool m_initialized = false;
            std::mutex m_write_mutex;

          public:
            explicit FFMPEGEncodeVideo(/* args */);
            virtual ~FFMPEGEncodeVideo();

            /** * @brief  初始化编码
             * @param   encoderinfo   视频参数信息
             * @param   srcindex   视频流编号
             * @param   writefilecallback   写入文件回调
             * @return  0: sucess ** **/
            virtual int Initencoder(InputInfo &encoderinfo,
                                    int srcindex,
                                    std::function<void(int, AVPacket &, int)> writefilecallback) override;

            /** * @brief  推入图片数据
             * @param   rgb   rgb图片数据
             * @param   size  图片大小
             * @param   srcpixfmt  图片格式
             * @return  0: sucess ** **/
            virtual int WriteRGBData(const uint8_t *rgb, int width, int srcpixfmt) override;

            /** * @brief  推入图片数据
             * @param   rgb   图片原始数据
             * @param   width  图片宽
             * @param   height  图片高
             * @param   srcpixfmt  图片格式
             * @return  0: sucess ** **/
            virtual int WriteRGBData(const uint8_t *buffer, int width, int height, int srcpixfmt) override;

            /** * @brief  推入图片数据
             * @param   imageData   图片原始数据结构体
             * @return  0: sucess ** **/
            virtual int WriteRGBData(std::shared_ptr<TIS_Info::ImageBuffer> imageData) override;

            /** * @brief  结束编码
             * @param
             * @return  ** **/
            virtual void EndEncode() override;

          private:
            /** * @brief  打开视频文件
             * @param   opt_arg  图片格式
             * @return ** **/
            void OpenVideo(AVDictionary *opt_arg);

            /** * @brief  初始化SwsContext
             * @param   srcpixfmt  图片格式
             * @return ** **/
            int InitSwsContext(enum AVPixelFormat srcpixfmat);

            /** * @brief 编码
             * @param   frame  编码数据
             * @return ** **/
            void encode(const AVFrame *frame);

            /** * @brief 释放资源
             * @return ** **/
            void Release();

            /** * @brief 配置YUV420P输入数据
             * @param   buffer  原始数据
             * @param   width  图片宽
             * @param   height  图片高
             * @param   src_data  图片数据
             * @param   src_linesize  图片行数据
             * @return ** **/
            void setupYUV420PInput(const uint8_t *buffer, int width, int height, const uint8_t **src_data, int *src_linesize);

            AVPixelFormat m_lastSrcFormat;
            size_t m_lastHeight;
            size_t m_lastWidth;
        };
    } // namespace SaveVideoNew
} // namespace _Kits