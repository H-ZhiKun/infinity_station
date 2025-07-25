#pragma once

#include "mutex"
#include<vector>
#include<QImage>
//#include "opencv2/opencv.hpp"


#define MAX_CAMERACOUNT 16

using namespace std;

namespace _Kits {


class	Camera3DInterface
{
	
public:
Camera3DInterface();
	virtual ~Camera3DInterface() { ; }
	//扫描
	virtual int GetScanCameraCount()=0;


	virtual const char* GetClassName() { return "CameraInterface3D"; }
	virtual bool	OpenCamera(int iCameraID, void* pHook) = 0;

	virtual bool	CloseCamera() = 0;
	virtual bool	StartGrab() = 0;
	virtual bool	StopGrab() = 0;
	virtual bool	SetExposure(int iExposureValue) = 0;
	virtual int		GetExposure() = 0;

	virtual int		GetImageWidth();
	virtual int		GetImageHeight();
	virtual float	GetGain(){return 0.0f;};
	virtual	bool	SetGain(float fGain){};
	virtual bool	SetLineRate(float fFrameRate){};
	virtual double	GetLineRate(){return 0.0f;};
	virtual	double	GetMaxLineRate(){return 0.0f;};
	virtual double	GetFPS(){return 0.0f;};
	virtual int		GetOffsetX(){return 0;};
	virtual int		GetOffsetY(){return 0;};
	virtual bool	GetGrayImage(QImage& pGrayImage, bool bChangeUpdate = true){return false;};
	virtual bool	GetImage3D(QImage& pImage3D, bool bChangeUpdate = true){return false;};
	virtual	bool	SetGrayImageData(void* pGrayImageData){return false;};
	virtual	bool	Set3dImageData(void* p3DImageData){return false;};
	virtual bool	SetDetectionThreshold(int iThreshold){return false;};
	virtual	int		GetDetectionThreshold(){return 0;};
	virtual	bool	IsGrab(){return false;};

	virtual __int64		GetFrameCount(){return 0;};
	virtual	void	SetFrameCount(__int64 iFrameCount){};
	void * GetImageGrabbedHook() { return m_pHook; }
	virtual	bool	SetHook(void* pHook){return false;};
	void CalFPS(double dTimeStamp);
	void			SetTimeStamp(double dTimeStamp);
	virtual	string	GetCameraName(){return "";};
	virtual bool	GetProfile(vector<float>& vecProfile){return false;};

	double			GetXRatio();
	double			GetZRatio();
	double			GetRealXOffset();
	double			GetRealZOffset();

	double			GetXMin();
	double			GetXMax();
	double			GetZMin();
	double			GetZMax();

	void			SetRangeInfo(double dXMin, double dXMax, double dZMin, double dZMax);
	virtual string			GetCalibString(int iFrameNo = 0){return "";	};
protected:
	void * 		m_pHook;
	int			m_iImageWidth;
	int			m_iImageHeight;
	
	string		m_strErrorInfo;
	int			m_iCameraID;
	std::mutex	m_lock;
	double		m_dFps;
	int			m_iCallFpsFrameCount;
	int			m_iFpsCount;
	double		m_dFpsTimeStampStart;
	uint64_t		m_iFrameCount;
	double		m_dTimeStamp;
	QImage		m_matGray;
	QImage		m_mat3D;
	std::mutex	m_lockMatGray;
	std::mutex	m_lockMat3D;
	std::mutex	m_lockProfile;
	int			m_iDetectThreshold;

	int			m_iOffsetX;
	int			m_iOffsetY;
	int			m_iExposure;
	double		m_dLineRate;

	bool		m_bStartGrab;

	double			m_dXRatio;
	double			m_dRealXOffset;
	double			m_dZRatio;
	double			m_dRealZOffset;
	vector<float>	m_vecProfle;

	double			m_dXmin;
	double			m_dXmax;
	double			m_dZMin;
	double			m_dZMax;
	string			m_strCalibJson;
};
}