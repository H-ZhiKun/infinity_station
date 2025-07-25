#include"CameraServer3D.h"
#include"camera3d_factory.h"
CameraServer3D::CameraServer3D(QObject* parent = nullptr) : QObject(parent) 
{

}


bool CameraServer3D::ScanCamera(const YAML::Node &config)
{
    int type = config["camera3d_type"].as<int>();
    std::unique_ptr<_Kits::Camera3DInterface> Camera3Dptr=Camera3DFactory::createCamera3D(type);
    m_pMulCamera->push_back(std::move(Camera3Dptr));
    m_iCameraCount=Camera3Dptr->GetScanCameraCount();
    for (size_t i = 1; i < m_iCameraCount; i++)
    {
        m_pMulCamera->push_back(std::move(Camera3DFactory::createCamera3D(type)));
        /* code */
    }
}
bool CameraServer3D::openCamera() 
{

}

bool CameraServer3D::closeCamera() 
{

}

bool CameraServer3D::startGrab() 
{

}
bool CameraServer3D::stopGrab()
{
}
bool CameraServer3D::getImage()
{
    
}
