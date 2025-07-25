#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <atomic>
#include <QObject> 
#include <QDebug>
#include <QImage>
#include <QPainter>

namespace _Kits {

#if __has_include(<windows.h>)
#include "WindowsPlayM4.h"
#elif __has_include(<unistd.h>)
#include "LinuxPlayM4.h"
#endif


class HikConvertRGB:public QObject
{
    Q_OBJECT

private:
    struct Hikblock{
        std::vector<unsigned char> YV12Buffer;
        FRAME_INFO frameInfo;

        Hikblock() = default;

        Hikblock(char *pBuf, FRAME_INFO *pFrameInfo, long nSize) {
            
            frameInfo = *pFrameInfo;

            YV12Buffer.resize(nSize);
            YV12Buffer.insert(YV12Buffer.begin(), pBuf, pBuf + nSize);
        }
    };
public:
    HikConvertRGB(/* args */);
    ~HikConvertRGB();
    void initConvert();
    void pushImage(char *pBuf, FRAME_INFO *pFrameInfo, long nSize);

signals:
    void convertRGBEndSingal(const QImage &image);
private:
    void processHikblock();
    bool YV12toRGB(const unsigned char* yv12, std::vector<unsigned char> &rgb, int width, int height);
private:
    std::condition_variable m_cvHikblock;
    std::deque<Hikblock> m_dequeHikblock;
    std::mutex m_mutexHikblock;
    std::deque<std::thread> m_threadHikblock_thread;
    std::atomic_bool m_threadHikblock_run = false;
    int m_threadNum = 2;
};
}