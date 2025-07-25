#pragma once
#include "kits/camera/hik/camera_normal/HikCamera.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <QObject>
#include "kits/common/module_base/ModuleBase.h"
#include "kits/camera/tools/LocationCtrl.h"
#include "tis_global/Struct.h"

namespace _Modules
{
    class HikManager : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(HikManager)
      public:
        explicit HikManager(QObject *parent = nullptr);
        virtual ~HikManager() noexcept;
        bool init(const YAML::Node &config) override;
        bool start() override;
        bool stop() override;

      signals:
        void initVideoCtrl(std::vector<std::string>);
        void initArcCtrl(std::vector<std::string>);
        void initHikNvrCtrl(TIS_Info::HikLogInfo);

        void sendImage(std::shared_ptr<TIS_Info::ImageBuffer>);
        void sendArcImage(std::shared_ptr<TIS_Info::ImageBuffer>); // 可能会存在，先预留

        void sendSubTitle(std::vector<QString>);

        /// @brief 录像指令
        /// @param  第一个为rootPath
        /// @param  第二个为录像名称
        /// @note   主要需要将录像名称和录像路径一起传递，在controller处拼接名字
        void startRecord(QString, QString);
        void stopRecord();

        /// @brief 燃弧录像指令
        /// @param  第一个为rootPath
        /// @param  第二个为录像名称
        /// @note   主要需要将录像名称和录像路径一起传递，在controller处拼接名字
        void startArc(QString, QString);
        void stopArc();

      private slots:
        void notifyTask(TIS_Info::TaskInfo);

      public slots:
        void onLocation(const QVariant &var);
        void onStartRecord(QString filename);

      private:
        std::unordered_map<std::string, std::shared_ptr<_Kits::HikBase>> m_cameraMap_;
        std::unordered_map<std::string, std::string> mmap_CameraIp_Name; // 用于存储相机Ip和名字
        std::vector<TIS_Info::HikLogInfo> mvec_cameraInfo_;
        std::unique_ptr<_Kits::LocationCtrl> m_locationCtrl_;
        std::vector<std::string> mvec_CameraName_video_;
        std::vector<std::string> mvec_CameraName_arc_;
        QString _mqstr_SavePath; // 保存图片的路径
    };
} // namespace _Modules