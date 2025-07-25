#pragma once
#include <QObject>
#include <yaml-cpp/yaml.h>
#include"factory/Camera3DInterface.h"
#include<vector>
namespace _Kits {

class CameraServer3D : public QObject 
{
    Q_OBJECT
public:
    CameraServer3D(QObject* parent = nullptr);
     ~CameraServer3D() = default;

     bool ScanCamera(const YAML::Node &config);
     bool openCamera();
     bool closeCamera();
     bool startGrab();
     bool stopGrab();
     bool getImage();
    virtual std::string getCameraName();

signals:
    void ImageOutPut(QImage image, const std::string &camera_name);

public slots:
     void OnStartRecord() ;
     void OnStopRecord() ;

protected:
    vector<std::shared_ptr<_Kits::Camera3DInterface>> m_pMulCamera;
    uint16_t m_iCameraCount;
};

}