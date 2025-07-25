#include "CameraCtrl.h"
#include "CameraFactory.h"
#include "SaveVideo.h"
#include "SaveVideoCache.h"
#include "kits/camera_service/hik/SaveVideoHikNVR.h"
#include <QFile>
#include <QTextStream>

using namespace _Kits;
CameraCtrl::CameraCtrl(/* args */)
{
}

CameraCtrl::~CameraCtrl()
{
    onStopRecord();
    m_consumer_running = false;
    m_queueCondition.notify_all();

    for (auto &thread : m_vec_consumerThread)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

// void CameraCtrl::initAllCameraFactory()
// {
//     ICameraFactory::RegisterCmaeraCrteatFactory<CameraHik>("HIK");
//     ICameraFactory::RegisterCmaeraCrteatFactory<CameraHikNvr>("HIKNVR");
//     ICameraFactory::RegisterCmaeraCrteatFactory<CameraBaumer>("BAUMER");
//     ICameraFactory::RegisterCmaeraCrteatFactory<CameraUsb>("FFMPEGUSB");
// }

int CameraCtrl::createObjectByType(const std::string &type)
{
    m_camera_base = ICameraFactory::CreateCameraByType(type);
    if ("HIKNVR" == type)
    {
        m_save_video_base = std::make_shared<SaveVideoHikNVR>();
    }
    else
    {
        m_save_video_base = std::make_shared<SaveVideo>();
    }

    m_cachesave_video_base = std::make_shared<SaveVideoCache>();

    return 0;
}

int CameraCtrl::start(const YAML::Node &config)
{
    m_consumer_running = true;
    m_isclose = false;
    qDebug() << "Starting consumer threads...";

    for (int i = 0; i < 1; i++)
    {
        std::thread consumerThread = std::thread(&CameraCtrl::imageConsumerThread, this);
        m_vec_consumerThread.push_back(std::move(consumerThread));
    }
    m_str_CameraName = config["camera_name"].as<std::string>();
    m_str_SaveVideoType = config["save_video_type"].as<std::string>();
    m_str_Video_SavePath = config["save_video_path"].as<std::string>();

    if (nullptr != m_camera_base)
    {
        if (m_camera_base->openCamera(config))
        {
            qDebug() << "open camera success";
        }
        else
        {
            return 1;
        }
        // connect(m_camera_base.get(),&CameraBase::imgInfoOut,this,&CameraCtrl::onGetImageInfo);
        connect(m_camera_base.get(), &CameraBase::ImageOutPut, this, &CameraCtrl::onGetImage);

        if (m_camera_base->startGrab())
        {
            qDebug() << "startGrab success";
        }
        else
        {
            qDebug() << "startGrab fail";
            return 2;
        }

        QVariant var;
        if (0 != m_save_video_base->initSaveVideo(var))
        {
            return 3;
        }
        if (0 != m_cachesave_video_base->initSaveVideo(var))
        {
            return 3;
        }
    }
    else
    {
        return 4;
    }

    return 0;
}
int CameraCtrl::onGetImage(const QImage image, const std::string &camera_name)
{

    if (image.isNull() || m_isclose)
    {
        return 1;
    }

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        // m_imageQueue.push_back({image.copy(), camera_name});
        m_imageQueue.push_back({image, camera_name});
        // qDebug() << "Added image to queue, size:" << m_imageQueue.size()
        // << "camera:" << QString::fromStdString(camera_name);
    }
    m_queueCondition.notify_one();

    return 0;
}

void CameraCtrl::onSubtitleChanged(std::vector<std::string> &subtitle)
{
    if (0 == subtitle.size())
    {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutexSubAdd);
    m_vec_subtitle = subtitle;
}

int CameraCtrl::setSubtitle(int x_StartPos, int y_StartPos, int offset_dis)
{
    if (nullptr != m_save_video_base)
    {
        return m_save_video_base->setSubtitle(x_StartPos, y_StartPos, offset_dis);
    }

    return 0;
}
void CameraCtrl::onStartRecord(const std::string &filename)
{
    if (nullptr != m_save_video_base)
    {
        // 构建文件路径
        QString filefullname =
            QString::fromStdString(m_str_Video_SavePath) + "/" + QString::fromStdString(m_str_CameraName) + "/" + QString::fromStdString(filename) + "." + QString::fromStdString(m_str_SaveVideoType);

        QString filepath = QString::fromStdString(m_str_Video_SavePath) + "/" + QString::fromStdString(m_str_CameraName) + "/";

        // 使用 QDir 创建目录
        QDir dir;
        if (!dir.exists(filepath))
        {
            if (!dir.mkpath(filepath))
            {
                qDebug() << "Failed to create directory:" << filepath;
                return;
            }
        }

        // 创建文件
        QFile file(filefullname);
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Failed to create file:" << filefullname;
            return;
        }
        file.close(); // 关闭文件，确保文件创建成功

        // 开始录制
        auto path = filefullname.toStdString();
        m_save_video_base->startRecord(1, path);
        emit flagRecord(true); // 发送录制开始信号
    }

    m_saveVideoFlag.store(true);
}

void CameraCtrl::onStopRecord()
{
    if (nullptr != m_save_video_base)
    {
        m_save_video_base->stopRecord();
        emit flagRecord(false); // 发送录制停止信号
    }

    m_saveVideoFlag.store(false);
}
void CameraCtrl::setOSD(QImage &image)
{
    // 绘制 OSD 到图像
    QPainter painter(&image);
    QFont font("Arial", m_ioffsetdis); // 设置字体和大小
    painter.setFont(font);
    painter.setPen(Qt::red);
    std::lock_guard<std::mutex> lock(m_mutexSubAdd);
    std::vector<std::string> vec_subtitle = m_vec_subtitle;
    // 遍历字幕并绘制到 QImage 上
    for (size_t i = 0; i < vec_subtitle.size() && i < MAX_STRINGNUM_V30; i++)
    {
        QString text = QString::fromStdString(vec_subtitle[i]);
        int yPos = m_iyStartPos + i * m_ioffsetdis;
        painter.drawText(m_ixStartPos, yPos, text);
    }

    // image.save("test.jpg");

    // 将字幕文本写入文件
    // QString filePath = "subtitles.txt";
    // QFile file(filePath);
    // if (file.open(QIODevice::Append | QIODevice::Text))
    // {
    //     QTextStream out(&file);
    //     for (const auto& subtitle : vec_subtitle)
    //     {
    //         out << QString::fromStdString(subtitle) << "\n";
    //     }
    //     file.close();
    // }
    // else
    // {
    //     qDebug() << "Failed to open file for writing subtitles:" << filePath;
    // }
}

int CameraCtrl::addVideoSave()
{
    return 0;
}
void CameraCtrl::imageConsumerThread()
{
    // qDebug() << "Consumer thread started:";
    std::unique_lock<std::mutex> lock(m_queueMutex);
    while (m_consumer_running)
    {
        ImageData imgData;
        {
            m_queueCondition.wait(lock, [this] { return !m_imageQueue.empty() || !m_consumer_running; });

            if (!m_consumer_running)
            {
                // qDebug() << "Consumer thread stopping...";
                break;
            }

            if (!m_imageQueue.empty())
            {
                imgData = std::move(m_imageQueue.front());
                m_imageQueue.pop_front();
                // qDebug() << "Consumed image from queue, remaining:" <<
                // m_imageQueue.size();
            }
        }

        if (!imgData.image.isNull())
        {
            QVariantMap imgMap;
            QImage img = imgData.image;
            setOSD(img);
            imgMap["img"] = img;
            imgMap["camera_name"] = QString::fromStdString(imgData.camera_name);
            emit popImageSignals(QVariant(imgMap));
            //if (m_saveVideoFlag.load())//需要创建任务
            {
                if (nullptr != m_save_video_base)
                {
                    // QImage img = imgData.image;
                    // setOSD(img);
                    m_save_video_base->pushImage(img);
                }

                if (nullptr != m_cachesave_video_base)
                {
                    // QImage img = imgData.image;
                    // setOSD(img);
                    m_cachesave_video_base->pushImage(img);
                }
            }
        }
    }
    // qDebug() << "Consumer thread exiting:";
}

void CameraCtrl::onCacheStartRecord(const QVariant &data)
{
    if (nullptr != m_cachesave_video_base)
    {

        arcData arc_data;

        auto filename = arcFileNameSet(data, arc_data);

        // 构建文件路径
        QString filefullname =
            QString::fromStdString(m_str_Video_SavePath) + "/" + QString::fromStdString(m_str_CameraName) + "/" + QString::fromStdString(filename) + "." + QString::fromStdString(m_str_SaveVideoType);

        QString filepath = QString::fromStdString(m_str_Video_SavePath) + "/" + QString::fromStdString(m_str_CameraName) + "/";

        // 使用 QDir 创建目录
        QDir dir;
        if (!dir.exists(filepath))
        {
            if (!dir.mkpath(filepath))
            {
                qDebug() << "Failed to create directory:" << filepath;
                return;
            }
        }

        // 创建文件
        QFile file(filefullname);
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Failed to create file:" << filefullname;
            return;
        }
        file.close(); // 关闭文件，确保文件创建成功

        // 开始录制
        auto path = filefullname.toStdString();
        if (m_cachesave_video_base->startRecord(1, path) == 3)
        {
            qDebug() << "Recording is already in progress";

            QFile::remove(filefullname); // 删除文件

            return;
        }

        // 组装发送信息ID，path
        QVariantMap mapData;

        mapData["arcId"] = arc_data.arcId;
        mapData["videoPath"] = filefullname;

        emit sendPathToArcuv(QVariant::fromValue(mapData));
    }
}

void CameraCtrl::onVideoSavePathChanged(std::string path)
{
    m_str_Video_SavePath = path;
}

std::string CameraCtrl::arcFileNameSet(const QVariant &data, CameraCtrl::arcData &arc_data)
{
    QVariantMap map = data.toMap();

    auto it = map.find("arctime");
    if (it != map.end())
    {
        arc_data.duration = it.value().toString().toStdString();
    }

    it = map.find("arccount");
    if (it != map.end())
    {
        arc_data.arccount = it.value().toString().toStdString();
    }

    it = map.find("occurtime");
    if (it != map.end())
    {
        arc_data.occurtime = it.value().toString().toStdString();
        std::replace(arc_data.occurtime.begin(), arc_data.occurtime.end(), ':', '_');
    }

    it = map.find("arctimestamp");
    if (it != map.end())
    {
        arc_data.arctime = it.value().toString().toStdString();
        std::replace(arc_data.arctime.begin(), arc_data.arctime.end(), ':', '_');
    }

    it = map.find("arcpulse");
    if (it != map.end())
    {
        arc_data.arcpulse = it.value().toString().toStdString();
    }
    it = map.find("arcid");
    if (it != map.end())
    {
        arc_data.arcId = it.value().toUInt();
    }

    return QString::number(arc_data.arcId).toStdString() + "_occurtime" + arc_data.occurtime + "_count" + arc_data.arccount + "_arctime" + arc_data.arctime + "_pulse" + arc_data.arcpulse;
}