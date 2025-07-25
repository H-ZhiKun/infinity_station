#pragma once
#include <functional>
#include <string>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}
#include "tis_global/Struct.h"

namespace _Kits
{
    namespace SaveVideoNew
    {

        class EncodeVideo
        {
          private:
            /* data */
          public:
            // 视频head信息
            typedef struct
            {
                uint8_t data[256]; // head数据
                uint32_t size;     // 数据大小
            } SpsHeader;

            // 输入图片的格式
            enum eFormatType
            {
                BGR24 = 0x00,
                RGB24 = 0x01,
                ARGB32 = 0x02,
                ABGR32 = 0x03,
                BGRA32 = 0x04,
                RGBA32 = 0x05,
                // YUV420SP = 0x06,
                // YUV420P = 0x07,
            };

            // 编码结果流格式
            enum eStreamType
            {
                H264 = 0x00,
                H265 = 0x01,
                WMV = 0x03,
            };

            struct FrameInfo
            {
                uint32_t height; // frame高
                uint32_t width;  // frame宽
                short format;    // eFormatType
                uint32_t fps;    // 帧率
            };

            struct StreamInfo
            {
                short StreamType; // eStreamType
                uint32_t gop;     // GOP
            };

            typedef struct InputInfo
            {
                int width = 0;                // 视频宽
                int height = 0;               // 视频高
                int fps = 30;                 // 视频帧率
                short avcodeid = 27;          // 编码器id
                short pixfmt = 0;             // 编码的数据格式
                std::string codecname;        // 编码器名称
                uint8_t data[256];            // head数据
                uint32_t size;                // 数据大小
                int rkencode = 0;             // 是否使用rkmpp编码
                short format;                 // eFormatType
                long long bit_rate = 4000000; // 码率
            } InputInfo;

            EncodeVideo() = default;
            virtual ~EncodeVideo() = default;

            /** * @brief  初始化编码
             * @param   encoderinfo   视频参数信息
             * @param   srcindex   视频流编号
             * @param   writefilecallback   写入文件回调
             * @return  0: sucess ** **/
            virtual int Initencoder(InputInfo &encoderinfo, int srcindex, std::function<void(int, AVPacket &, int)> writefilecallback) = 0;

            /** * @brief  推入图片数据
             * @param   rgb   rgb图片数据
             * @param   size  图片大小
             * @param   srcpixfmt  图片格式
             * @return  0: sucess ** **/
            virtual int WriteRGBData(const uint8_t *rgb, int width, int srcpixfmt) = 0;

            /** * @brief  推入图片数据
             * @param   rgb   图片原始数据
             * @param   width  图片宽
             * @param   height  图片高
             * @param   srcpixfmt  图片格式
             * @return  0: sucess ** **/
            virtual int WriteRGBData(const uint8_t *buffer, int width, int height, int srcpixfmt) = 0;

            /** * @brief  推入图片数据
             * @param   imageData   图片原始数据结构体
             * @return  0: sucess ** **/
            virtual int WriteRGBData(std::shared_ptr<TIS_Info::ImageBuffer> imageData) = 0;

            /** * @brief  结束编码
             * @param
             * @return  ** **/
            virtual void EndEncode() = 0;
        };
    } // namespace SaveVideoNew
} // namespace _Kits