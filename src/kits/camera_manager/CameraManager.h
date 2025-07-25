// #pragma once
// // #include "ImgChannel.h"
// #include "bgapi2_genicam.hpp"
// #include "kits/camera_service/encodevideo.h"
// #include "kits/camera_service/encodevideoffmpeg.h"
// #include "kits/camera_service/muxingvideo.h"
// #include <QDir>
// #include <QFont>
// #include <QImage>
// #include <QObject>
// #include <QPainter>
// #include <QString>
// #include <qimage.h>
// #include <qtmetamacros.h>
// #include <queue>
// #include <qvariant.h>
// #include <string>
// #include <vector>
// #include <yaml-cpp/yaml.h>

// namespace _Kits
// {
// class CameraManager : public QObject
// {
//     Q_OBJECT
//   public:
//     virtual bool openCamera(const YAML::Node &config) = 0;
//     virtual bool closeCamera() = 0;
//     virtual bool startGrab() = 0;
//     virtual bool stopGrab() = 0;
//     virtual bool getImage() = 0;

//   public slots:
//     virtual void OnSubtitleChanged(std::vector<std::string> &subtitle) = 0;
// };
// } // namespace _Kits
