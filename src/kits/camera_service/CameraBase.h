#pragma once
#include "ImgChannel.h"
// #include "SaveVideo.h"
#include <QDir>
#include <QFont>
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QString>
#include <qimage.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace _Kits
{

    // 相机保存图片类型
    enum PicSaveType
    {
        PicSaveType_None,
        PicSaveType_Visible,          // 可见光
        PicSaveType_Infrared,         // 热成像
        PicSaveType_Infrared_Addition // 热成像附加信息显示
    };

    // 相机像素格式
    enum PixelFormat
    {
        PixelFormat_Gray = 1,
        PixelFormat_Color
    };

    // 非Bool的判断是非类型
    enum ReturnCode
    {
        ReturnCode_Success = 0, // 成功
        ReturnCode_Fail = 1,    // 失败
        ReturnCode_Error = 2,   // 错误
        ReturnCode_Invalid = 3, // 无效
        ReturnCode_NOOVER = 4,  // 无超限
        ReturnCode_OVER = 5,    // 超限
    };

    // 相机信息
    struct CameraInfo
    {
        std::string mstr_Camera_Type; // 相机类型
        std::string mstr_Camera_ID;   // 相机Id-Ip

        unsigned short mus_CameraLeft;   // 相机采集范围left
        unsigned short mus_CameraRight;  // 相机采集范围Right
        unsigned short mus_CameraTop;    // 相机采集范围Top
        unsigned short mus_CameraBottom; // 相机采集范围Bottom

        unsigned short mus_CameraGain;     // 相机采集增益
        unsigned short mus_CameraExposure; // 相机采集曝光
        unsigned short mus_CameraFps;      // 相机采集范围帧率
        PixelFormat mi_CameraPixelFormat;  // 相机像素格式：1 灰度，3 彩色
        unsigned short mus_CameraPort;     // 网络相机端口
        std::string mstr_CameraPassword;   // 网络相机密码
        std::string mstr_CameraUserName;   // 网络相机用户名
        std::string mstr_CameraName;       // 相机名，标识用！方便区别
        ReturnCode mus_IsShowImg;          // 默认是否显示图像
        ReturnCode mi_IsShowData;          // 是否显示字幕
        PicSaveType mi_SaveImageMod;       // 保存格式   1：可见光 2：红外成像
                                           // 3：红外成像附加信息显示
        std::string mstr_SaveImageType;    // 保存格式   jpg,png

        unsigned char mui8_Channel;      // 通道号
        std::string mstr_Img_SavePath;   // 图像保存路径
        std::string mstr_Video_SavePath; // 视频保存路径
        std::string mstr_SaveVideoType;  // 视频保存格式

        bool mb_iscacheforvideo = false;
        int mi_cacheforvideo_timesize_before = 5;
        int mi_cacheforvideo_timesize_after = 5;
    };

    typedef struct
    {
        unsigned long dwYear;   // 年
        unsigned long dwMonth;  // 月
        unsigned long dwDay;    // 日
        unsigned long dwHour;   // 时
        unsigned long dwMinute; // 分
        unsigned long dwSecond; // 秒
    } DownloadTime;

    struct DownloadTimeRange
    {
        DownloadTime startTime; // 开始时间
        DownloadTime endTime;   // 结束时间
    };

    struct ImgInfo
    {
        int m_img_width;
        int m_img_height;
    };

    class CameraBase : public QObject
    {
        Q_OBJECT

      public:
        CameraBase(QObject *parent = nullptr);
        virtual ~CameraBase();
        // 纯虚函数接口
        virtual bool openCamera(const YAML::Node &config) = 0;
        virtual bool closeCamera() = 0;
        virtual bool startGrab() = 0;
        virtual bool stopGrab() = 0;
        virtual bool getImage() = 0;
        virtual std::string getCameraName() = 0;

      signals:
        void ImageOutPut(const QImage image, const std::string &camera_name);
        void imgInfoOut(ImgInfo &imgInfo);

      protected:
        bool m_bIsGrabbing = false;
        CameraInfo m_cameraInfo; // 写一下通用的 配置， 每个相机再组合自己特殊的成员
        ImgInfo m_imgInfo;

      protected:
        virtual void ImageHeightWidth()
        {
            return;
        };
        virtual bool ReadConfig(const YAML::Node &config) = 0;
    };

} // namespace _Kits