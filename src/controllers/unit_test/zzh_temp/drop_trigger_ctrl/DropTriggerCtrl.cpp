#include "DropTriggerCtrl.h"
#include "service/AppFramework.h"

using namespace _Controllers;

_Controllers::DropTriggerCtrl::DropTriggerCtrl()
    : _m_thread_saveVideo(std::thread(&DropTriggerCtrl::saveVideoThread, this)), _mb_SaveVideo(true)
{
}

_Controllers::DropTriggerCtrl::~DropTriggerCtrl()
{
    _mb_SaveVideo.store(false);

    _m_cond_saveVideo.notify_all();

    if (_m_thread_saveVideo.joinable())
    {
        _m_thread_saveVideo.join();
    }

    for (auto &[name, queue] : _mmap_imgQueue)
    {
        queue.clear();
    }
}

void DropTriggerCtrl::sendArcImage(std::shared_ptr<TIS_Info::ImageBuffer> img)
{
    // 在这之前决定写字幕的方式
    _mmap_imgQueue[img->name].push_back(img);

    _m_cond_saveVideo.notify_one();
}

void _Controllers::DropTriggerCtrl::saveVideoThread()
{
    std::vector<std::string> cameraNames;
    bool isEmpty = true;

    while (_mb_SaveVideo.load())
    {
        std::unique_lock<std::mutex> lock(_m_mutex_saveVideo);
        _m_cond_saveVideo.wait(lock, [this]() {
            return !_mb_SaveVideo.load() ||
                   std::any_of(_mmap_imgQueue.begin(), _mmap_imgQueue.end(), [](const auto &pair) { return !pair.second.empty(); });
        });

        if (!_mb_SaveVideo.load())
        {
            return;
        }

        for (const auto &[name, queue] : _mmap_imgQueue)
        {
            if (!queue.empty())
            {
                cameraNames.push_back(name);
            }
        }

        // 处理图像队列
        for (const auto name_not_empty : cameraNames)
        {
            for (auto &img : _mmap_imgQueue[name_not_empty])
            {
                _mmap_saveVideo[name_not_empty]->pushOriginalImage(img);
                _mmap_imgQueue[name_not_empty].pop_front();
            }
        }

        cameraNames.clear();
    }
}

void _Controllers::DropTriggerCtrl::onStartRecord(QString rootPath, QString fileName)
{
    QString dirPath, filePath;

    for (const auto &[name, saveVideo] : _mmap_saveVideo)
    {
        // 构建完整路径
        dirPath = rootPath + "/" + QString::fromStdString(name);
        filePath = dirPath + "/" + fileName + ".mp4";
        if (_mqstr_SavePath != rootPath)
        {
            _mqstr_SavePath = rootPath; // 更新保存路径
        }

        // 创建目录结构
        QDir dir;
        if (!dir.exists(dirPath))
        {
            if (!dir.mkpath(dirPath))
            {
                qWarning() << "Failed to create directory:" << dirPath;
                continue; // 跳过当前摄像头
            }
        }

        // 开始录像
        saveVideo->startRecord(filePath);

        _Service::App().invokeModuleAsync(TIS_Info::QmlPrivateEngine::callFromCpp,
                                          TIS_Info::QmlCommunication::ForQmlSignals::save_path_send,
                                          QVariant::fromValue(dirPath));
    }
}

void _Controllers::DropTriggerCtrl::onStopRecord()
{
    for (const auto &[name, saveVideo] : _mmap_saveVideo)
    {
        saveVideo->stopRecord();
    }
}

void _Controllers::DropTriggerCtrl::onInitTrigger(std::vector<std::string> cameraName)
{
    for (const auto &name : cameraName)
    {
        if (_mmap_saveVideo.find(name) == _mmap_saveVideo.end())
        {
            _mmap_saveVideo[name] = std::make_unique<_Kits::SaveVideoNew::SaveVideoCache>();
            _mmap_saveVideo[name]->initSaveVideo(1); // 初始化录像kit
        }
    }
}
