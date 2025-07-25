#include "HikCamera.h"
#include "tis_global/EnumPublic.h"
#include <QDebug>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <string.h>
#include "kits/common/log/CRossLogger.h"
#include <QStringConverter>
#include <QString>
#include <QStringList>
#include "kits/common/thread_pool/ConcurrentPool.h"

#ifndef Q_MOC_RUN
#ifdef emit
#undef emit
#include <tbb/tbb.h>
#include <algorithm>
#define emit
#else
#include <tbb/tbb.h>
#include <algorithm>
#endif // emit
#endif // Q_MOC_RUN

using namespace _Kits;

typedef void(CALLBACK *StdCallFuncDebWin)(long, char *, long, FRAME_INFO *, long, long);
typedef void(CALLBACK *StdCallFuncDebLinux)(int, char *, int, FRAME_INFO *, void *, int);

typedef void(CALLBACK *StdCallFuncReal)(LONG, DWORD, BYTE *, DWORD, void *);

HikCamera::HikCamera() : HikBase()
{
    memset(&m_struPlayInfo, 0, sizeof(NET_DVR_PREVIEWINFO));
}

HikCamera::~HikCamera()
{
    mb_isStop = true;

    m_cond_yvToyuv.notify_all();
    closeCamera();

    if (m_thread_yvToyuv.joinable())
    {
        m_thread_yvToyuv.join();
    }
}

bool HikCamera::init(TIS_Info::HikLogInfo &cameraInfo)
{
    if (m_lUserID >= 0)
    {
        LogError("Camera already connected! ip:{} port:{}", m_cameraLogInfo.mstr_ipAddr.c_str(), m_cameraLogInfo.mus_port);
        return false;
    }

    // 初始化 SDK
    if (!NET_DVR_Init())
    {
        LogError("Failed to initialize SDK! ip:{} port:{}, error_code:{}",
                 m_cameraLogInfo.mstr_ipAddr.c_str(),
                 m_cameraLogInfo.mus_port,
                 NET_DVR_GetLastError());
        return false;
    }

    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40 struDeviceInfo = {0};

    m_cameraLogInfo = cameraInfo;

#if __has_include(<windows.h>)
    strcpy_s(struLoginInfo.sDeviceAddress, m_cameraLogInfo.mstr_ipAddr.c_str());
    strcpy_s(struLoginInfo.sUserName, m_cameraLogInfo.mstr_userName.c_str());
    strcpy_s(struLoginInfo.sPassword, m_cameraLogInfo.mstr_password.c_str());
#elif __has_include(<unistd.h>)
    strncpy(struLoginInfo.sDeviceAddress, m_cameraLogInfo.mstr_ipAddr.c_str(), sizeof(struLoginInfo.sDeviceAddress));
    strncpy(struLoginInfo.sUserName, m_cameraLogInfo.mstr_userName.c_str(), sizeof(struLoginInfo.sUserName));
    strncpy(struLoginInfo.sPassword, m_cameraLogInfo.mstr_password.c_str(), sizeof(struLoginInfo.sPassword));
#endif

    struLoginInfo.wPort = m_cameraLogInfo.mus_port;

    m_lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfo);
    if (m_lUserID < 0)
    {
        LogError("Login failed, error code:{}", NET_DVR_GetLastError());

        NET_DVR_Cleanup();

        return false;
    }

    mi_Channel = m_cameraLogInfo.mi_channel;
    getImageHeightWidth();

    return true;
}

bool HikCamera::start(TIS_Info::HikLogInfo &cameraInfo)
{
    m_ImageBufferPools = std::make_shared<ObjectPool<TIS_Info::ImageBuffer>>();
    if (m_ImageBufferPools == nullptr)
    {
        LogError("[HikCamera::start]Failed to create image buffer pool");
        return false;
    }

    if (!init(cameraInfo))
    {
        return false;
    }

    if (!startGrab())
    {
        return false;
    }

    m_thread_yvToyuv = std::thread(&HikCamera::yvToyuv_thread, this);

    ConcurrentPool::runEvery("HikCamera::writeSubTitleToCamera", 500, [this]() { writeSubTitleToCamera(); });
    connect(&m_timer_subTitle, &QTimer::timeout, [this]() { mb_subTitle_show = false; });
    m_timer_subTitle.setInterval(5000); // 五秒监测一次字幕是否需要写入更新

#if __has_include(<windows.h>)

    unsigned long amodeParamInfoSize;

#else if __has_include(<unistd.h>)

    unsigned int amodeParamInfoSize;

#endif

    NET_DVR_GetDVRConfig(
        m_lUserID, NET_DVR_GET_SHOWSTRING_V30, mi_Channel, &m_subTitle_change, sizeof(NET_DVR_SHOWSTRING_V30), &amodeParamInfoSize);

    return true;
}

bool HikCamera::stopGrab()
{
    if (m_lRealPlayHandle >= 0)
    {
        if (!NET_DVR_StopRealPlay(m_lRealPlayHandle))
        {
            qDebug() << "Stop realplay failed, error code:" << NET_DVR_GetLastError();
            return false;
        }
        m_lRealPlayHandle = -1;
    }

    PlayM4_FreePort(m_lPort);

    return true;
}

bool HikCamera::startGrab()
{
    if (m_lUserID < 0)
    {
        qDebug() << "Camera not connected!";
        return false;
    }

    m_struPlayInfo.hPlayWnd = NULL;
    m_struPlayInfo.lChannel = m_cameraLogInfo.mi_channel;
    m_struPlayInfo.dwStreamType = 0;
    m_struPlayInfo.dwLinkMode = 0;

    m_lRealPlayHandle = NET_DVR_RealPlay_V40(
        m_lUserID,
        &m_struPlayInfo,
        static_cast<StdCallFuncReal>([](LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *pUser) {
            HikCamera *pThis = static_cast<HikCamera *>(pUser);

            static bool bInit = false;

            if (bInit)
            {
                g_mapPortUser[pThis->m_lPort] = pUser;
            }

            switch (dwDataType)
            {
            case NET_DVR_SYSHEAD: // 系统头数据
                if (!bInit && dwBufSize > 0)
                {
                    // 初始化 PlayM4 解码库
                    if (PlayM4_GetPort(&pThis->m_lPort) == 1 && PlayM4_SetStreamOpenMode(pThis->m_lPort, STREAME_REALTIME) == 1)
                    {

                        if (PlayM4_OpenStream(pThis->m_lPort, pBuffer, dwBufSize, 20 * 1024 * 1024) == 1)
                        {
// 设置解码回调，输出 RGB 数据
#if __has_include(<windows.h>)
                            PlayM4_SetDecCallBack(
                                pThis->m_lPort,
                                static_cast<StdCallFuncDebWin>(
                                    [](long nPort, char *pBuf, long nSize, FRAME_INFO *pFrameInfo, long nReserved1, long nReserved2) {
                                        if (nSize > 0 && pBuf != nullptr)
                                        {
                                            HikCamera *pThis = static_cast<HikCamera *>(g_mapPortUser[nPort]);
                                            if (pThis == nullptr)
                                            {
                                                qDebug() << "pThis is null.";
                                                return;
                                            }

                                            if (pFrameInfo->nType == T_YV12)
                                            {
                                                auto img = pThis->m_ImageBufferPools->getObject(
                                                    pFrameInfo->nWidth, pFrameInfo->nHeight, 3); // YUV420P

                                                if (img == nullptr)
                                                {
                                                    LogError("[HikCamera::DecCBFun]Failed to get image buffer");
                                                    return;
                                                }

                                                int size = img->data.size();
                                                if (size >= nSize)
                                                {
                                                    std::memcpy(img->data.data(), pBuf, nSize);
                                                    img->timestamp = pFrameInfo->nStamp;
                                                    img->pixFormat = (int)TIS_Info::EnumPublic::AVPixelFormat::AV_PIX_FMT_YUV420P;
                                                    img->name = pThis->m_cameraLogInfo.mstr_deviceName;
                                                    img->addr = pThis->m_cameraLogInfo.mstr_ipAddr;

                                                    pThis->mque_yv12.push_back(img);    // 将 YV12 图像数据存入队列
                                                    pThis->m_cond_yvToyuv.notify_one(); // 通知转换线程
                                                }
                                                else if (size == 0)
                                                {
                                                    return; // 如果 size 为 0，直接返回  因为此时可能已经析构
                                                }
                                                else
                                                {
                                                    LogError("[HikCamera::DecCBFun]Image buffer size not match");
                                                }
                                            }
                                        }
                                    }));
#elif __has_include(<unistd.h>)
                            PlayM4_SetDecCallBack(
                                pThis->m_lPort,
                                static_cast<StdCallFuncDebLinux>(
                                    [](int nPort, char *pBuf, int nSize, FRAME_INFO *pFrameInfo, void *nReserved1, int nReserved2) {
                                        if (nSize > 0 && pBuf != nullptr)
                                        {
                                            HikCamera *pThis = static_cast<HikCamera *>(g_mapPortUser[nPort]);
                                            if (pThis == nullptr)
                                            {
                                                qDebug() << "pThis is null.";
                                                return;
                                            }

                                            if (pFrameInfo->nType == T_YV12)
                                            {
                                                // pThis->pushImage(pBuf, pFrameInfo, nSize);
                                                auto img = pThis->m_ImageBufferPools->getObject(
                                                    pFrameInfo->nWidth, pFrameInfo->nHeight, 3); // YUV420P

                                                if (img == nullptr)
                                                {
                                                    LogError("[HikCamera::DecCBFun]Failed to get image buffer");
                                                    return;
                                                }

                                                int size = img->data.size();
                                                if (size >= nSize)
                                                {
                                                    std::memcpy(img->data.data(), pBuf, nSize);
                                                    img->timestamp = pFrameInfo->nStamp;
                                                    img->pixFormat = (int)TIS_Info::EnumPublic::AVPixelFormat::AV_PIX_FMT_YUV420P;
                                                    img->name = pThis->m_cameraLogInfo.mstr_deviceName;
                                                    img->addr = pThis->m_cameraLogInfo.mstr_ipAddr;

                                                    pThis->mque_yv12.push_back(img);    // 将 YV12 图像数据存入队列
                                                    pThis->m_cond_yvToyuv.notify_one(); // 通知转换线程
                                                }
                                                else if (size == 0)
                                                {
                                                    return; // 如果 size 为 0，直接返回  因为此时可能已经析构
                                                }
                                                else
                                                {
                                                    LogError("[HikCamera::DecCBFun]Image buffer size not match");
                                                }
                                            }
                                        }
                                    }));
#endif
                            PlayM4_Play(pThis->m_lPort, NULL);
                            bInit = true;
                            qDebug() << "解码库初始化成功";
                        }
                    }
                    break;

                case NET_DVR_STREAMDATA: // 流数据
                    if (bInit && dwBufSize > 0 && pBuffer != nullptr)
                    {
                        if (!PlayM4_InputData(pThis->m_lPort, pBuffer, dwBufSize))
                        {
                            LogError("[HikCamera::RealDataCallBack]PlayM4_InputData failed, error code:{}",
                                     PlayM4_GetLastError(pThis->m_lPort));
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }),
        this);
    if (m_lRealPlayHandle < 0)
    {
        qDebug() << "Start realplay failed, error code:" << NET_DVR_GetLastError();
        return false;
    }

    return true;
}

void _Kits::HikCamera::getImageHeightWidth()
{
    BOOL bFlag = PlayM4_GetPort(&m_lPort);
    if (!bFlag)
    {
        qDebug() << "Failed to get port, error code:" << PlayM4_GetLastError(m_lPort);
        return;
    }

    BOOL ret = PlayM4_GetPictureSize(m_lPort, &mi_imageWidth, &mi_imageHeight);
    if (!ret)
    {
        qDebug() << "Failed to get picture size, error code:" << PlayM4_GetLastError(m_lPort);
    }
}

std::shared_ptr<TIS_Info::ImageBuffer> _Kits::HikCamera::yv12Toyuv420p(std::shared_ptr<TIS_Info::ImageBuffer> img_yv12)
{
    if (!img_yv12)
    {
        LogError("[HikCamera::yv12Toyuv420p]Received null image buffer");
        return nullptr;
    }

    const int width = img_yv12->width;
    const int height = img_yv12->height;
    const int y_size = width * height;
    const int uv_size = y_size / 4; // 每个色度平面大小

    // 验证源数据大小
    const size_t required_size = y_size + 2 * uv_size;
    if (img_yv12->data.size() < required_size)
    {
        LogError("[HikCamera::yv12Toyuv420p]Invalid source buffer size: %zu < %d", img_yv12->data.size(), required_size);
        return nullptr;
    }

    // 获取目标缓冲区
    auto img_yuv420p = m_ImageBufferPools->getObject(width, height, 3);
    if (!img_yuv420p)
    {
        LogError("[HikCamera::yv12Toyuv420p]Failed to get target buffer");
        return nullptr;
    }

    // 确保目标缓冲区足够大 (一次性设置大小)
    img_yuv420p->data.resize(required_size);

    // 获取原始指针 (避免vector迭代器开销)
    uint8_t *dst = img_yuv420p->data.data();
    const uint8_t *src = img_yv12->data.data();

    // 1. 复制Y平面 (0 → y_size)
    std::memcpy(dst, src, y_size);

    // 2. 复制U平面 (源：V平面后 | 目标：U平面位置)
    std::memcpy(dst + y_size,           // 目标U起始位置
                src + y_size + uv_size, // 源U起始位置 (YV12中U在V后)
                uv_size);

    // 3. 复制V平面 (源：V平面位置 | 目标：V平面位置)
    std::memcpy(dst + y_size + uv_size, // 目标V起始位置
                src + y_size,           // 源V起始位置
                uv_size);

    // 复制元数据
    img_yuv420p->timestamp = img_yv12->timestamp;
    img_yuv420p->pixFormat = static_cast<int>(TIS_Info::EnumPublic::AVPixelFormat::AV_PIX_FMT_YUV420P);
    img_yuv420p->name = img_yv12->name;
    img_yuv420p->addr = img_yv12->addr;

    return img_yuv420p;
}

void _Kits::HikCamera::yvToyuv_thread()
{
    while (!mb_isStop)
    {
        std::unique_lock<std::mutex> lock(m_mutex_yvToyuv);
        m_cond_yvToyuv.wait(lock, [this] { return !mque_yv12.empty() || mb_isStop; });

        if (mb_isStop)
        {
            break; // 如果停止标志为真，退出循环
        }

        if (!mque_yv12.empty())
        {
            auto img = mque_yv12.front();
            mque_yv12.pop_front();

            lock.unlock(); // 解锁互斥量，允许其他线程访问

            if (img == nullptr)
            {
                LogError("[HikCamera::yvToyuv]Received null image buffer");
                continue;
            }

            auto img_rgb = YV12toRGB(img);

            if (img_rgb == nullptr)
            {
                LogError("[HikCamera::yvToyuv]Failed to convert YV12 to YUV420P");
                continue;
            }

            emit sendImage(img_rgb);
        }
    }
}

bool HikCamera::closeCamera()
{
    if (m_lUserID >= 0)
    {
        stopGrab();
        if (!NET_DVR_Logout(m_lUserID))
        {
            qDebug() << "Logout failed, error code:" << NET_DVR_GetLastError();
            return false;
        }
        m_lUserID = -1;
    }

    PlayM4_CloseStream(m_lPort);
    PlayM4_FreePort(m_lPort);
    NET_DVR_Cleanup();

    return true;
}

std::vector<QString> HikCamera::foldSubtitles(const std::vector<QString> &subTitle, int lineNum, int maxByteLength, QStringEncoder &encoder)
{
    if (subTitle.empty() || lineNum <= 0 || maxByteLength <= 0)
    {
        return subTitle;
    }

    std::vector<QString> result;
    QString currentLine;
    int currentByteSize = 0;

    // 创建解码器（与编码器相同类型）
    QStringDecoder decoder(encoder.name());
    if (!decoder.isValid())
    {
        // 如果解码器无效，回退到 UTF-8
        decoder = QStringDecoder(QStringConverter::Utf8);
    }

    for (const QString &sub : subTitle)
    {
        // 计算当前片段编码后的字节长度
        const QByteArray subBytes = encoder.encode(sub);
        const int subByteLength = subBytes.size();

        // 计算添加分隔符后的长度
        int combinedByteLength = 0;
        if (!currentLine.isEmpty())
        {
            // 添加分隔符（空格）
            const QByteArray spaceBytes = encoder.encode(" ");
            combinedByteLength = currentByteSize + spaceBytes.size() + subByteLength;
        }
        else
        {
            combinedByteLength = subByteLength;
        }

        if (combinedByteLength <= maxByteLength)
        {
            // 可以安全添加
            if (!currentLine.isEmpty())
            {
                currentLine += " ";
            }
            currentLine += sub;
            currentByteSize = combinedByteLength;
        }
        else
        {
            // 保存当前行（如果结果未满）
            if (!currentLine.isEmpty() && result.size() < lineNum)
            {
                result.push_back(currentLine);
            }

            // 开始新行（如果结果行数未满）
            if (result.size() < lineNum)
            {
                // 处理超长单行
                if (subByteLength > maxByteLength)
                {
                    // 拆分超长行
                    QString remaining = sub;
                    while (!remaining.isEmpty() && result.size() < lineNum)
                    {
                        QString clipped;
                        int pos = findSafeSplitPos(encoder, decoder, remaining, maxByteLength);
                        if (pos <= 0)
                            break; // 安全保护

                        clipped = remaining.left(pos);
                        remaining = remaining.mid(pos);

                        result.push_back(clipped);
                    }
                    currentLine = remaining;
                    currentByteSize = encoder.encode(remaining).data.size();
                }
                else
                {
                    currentLine = sub;
                    currentByteSize = subByteLength;
                }
            }
            else
            {
                // 已达最大行数，丢弃剩余字幕
                currentLine.clear();
                currentByteSize = 0;
                break;
            }
        }
    }

    // 处理最后一行
    if (!currentLine.isEmpty() && result.size() < lineNum)
    {
        result.push_back(currentLine);
    }

    return result;
}

// 辅助函数：安全分割位置（避免拆分多字节字符）
int HikCamera::findSafeSplitPos(QStringEncoder &encoder, QStringDecoder &decoder, const QString &str, int maxBytes)
{
    // 先尝试整个字符串编码
    QByteArray bytes = encoder.encode(str);
    if (bytes.size() <= maxBytes)
    {
        return str.length();
    }

    // 截取最大字节长度的部分
    bytes = bytes.left(maxBytes);

    // 使用解码器转换为字符串
    QString decoded = decoder.decode(bytes);

    return decoded.length();
}

void _Kits::HikCamera::writeSubTitleToCamera()
{
    if (mb_subTitle_show == false || m_subTitle_show.struStringInfo[0].wShowString == 0)
    {
        // 如果没有字幕显示，直接返回
        return;
    }

    NET_DVR_SetDVRConfig(m_lUserID, NET_DVR_SET_SHOWSTRING_V30, mi_Channel, &m_subTitle_show, sizeof(m_subTitle_show));
}

void HikCamera::onWriteSubTitle(std::vector<QString> subTitle)
{
    if (m_timer_subTitle.isActive())
    {
        m_timer_subTitle.stop();
        m_timer_subTitle.start(5000);
    }

    if (subTitle.empty() || m_lRealPlayHandle < 0)
    {
        return;
    }

    if (mvec_subTitle_nofold == subTitle)
    {
        return;
    }

    mvec_subTitle_nofold = subTitle;

    QStringEncoder encoder(QStringConverter::System);
    if (!encoder.isValid())
    {
        encoder = QStringEncoder(QStringConverter::Utf8);
    }

    // 获取最大单行长度（字节）
    const int maxLineBytes = sizeof(m_subTitle_change.struStringInfo[0].sString) - 1;

    // 传入编码器和字节长度限制
    auto nowsubTile = foldSubtitles(subTitle, MAX_STRINGNUM, maxLineBytes, encoder);

    // 处理每行字幕
    for (int i = 0; i < MAX_STRINGNUM; i++)
    {
        if (i < nowsubTile.size())
        {
            QByteArray encodedData = encoder.encode(nowsubTile[i]);

            // 确保不会越界（最大maxLineBytes字节）
            if (encodedData.size() > maxLineBytes)
            {
                // 安全截断多字节字符
                int truncatePos = maxLineBytes;
                while (truncatePos > 0 && (static_cast<quint8>(encodedData[truncatePos]) & 0x80) &&
                       !(static_cast<quint8>(encodedData[truncatePos]) & 0x40))
                {
                    truncatePos--;
                }

                // 执行截断
                encodedData = encodedData.left(truncatePos);

                // 添加省略号表示截断
                if (truncatePos > 3)
                {
                    QByteArray ellipsis = encoder.encode("...");
                    int ellipsisSize = ellipsis.size();

                    if (encodedData.size() + ellipsisSize <= maxLineBytes)
                    {
                        // 有足够空间添加完整省略号
                        encodedData.append(ellipsis);
                    }
                    else if (encodedData.size() >= ellipsisSize)
                    {
                        // 替换最后几个字符为省略号
                        memcpy(encodedData.data() + encodedData.size() - ellipsisSize, ellipsis.data(), ellipsisSize);
                    }
                }
            }

            int byteLength = encodedData.size();
            m_subTitle_change.struStringInfo[i].wStringSize = byteLength;
            m_subTitle_change.struStringInfo[i].wShowString = 1;
            m_subTitle_change.struStringInfo[i].wShowStringTopLeftX = mi_xStartPos;
            m_subTitle_change.struStringInfo[i].wShowStringTopLeftY = mi_yStartPos + mi_offset_dis * i;

            memset(m_subTitle_change.struStringInfo[i].sString, 0, sizeof(m_subTitle_change.struStringInfo[i].sString));
            memcpy(m_subTitle_change.struStringInfo[i].sString, encodedData.constData(), byteLength);
        }
        else
        {
            m_subTitle_change.struStringInfo[i].wShowString = 0;
            m_subTitle_change.struStringInfo[i].wStringSize = 0;
            memset(m_subTitle_change.struStringInfo[i].sString, 0, sizeof(m_subTitle_change.struStringInfo[i].sString));
        }
    }

    std::lock_guard<std::mutex> lock(m_mutex_subTitle);
    m_subTitle_show = m_subTitle_change;
    mb_subTitle_show = true;
}

std::shared_ptr<TIS_Info::ImageBuffer> HikCamera::YV12toRGB(std::shared_ptr<TIS_Info::ImageBuffer> img_yv12)
{
    if (!img_yv12 || img_yv12->data.empty())
    {
        LogError("[HikCamera::YV12toRGB] null or empty buffer");
        return nullptr;
    }

    auto &w = img_yv12->width;
    auto &h = img_yv12->height;

    std::shared_ptr<TIS_Info::ImageBuffer> img_rgb = m_ImageBufferPools->getObject(w, h, 3); // RGB24

    if (_mp_yuvConverter == nullptr)
    {
        _mp_yuvConverter = std::make_shared<YuvConverter>();
        if (_mp_yuvConverter == nullptr)
        {
            LogError("[HikCamera::YV12toRGB]Failed to create YuvConverter");
            return nullptr;
        }
        _mp_yuvConverter->init(w, h);
    }

    _mp_yuvConverter->yv12ToRgb_cuda(img_yv12->data.data(), img_rgb->data.data());

    // 5. 补元数据
    img_rgb->width = w;
    img_rgb->height = h;
    img_rgb->timestamp = img_yv12->timestamp;
    img_rgb->pixFormat = static_cast<int>(TIS_Info::EnumPublic::AVPixelFormat::AV_PIX_FMT_RGB24);
    img_rgb->name = img_yv12->name;
    img_rgb->addr = img_yv12->addr;

    return img_rgb;
}