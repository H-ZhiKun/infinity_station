#include "HikConvertRGB.h"

namespace _Kits {

HikConvertRGB::HikConvertRGB(/* args */)
{
}

HikConvertRGB::~HikConvertRGB()
{
    m_threadHikblock_run.store(false);
    m_cvHikblock.notify_all();
    for (auto &th : m_threadHikblock_thread)
    {
        if (th.joinable())
            th.join();
    }
}

void HikConvertRGB::initConvert()
{
    m_threadHikblock_run.store(true);
    int iThreadNum = 0;
    for (; iThreadNum < m_threadNum; ++iThreadNum)
    {
        m_threadHikblock_thread.push_back(
            std::thread(&HikConvertRGB::processHikblock, this));
    }
}

void HikConvertRGB::pushImage(char *pBuf, FRAME_INFO *pFrameInfo, long nSize)
{
    std::lock_guard<std::mutex> lock(m_mutexHikblock);
    m_dequeHikblock.push_back(
        Hikblock(pBuf, pFrameInfo, nSize));
    m_cvHikblock.notify_one();
}

bool HikConvertRGB::YV12toRGB(const unsigned char *yv12,
    std::vector<unsigned char> &rgb,
    int width,
    int height)
{
    if (!yv12)
    {
        //qDebug() << "Invalid input pointers";
        return false;
    }

    if (yv12[0] == '\0')
    {
        return false;
    }

    if (width <= 0 || height <= 0)
    {
        //qDebug() << "Invalid width or height";
        return false;
    }

    const int frameSize = width * height;
    const int qFrameSize = frameSize / 4;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int y = yv12[j * width + i] & 0xFF;
            int v = yv12[frameSize + (j >> 1) * (width >> 1) + (i >> 1)] & 0xFF;
            int u = yv12[frameSize + qFrameSize + (j >> 1) * (width >> 1) +
            (i >> 1)] &
            0xFF;

            int r = y + (int)(1.370705 * (u - 128));
            int g =
            y - (int)(0.698001 * (v - 128)) - (int)(0.337633 * (u - 128));
            int b = y + (int)(1.732446 * (v - 128));

            r = qBound(0, r, 255);
            g = qBound(0, g, 255);
            b = qBound(0, b, 255);

            rgb[3 * (j * width + i) + 0] = (unsigned char)r;
            rgb[3 * (j * width + i) + 1] = (unsigned char)g;
            rgb[3 * (j * width + i) + 2] = (unsigned char)b;
        }
    }
    return true;


}

void HikConvertRGB::processHikblock()
{
    static int flag = 0;
    while (m_threadHikblock_run.load())
    {
        std::unique_lock<std::mutex> lock(m_mutexHikblock);
        m_cvHikblock.wait(lock, [this] {
            return !m_dequeHikblock.empty() || !m_threadHikblock_run.load();
        });
        if (!m_threadHikblock_run.load())
            return;
        Hikblock hikblock = m_dequeHikblock.front();
        m_dequeHikblock.pop_front();
        while(m_dequeHikblock.size() > 20)
        {
            m_dequeHikblock.pop_front();
        }

        lock.unlock();

        int width = hikblock.frameInfo.nWidth;
        int height = hikblock.frameInfo.nHeight;
        int bufferSize = width * height * 3; // 用于存储RGB数据的缓冲区大小

        std::vector<unsigned char> rgbBuffer(bufferSize, 0);
        if (YV12toRGB((unsigned char *)hikblock.YV12Buffer.data(),
                      rgbBuffer,
                      width,
                      height))
        {
            QImage img(rgbBuffer.data(), width, height, QImage::Format_BGR888);

            img = img.scaled(1280, 760);
            convertRGBEndSingal(img); 
        }
        else
        {
            qDebug() << "QImage 构造失败";
        }
    }
}

}