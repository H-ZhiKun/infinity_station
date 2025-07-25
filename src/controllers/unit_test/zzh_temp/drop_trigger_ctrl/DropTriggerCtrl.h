#pragma once
#include "kits/common/object_pool/ObjectPool.h"
#include "kits/common/controller_base/ControllerBase.h"
#include "tis_global/Field.h"
#include "tis_global/Function.h"
#include "tis_global/Struct.h"

#include "kits/common/save_video_by_ffmpeg/SaveVideoCache.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace _Controllers
{
    class DropTriggerCtrl : public ControllerBase<DropTriggerCtrl>
    {
      public:
        DropTriggerCtrl();
        ~DropTriggerCtrl();

        void sendArcImage(std::shared_ptr<TIS_Info::ImageBuffer>);
        void onStartRecord(QString, QString);
        void onStopRecord();

        /// @brief 用作初始化录像kit，由模块触发
        /// @param cameraName(std::vector<std::string>) 录像kit的名称列表
        /// @return void
        /// @note 这个函数会在录像kit初始化时调用，通常在录像开始前调用一次即可
        void onInitTrigger(std::vector<std::string>);

        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::BaslerManager::sendImage, DropTriggerCtrl::sendArcImage);
        ASYNC_TASK_ADD(TIS_Info::BaslerManager::startRecordTriggerByOneCam, DropTriggerCtrl::onStartRecord);
        ASYNC_TASK_ADD(TIS_Info::BaslerManager::stopRecordTriggerByOneCam, DropTriggerCtrl::onStopRecord);
        ASYNC_TASK_ADD(TIS_Info::BaslerManager::initTriggerCtrls, DropTriggerCtrl::onInitTrigger);
        TASK_LIST_END

      private:
        std::unordered_map<std::string, std::deque<std::shared_ptr<TIS_Info::ImageBuffer>>> _mmap_imgQueue;
        std::unordered_map<std::string, std::unique_ptr<_Kits::SaveVideoNew::SaveVideoCache>> _mmap_saveVideo;

        std::thread _m_thread_saveVideo;
        std::condition_variable _m_cond_saveVideo;
        std::mutex _m_mutex_saveVideo;
        std::atomic_bool _mb_SaveVideo;

        QString _mqstr_SavePath;

      private:
        void saveVideoThread();

      signals:
        void saveImage(std::shared_ptr<TIS_Info::ImageBuffer> img);
    };
} // namespace _Controllers