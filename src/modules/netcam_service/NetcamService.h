#pragma once

#include "kits/camera_service/basler/CameraBasler.h"
#include "kits/common/module_base/ModuleBase.h"
#include "tis_global/Struct.h"
#include <deque>

namespace _Modules
{

    class NetcamService : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(NetcamService)

      public:
        explicit NetcamService(QObject *parent = nullptr);

        virtual ~NetcamService() noexcept override;
        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        void imgout(TIS_Info::ImageInfo);
        void imgoutqml(TIS_Info::ImageInfo);
        void isRecord(TIS_Info::RecordInfo);
        void initinfo(const QVariantMap &);

      private slots:
        void onGetImage(const QImage image, const std::string &camera_name);
        void onRecvRecordByQml(QVariant);
        void onTrigger();

      private:
        std::unordered_map<std::string, std::unique_ptr<_Kits::CameraBasler>> m_mapCamera;
        QVariantMap m_mapInitinfo;
        YAML::Node m_yaml_config;
        QString mqstr_record_path;
        std::string serialNumber;

      private:
        void ReadCameraConfig(const YAML::Node &config, _Kits::CameraInfo &camera_info);
    };

} // namespace _Modules