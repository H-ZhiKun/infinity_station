#ifndef _CAMERA_USER_H
#define _CAMERA_USER_H

#include "kits/camera_service/CameraBase.h"
#include "kits/common/module_base/ModuleBase.h"
// #include "modules/camera_user/detect_kit/panto/include/TogeeAlgorithmLib.h"
#include "CameraCtrl.h"
#include "DetectCtrl.h"
#include "LocationCtrl.h"
#include <QImage>
#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <string>
#include <thread>

#include "tis_global/Struct.h"

#ifdef _WIN32
#include <objbase.h>
#include <qwindowdefs_win.h>
#endif
namespace _Modules
{
    class CameraUser : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(CameraUser)
      public:
        CameraUser(QObject *parent = nullptr);
        virtual ~CameraUser();

        CameraUser(const CameraUser &) = delete;
        CameraUser &operator=(const CameraUser &) = delete;
        // CameraUser(CameraUser &&) = delete;

      public:
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void subtitleSend(std::vector<std::string> &var);
        void startRecord(const std::string &filename);
        void stopRecord();
        void saveImage(QImage image, const _Kits::CameraInfo &camera_info, const QDateTime &dateTime);
        void imgout(const QVariant &var);
        void CallFromArc(const QVariant &value);
        void sendPathToArcuv(const QVariant &path); // 发送保存路径至燃弧
        void sendTaskPathToCtl(std::string path);   // 发送任务路径至控制器
        void flagRecord(bool);

      public slots:
        void SubtitleChange(const QVariant &var);
        void CameraStartRecord(const QVariant &filename);
        void CameraStopRecord();
        // _Kits::ReturnCode OnGetImage(const QImage &image,
        //                              const std::string &camera_name);
        void PantographDetect(QImage &image, const std::string &camera_name);
        // _Kits::ReturnCode OnGetImageInfo(const _Kits::ImgInfo &imgInfo);
        void OnSaveImage(QImage image, const _Kits::CameraInfo &camera_info, const QDateTime &dateTime);

        void onCallFromArc(const QVariant &value);

      private slots:
        void notifyTask(TIS_Info::TaskInfo);

      private:
        YAML::Node m_details;
        std::vector<std::shared_ptr<CameraCtrl>> m_allCameraCtrl;
        std::unique_ptr<LocationCtrl> m_locationCtrl = nullptr;
        std::unique_ptr<DetectCtrl> m_detectCtrl = nullptr;
        // pantographDetectionModel m_detect_model;
        // detectResults m_detect_results;
        // pantographDetectionInput m_detect_input;
        _Kits::ImgInfo m_imgInfo;
        std::atomic_bool m_isclose = true;

      private:
        void ReadCameraConfig(const YAML::Node &config, _Kits::CameraInfo &camera_info);
        void createCamera(const YAML::Node &config_node);

      private:
        class ComInitTool
        {
          public:
            ComInitTool()
            {
#ifdef _WIN32
                HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
                if (FAILED(hr))
                {
                    return;
                }
#endif
            }
            ~ComInitTool()
            {
#ifdef _WIN32
                CoUninitialize();
#endif
            }
        };
        inline static ComInitTool m_cominit;
    };

} // namespace _Modules

#endif