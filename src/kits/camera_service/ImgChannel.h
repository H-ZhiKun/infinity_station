#pragma once

#include <QImage>
#include <map>


namespace _Kits{
class ImgChannel {
    public:
    ImgChannel() = default;
    ~ImgChannel() = default;
    
    static void RegisterImgChannel(int channel, QImage::Format format)
    {
        m_image_channel[channel] = format;
    }

    static QImage::Format GetImgChannel(int channel)
    {
        if (m_image_channel.find(channel) != m_image_channel.end())
        {
            return m_image_channel[channel];
        }
        else
        {
            return QImage::Format_Invalid;
        }
    }

    private:
    static inline std::map<int, QImage::Format> m_image_channel = {};
};
}