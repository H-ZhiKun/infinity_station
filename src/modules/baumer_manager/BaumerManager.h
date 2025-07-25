#pragma once
#include "kits/camera/baumer/camera_normal/BaumerCamera.h"
#include "kits/camera/baumer/base/BaumerBase.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <QObject>
#include "kits/common/module_base/ModuleBase.h"
#include "kits/camera/tools/LocationCtrl.h"

namespace _Modules
{
    class BaumerManager : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(BaumerManager)
      public:
        explicit BaumerManager(QObject *parent = nullptr);
        virtual ~BaumerManager();
        bool init(const YAML::Node &config) override;
        bool start() override;
        bool stop() override;
        bool searchCamera(BGAPI2::Interface *ifc, const std::string &needNumber = "");
        bool searchCamera(const std::string &needNumber = "");
        bool removeCamera(const std::string &snNumber);

      signals:
        void initVideoCtrl(std::vector<std::string>);
        void initArcCtrl(std::vector<std::string>);

        void sendImage(std::shared_ptr<TIS_Info::ImageBuffer>);
        void sendArcImage(std::shared_ptr<TIS_Info::ImageBuffer>);

        /// @brief 录像指令
        /// @param  第一个为录像名称
        /// @param  第二个为rootPath
        /// @note   主要需要将录像名称和录像路径一起传递，在controller处拼接名字
        void startRecord(QString, QString);
        void stopRecord();

        /// @brief 燃弧录像指令
        /// @param  第一个为录像名称
        /// @param  第二个为rootPath
        /// @note   主要需要将录像名称和录像路径一起传递，在controller处拼接名字
        void startArc(TIS_Info::arcData, QString);
        void stopArc();

        void sendSubTitle(std::vector<QString>);

      private slots:
        void notifyTask(TIS_Info::TaskInfo);
        void onLocation(const QVariant &var);
        void onStartRecord(QString);
        void onStartArc(TIS_Info::arcData ); // 提供给燃弧模块的槽 触发，开始录像，通过acr

      private:
        static void BGAPI2CALL PnPEventHandler(void *callBackOwner, BGAPI2::Events::PnPEvent *pBuffer);
        void initSystem();
        void initInterface();
        void deInit();
        void closeCameras();
        std::thread m_thInit_;
        std::thread m_thread_start_;
        BGAPI2::System *m_pSystem = nullptr;
        bool m_bInit = false;
        bool m_bStart = false;
        bool m_bStop = false;

        std::unordered_map<std::string, std::unique_ptr<_Kits::BaumerBase>> m_mapCamera;
        std::unique_ptr<_Kits::LocationCtrl> m_locationCtrl_;
        std::unordered_map<std::string, std::string> mmap_CameraSN_Name; // 用于存储相机序列号和名字
        std::unordered_map<std::string, std::string> mmap_CameraSN_Type; // 用于存储相机序列号和类型
        std::vector<std::string> mvec_CameraName_video_;
        std::vector<std::string> mvec_CameraName_arc_;
        QString _mqstr_SavePath; // 保存图片的路径
    };
} // namespace _Modules