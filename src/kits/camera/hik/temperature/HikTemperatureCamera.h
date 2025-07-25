#pragma once
#include <QObject>
#include <atomic>
#include "yaml-cpp/yaml.h"
#include "HCNetSDK.h"
#include "DecodeCardSdk.h"
#include "DataType.h"
#include "plaympeg4.h"
#include <thread>
#include "tis_global/Struct.h"

namespace _Kits {
class HikTemperatureCamera:public QObject
{

    Q_OBJECT
private:
    /* data */
    static constexpr uint64_t XML_ABILITY_OUT_LEN = 3*1024*1024;
    std::atomic_bool m_initFlag = false;
    std::atomic_bool m_startFlag = false;
    std::atomic_bool m_captureFlag = false;
    LONG m_lUserID = -1;
    LONG m_lRealTimeInfoHandle = -1;
    int m_nChannel = 0;
    // int m_imageWidth = 0;
    // int m_imageHeight = 0;
    int m_minx  = INT_MAX;
	int m_miny = INT_MAX;
	int m_maxx = INT_MIN;
	int m_maxy = INT_MIN;
    std::thread m_captureImageThread;
    std::shared_ptr<TIS_Info::IRImageBuffer> m_IRImageBuffer = nullptr;
    std::atomic<float> m_avgtemp = 0.0;
    std::atomic<float> m_mintemp = 0.0;
    std::atomic<float> m_maxtemp = 0.0;
    std::atomic<float> m_maxpointx = 0;
    std::atomic<float> m_maxpointy = 0;
signals:
    void IRimageReadySignal(std::shared_ptr<TIS_Info::IRImageBuffer> image);
    void IRTempRectSignal(const int startx,const int starty,const int endx,const int end,const int w, const int h);
public:
    explicit HikTemperatureCamera();
    virtual ~HikTemperatureCamera();
public:
    int  openCamera(const YAML::Node &config);
    int  startGrab();
    void stopGrab();
    void closeCamera();
    int  changeVideoCodingType(long lUserID, int type, int iChannelNum);
    int  setP2PParam(long lUserID,int channel);
private:
    int  init();
   // bool getDeviceAbility();
    void handleCallbackData(DWORD dwType, void* lpBuffer, DWORD dwBufLen);
    int  getTemperatureByCallback(const int Channel);
    int  getP2PParam();
    int  parseRectXML(const char* xmldata);
    //int  parseDeviceAbilityXML(const char* xmldata);
    int  setP2PParam();
    int  captureImagePreviewThread();

};

}
