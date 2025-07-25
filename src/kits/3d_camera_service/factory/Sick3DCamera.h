#ifndef _HIKI_CAMERA_H_
#define _HIKI_CAMERA_H_
#include "ranger3.h"
#include <vector>
#include "Camera3DInterface.h"

using namespace std;

class CSick3DCamera : public Camera3DInterface
{
public:
	CSick3DCamera();
	virtual ~CSick3DCamera(void);

	virtual const char*	GetClassName() { return "CSick3DCamera"; }
	virtual bool	OpenCamera(int iCameraID, void* pHook);
	virtual bool	CloseCamera();

	virtual bool	StartGrab();
	virtual bool	StopGrab();
	virtual bool	SetROI(RECT roi, bool bUpdate = false);
	virtual bool	SetExposure(int iExposureValue);
	virtual int		GetExposure();
	virtual	double	GetMaxLineRate();
	virtual	double	GetLineRate();
	virtual bool	SetLineRate(float fFrameRate);
	virtual bool	GetSensorSize(int& iWidth, int& iHeight);
	virtual bool	GetStatisticsInfo(int& iLostPacket, int& iLostFrames, int& iGoodFrames);

	virtual	bool	GetCameraSNList(vector<string>& vecCamerSNList);
	virtual bool	ResetStatistics();
	virtual int		GetOffsetX();
	virtual int		GetOffsetY();
	virtual bool	SetImageHeight(int iImageHeight);
	virtual	bool	SetImageWidth(int iImageWidth);

	virtual	int		GetDetectionThreshold();
	virtual bool	SetDetectionThreshold(int iThreshold);

	virtual	bool	SetTrigger(bool	bTriggerON = true);
	virtual bool	SetTriggeSetting(int iTriggerLine, int  iTriggerAct, int iTriggerSvr);
	virtual	bool	GetTrigger();

	bool			SaveConfigFile(const char* pstrFileName);
	bool			SaveParamToCamera();
	bool			SetBufferCount(int iBufferCount);
	int				GetBufferCount();
	void		GetDevicesIP(vector<string>& vecDeviceIP);
	
	int		GetScanCameraCount() { return m_vecDeviceIP.size(); }
	bool		InitCameraSystem();
	virtual void	GetProfile(vector<float>& vecProfile);
	void			SetCalibInfo(SickCam::CalibInfo& calibInfo);
	virtual string			GetCalibString(int iFrameNo = 0);
	bool			SaveCalibXml(const char* pstrFileName);
protected:
	static std::shared_ptr<R3S>	m_pR3S;
	static vector<string>		m_vecDeviceIP;//西克扫描到的相机IP

	string fnCalibToString(int iFrameID = 0);
	SickCam::Ranger3*			m_pCamera;
	SickCam::CalibInfo			m_cameraCalibInfo;
	std::mutex					m_lockCalibInfo;

public:
};
#endif