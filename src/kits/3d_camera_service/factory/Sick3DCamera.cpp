#include "Sick3DCamera.h"

std::shared_ptr<R3S>	CSick3DCamera::m_pR3S = NULL;
vector<string>			CSick3DCamera::m_vecDeviceIP;

void SICK_CALLBACK on_callback_function(SickCam::ImgT* pImg, void* pObject)
{
	CSick3DCamera* pCameraObject = (CSick3DCamera*)pObject;

	if (false == pCameraObject->IsGrab())
	{
		return;
	}
	if (NULL == pObject)
		return;
	unsigned char* pImagebuffer = NULL;
	unsigned int		iHigh = 0, iLow = 0;
	int		iFrameCount = 0;

	CGrabImageHook3D* pHook = pCameraObject->GetImageGrabbedHook();
	double	dTimeStamp = 0.0;
	CGwImageHeader3D	imageHead;

	//////////////////////////////////////////////////////////////////////////
	int		i = 0;
	double	dFPS = 0.0;
	int		iValue = 0;
	void* pData = NULL;
	UCHAR	ucArcValueArray[16] = { 0 };

	if (NULL != pImg && NULL != pCameraObject )
	{
		printf("image = %lld\n",pImg->get_ID());
		auto data = pImg->getData();
		auto ChunkInfo = pImg->get_ChunkInfo();
		//get time stamp for line 1;
		INT64	llTemp = 0;
		double	dTimeStamp = 0.0;
		if (ChunkInfo.size() > 0)
		{
			//nanoseconds
			llTemp = ChunkInfo[0].timestamp;
			dTimeStamp = (double)llTemp / 1000000000;
		}
		//copy height image to data
		//gray image
		imageHead.pImageDataGray = data[SickCam::REF_CAL];
		//3d image
		imageHead.pImageData3D = data[SickCam::RAN_CAL_16];
		//encoder 
		imageHead.pEncoder = (uint32_t*)data[SickCam::MAR];
		//image width
		imageHead.iWidth = pImg->get_cols();
		//image height
		imageHead.iHeight = pImg->get_rows();
		//time stamp
		imageHead.dTimeStamp = dTimeStamp;
		//camera us
		imageHead.iCameraUs = llTemp/1000;
		imageHead.fileTimeSystem = GetSTLFileTimeAsInt64();

		__int64 iFrameCount = pCameraObject->GetFrameCount();
		iFrameCount++;
		pCameraObject->SetFrameCount(iFrameCount);
		////set timestamp
		pCameraObject->SetTimeStamp(dTimeStamp);

		imageHead.uiFrameNo = iFrameCount;
		imageHead.fgain = pCameraObject->GetGain();
		imageHead.uiExposureTime = pCameraObject->GetExposure();

		strcpy_s(imageHead.strname, sizeof(imageHead.strname), pCameraObject->GetCameraName().data());

		imageHead.uiOffsetx = pCameraObject->GetOffsetX();
		imageHead.uiOffsety = pCameraObject->GetOffsetY();
		
		//copy data to gray and 3d image
		if (pCameraObject->GetImageWidth() == imageHead.iWidth && pCameraObject->GetImageHeight() == imageHead.iHeight)
		{
			pCameraObject->SetGrayImageData(imageHead.pImageDataGray);
			pCameraObject->Set3dImageData(imageHead.pImageData3D);
		}
		if (NULL != pHook)
			pHook->OnFrameGrabbed(&imageHead);

		pCameraObject->CalFPS(dTimeStamp);

		SickCam::CalibInfo calibInfo = pImg->getCalibration_info();
		pCameraObject->SetRangeInfo(calibInfo.lower_bound_x, calibInfo.upper_bound_x, calibInfo.lower_bound_r, calibInfo.upper_bound_r);
		pCameraObject->SetCalibInfo(calibInfo);
	}
}

bool CSick3DCamera::InitCameraSystem()
{
	bool	bRet = false;
	if (NULL == m_pR3S)
	{
		string	strCti = GetExeDir() + "\\SICKGigEVisionTL.cti";
		m_pR3S = std::make_shared<SickCam::Ranger3Shared>(
			"",											/* 日志文件 */
			strCti.data(),	/* cti 文件 */
			false,										/* 是否打印信息到屏幕 */
			false										/* 是否打印信息到文件 */
		);
		m_vecDeviceIP.clear();
		if (SickCam::CAM_STATUS::All_OK == m_pR3S->scanDevice())
		{
			if (m_pR3S->isAvaliable())
			{
				auto devices = m_pR3S->getConDevListIP();
				for (auto iter = devices.begin(); iter != devices.end(); iter++)
				{
					m_vecDeviceIP.push_back(iter->first);
				}
			}
		}
	}

	bRet = true;
	return	bRet;
}

void CSick3DCamera::GetProfile(vector<float>& vecProfile)
{
	//get first line
	unsigned short	usArray[4096] = { 0 };
	memcpy(usArray, m_mat3D.data, m_iImageWidth * 2);
	vecProfile.clear();
	float	fValue = 0.0;
	for (int i = 0; i < m_iImageWidth; i++)
	{
		fValue = (float)(usArray[i]) * m_dZRatio;
		vecProfile.push_back(fValue);
	}
}

void CSick3DCamera::SetCalibInfo(SickCam::CalibInfo& calibInfo)
{
	m_lockCalibInfo.lock();
	m_cameraCalibInfo = calibInfo;
	m_lockCalibInfo.unlock();
}

string	CSick3DCamera::GetCalibString(int iFrameNo)
{
	string	strCalibJson;
	if (m_strCalibJson.size() < 10)
	{
		SickCam::CalibInfo calibInfo;
		m_lockCalibInfo.lock();
		calibInfo = m_cameraCalibInfo;
		m_lockCalibInfo.unlock();
		//calib to xml file
		strCalibJson = fnCalibToString(iFrameNo);
		m_strCalibJson = strCalibJson;
	}
	return m_strCalibJson;
}

bool CSick3DCamera::SaveCalibXml(const char* pstrFileName)
{
	bool	bRet = false;
	string	strCalibXml;
	FILE* fp = NULL;
	fp = fopen(pstrFileName, "w+");
	strCalibXml = GetCalibString();
	if (NULL != fp)
	{
		if (strCalibXml.size() > 10)
		{
			fwrite(strCalibXml.data(), strCalibXml.length(), 1, fp);
			fclose(fp);
			bRet = true;
		}
	}
	return	bRet;
}

CSick3DCamera::CSick3DCamera()
{
	m_pCamera = NULL;
	m_iImageWidth = 2560;
	m_iImageHeight = 1024;
	m_bStartGrab = false;

	m_iOffsetX = 0;
	m_iOffsetY = 0;
	m_iExposure = 0;
	InitCameraSystem();
}

CSick3DCamera::~CSick3DCamera(void)
{
	CloseCamera();
	m_pR3S.reset();
	LOG_INFO("Camera Close!");
}

bool CSick3DCamera::OpenCamera(int iCameraID, CGrabImageHook3D* pHook)
{
	bool	bRet = false;
	
	if (NULL != m_pCamera)
	{
		//close camera and release
		CloseCamera();
		m_pCamera = NULL;
	}
	if (iCameraID > m_vecDeviceIP.size() || iCameraID < 0)
		return	bRet;
	
	m_bStartGrab = false;

	string	strIP;
	do 
	{
		if (m_vecDeviceIP.size() <= 0)
		{
			break;
		}
		strIP = m_vecDeviceIP[iCameraID];
		m_pCamera = new SickCam::Ranger3(m_pR3S, strIP, true);

		if (NULL == m_pCamera)
		{
			m_strErrorInfo = "Camera create error!";
			break;
		}

		auto err = m_pCamera->isReady();
		if (err != SickCam::CAM_STATUS::All_OK)
		{
			m_strErrorInfo = "Camera create error!";
			break;
		}
		err = m_pCamera->connectCamera();
		m_iDetectThreshold = GetDetectionThreshold();
		m_iExposure = GetExposure();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			break;
		}
		err = m_pCamera->setParameterValue("DeviceScanType", "Linescan3D");

		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			break;
		}
		//set image height		
		err = m_pCamera->setParameter("Height_RegionSelector_Scan3dExtraction1", to_string(m_param.m_iImageHeight));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			break;
		}

		//set capture region roi
		//确保被8整除
		m_param.m_iXs = m_param.m_iXs & ~0x07;
		err = m_pCamera->setParameter("OffsetX_RegionSelector_Region1", to_string(m_param.m_iXs));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			break;
		}
		m_param.m_iYs = m_param.m_iYs & ~0x03;
		err = m_pCamera->setParameter("OffsetY_RegionSelector_Region1", to_string(m_param.m_iYs));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			break;
		}

		//set region width
		int iRegionWidth = m_param.m_iXe - m_param.m_iXs;
		iRegionWidth = iRegionWidth & ~0x1F;
		if (iRegionWidth < 160) iRegionWidth = 160;
		m_param.m_iXe = m_param.m_iXs + iRegionWidth;
		err = m_pCamera->setParameter("Width_RegionSelector_Region1", to_string(iRegionWidth));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			break;
		}

		int iRegionHeight = m_param.m_iYe - m_param.m_iYs;
		iRegionHeight = iRegionHeight & ~0x03;
		if (iRegionHeight < 12) iRegionHeight = 12;
		m_param.m_iYe = m_param.m_iYs + iRegionHeight;
		err = m_pCamera->setParameter("Height_RegionSelector_Region1", to_string(iRegionHeight));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			break;
		}

		//set image width
		m_param.m_iImageWidth = m_param.m_iImageWidth & ~0x1F;  // 0x1F 的二进制表示形式为"11111"

		err = m_pCamera->setCalibration_inDevice(m_param.m_iImageWidth,true);
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			break;
		}
		m_iCameraID = iCameraID;
		string	strValue;

		strValue = m_pCamera->getParameter("OffsetX");
		if (strValue.size() > 0)
		{
			m_iOffsetX = atoi(strValue.data());
		}

		strValue = m_pCamera->getParameter("OffsetY");
		if (strValue.size() > 0)
		{
			m_iOffsetY = atoi(strValue.data());
		}

		strValue = m_pCamera->getParameter("Width_RegionSelector_Scan3dExtraction1");
		if (strValue.size() > 0)
		{
			m_iImageWidth = atoi(strValue.data());
		}

		strValue = m_pCamera->getParameter("Height_RegionSelector_Scan3dExtraction1");
		if (strValue.size() > 0)
		{
			m_iImageHeight = atoi(strValue.data());
		}

		//获得各个标定信息
		strValue = m_pCamera->getParameter("Scan3dCoordinateOffset_Scan3dCoordinateSelector_CoordinateA");
		if (strValue.size() > 0)
		{
			m_dRealXOffset = atof(strValue.data());
		}

		strValue = m_pCamera->getParameter("Scan3dCoordinateScale_Scan3dCoordinateSelector_CoordinateA");
		if (strValue.size() > 0)
		{
			m_dXRatio = atof(strValue.data());
		}

		strValue = m_pCamera->getParameter("Scan3dCoordinateScale_Scan3dCoordinateSelector_CoordinateC");
		if (strValue.size() > 0)
		{
			m_dZRatio = atof(strValue.data());
		}

		strValue = m_pCamera->getParameter("Scan3dCoordinateOffset_Scan3dCoordinateSelector_CoordinateC");
		if (strValue.size() > 0)
		{
			m_dRealZOffset = atof(strValue.data());
		}

		//set line rate
		if (SetLineRate(m_param.m_fLineRate))
		{
			bRet = StartGrab();
		}
	} while (0);

	return	bRet;
}

bool CSick3DCamera::OpenCamera(string strCameraSN, void* pHook)
{
	bool	bRet = false;
	m_iCameraID = -1;
	for (int i = 0; i < m_vecDeviceIP.size(); i++)
	{
		if (strCameraSN == m_vecDeviceIP[i])
		{
			m_iCameraID = i;
			break;
		}
	}
	bRet = OpenCamera(m_iCameraID, pHook);
	return	bRet;
}

bool CSick3DCamera::CloseCamera()
{
	bool	bRet = false;
	if (NULL != m_pCamera)
	{
		bRet = StopGrab();
		//disconnect camera
		auto err = m_pCamera->disconnectCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			bRet = false;
		}
		else
		{
			delete m_pCamera;
			m_pCamera = NULL;
			bRet = true;
		}
	}
	return	bRet;
}

bool CSick3DCamera::StartGrab()
{
	bool	bRet = false;
	string	strValue;

	if (NULL != m_pCamera)
	{
		if (true == m_bStartGrab)
		{
			return	true;
		}
		//get image height
		//get image width
		if (NULL != m_pCamera)
		{
			m_strCalibJson.clear();
			m_bStartGrab = false;
			strValue = m_pCamera->getParameter("Width_RegionSelector_Scan3dExtraction1");
			if (strValue.size() > 0)
			{
				m_iImageWidth = atoi(strValue.data());
			}
		}
		if (NULL != m_pCamera)
		{
			strValue = m_pCamera->getParameter("Height_RegionSelector_Scan3dExtraction1");
			if (strValue.size() > 0)
			{
				m_iImageHeight = atoi(strValue.data());
			}
		}

		//重新分配grayimage和3dimage
		m_matGray.create(m_iImageHeight, m_iImageWidth, CV_8U);
		m_mat3D.create(m_iImageHeight, m_iImageWidth, CV_16U);
		LOG_INFO("Start Camera width = %d height = %d Offset x = %d Offset y = %d lineRate = %.1f MaxLineRate = %.1f ExposureTime = %d", m_iImageWidth, m_iImageHeight, GetOffsetX(), GetOffsetY(), GetLineRate(), GetMaxLineRate(), GetExposure());

		auto err = m_pCamera->startCamera(on_callback_function, this);

		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			bRet = false;
		}
		else
		{
			bRet = true;
			m_bStartGrab = true;
		}
	}
	return	bRet;
}

bool CSick3DCamera::StopGrab()
{
	bool	bRet = false;
	if (NULL != m_pCamera)
	{
		if (m_bStartGrab)
		{
			try
			{
				m_bStartGrab = false;
				Sleep(600);
				//等图像回调完
				auto err = m_pCamera->stopCamera();
				if (SickCam::CAM_STATUS::All_OK != err)
				{
					m_strErrorInfo = SickCam::CAM_STATUS_str(err);
					bRet = false;
				}
				else
				{
					m_bStartGrab = false;
					bRet = true;
				}
			}
			catch (...)
			{
				bRet = false;
			}
		}
	}
	return	bRet;
}

bool CSick3DCamera::SetROI(RECT roi, bool bUpdate /*= false*/)
{
	return	false;
}

//必须在非start状态下，需要先stopgrab 然后startgrab实现改功能
bool CSick3DCamera::SetExposure(int iExposureValue)
{
	bool	bRet = false;
	if (NULL != m_pCamera)
	{
		bool	bStartGrab = m_bStartGrab;
		StopGrab();

		auto err = m_pCamera->setParameter("ExposureTime", std::to_string(iExposureValue));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			bRet = false;
		}
		else
		{
			m_param.m_iExposureTime = iExposureValue;
			bRet = true;
			if( bStartGrab )
				bRet = StartGrab();
		}
		
	}
	return bRet;
}

int CSick3DCamera::GetExposure()
{
	//start 状态下无法读取
	int		iExposure = -1;
	if (m_iExposure > 0)
		return	m_iExposure;

	string	strValue;
	if (NULL != m_pCamera)
	{
		strValue = m_pCamera->getParameter("ExposureTime");
		if (strValue.size() > 0)
		{
			iExposure = atoi(strValue.data());
			m_iExposure = iExposure;
		}
	}
	return iExposure;
}

double CSick3DCamera::GetMaxLineRate()
{
	double	dMaxLineRate = 0;
	string	strValue;
	if (NULL != m_pCamera)
	{
		strValue = m_pCamera->getParameter("MaxLineRate");
		if (strValue.size() > 0)
			dMaxLineRate = atof(strValue.data());
	}
	return dMaxLineRate;
}

double CSick3DCamera::GetLineRate()
{
	double	dLineRate = 0;
	string	strValue;
	if (NULL != m_pCamera)
	{
		strValue = m_pCamera->getParameter("AcquisitionLineRate");
		if (strValue.size() > 0)
			dLineRate = atof(strValue.data());
	}
	return dLineRate;
}

bool CSick3DCamera::SetLineRate(float fFrameRate)
{
	bool	bRet = false;
	double	dMaxLineRate = GetMaxLineRate();
	if (fFrameRate > dMaxLineRate)
	{
		fFrameRate = int(dMaxLineRate);
	}
	//set line rate
	if (NULL != m_pCamera)
	{
		bool	bStartGrab = m_bStartGrab;
		StopGrab();
		auto err = m_pCamera->setParameter("AcquisitionLineRate", to_string(fFrameRate));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			bRet = false;
		}
		else
		{
			bRet = true;
			m_param.m_fLineRate = fFrameRate;
			if( bStartGrab )
				bRet = StartGrab();
			
		}
	}
	return	bRet;
}

bool CSick3DCamera::GetSensorSize(int& iWidth, int& iHeight)
{
	return	false;
}

bool CSick3DCamera::GetStatisticsInfo(int& iLostPacket, int& iLostFrames, int& iGoodFrames)
{
	return	false;
}

bool CSick3DCamera::GetCameraSNList(vector<string>& vecCamerSNList)
{
	m_vecDeviceIP = vecCamerSNList;
	return	true;
}

bool CSick3DCamera::ResetStatistics()
{
	return	false;
}

int CSick3DCamera::GetOffsetX()
{
	return	m_iOffsetX;
}

int CSick3DCamera::GetOffsetY()
{
	return	m_iOffsetY;
}

bool CSick3DCamera::SetImageHeight(int iImageHeight)
{
	bool	bRet = false;
	
	//set line rate
	if (NULL != m_pCamera)
	{
		StopGrab();
		
		auto err = m_pCamera->setParameter("Height_RegionSelector_Scan3dExtraction1", to_string(iImageHeight));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			bRet = false;
		}
		else
		{
			m_iImageHeight = iImageHeight;
			m_param.m_iImageHeight = iImageHeight;
			StartGrab();
			bRet = true;
		}
	}
	return	bRet;
}

bool CSick3DCamera::SetImageWidth(int iImageWidth)
{
	bool	bRet = false;
	CloseCamera();
	iImageWidth = iImageWidth & ~0x1F;  // 0x1F 的二进制表示形式为"11111"
	//得到传感器宽度
	string	strValue;
	bool	bStartGrab = m_bStartGrab;
	if (iImageWidth > 4096)
		iImageWidth = 4096;
	if (iImageWidth < 160)
		iImageWidth = 160;
	m_param.m_iImageWidth = iImageWidth;
	bRet = OpenCamera(&m_param, m_pHook);
	return	bRet;
}

int CSick3DCamera::GetDetectionThreshold()
{
	//start 状态下无法读取
	int		iDetectThreshold = -1;
	if (m_iDetectThreshold > 0)
		return	m_iDetectThreshold;

	string	strValue;
	if (NULL != m_pCamera)
	{
		strValue = m_pCamera->getParameter("DetectionThreshold");
		if (strValue.size() > 0)
		{
			iDetectThreshold = atoi(strValue.data());
			m_iDetectThreshold = iDetectThreshold;
		}
	}
	return iDetectThreshold;
}

bool CSick3DCamera::SetDetectionThreshold(int iThreshold)
{
	bool	bRet = false;
	if (NULL != m_pCamera)
	{
		StopGrab();

		auto err = m_pCamera->setParameter("DetectionThreshold", std::to_string(iThreshold));
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			m_strErrorInfo = SickCam::CAM_STATUS_str(err);
			bRet = false;
		}
		else
		{
			bRet = StartGrab();
		}

	}
	return bRet;
}

bool CSick3DCamera::SetTrigger(bool bTriggerON /*= true*/)
{
	return	false;
}

bool CSick3DCamera::SetTriggeSetting(int iTriggerLine, int iTriggerAct, int iTriggerSvr)
{
	return	false;
}

bool CSick3DCamera::GetTrigger()
{
	return	false;
}

bool CSick3DCamera::SaveConfigFile(const char* pstrFileName)
{
	return	false;
}

bool CSick3DCamera::SaveParamToCamera()
{
	return	false;
}


bool CSick3DCamera::SetBufferCount(int iBufferCount)
{
	bool	bRet = false;
	if (NULL != m_pCamera)
	{
		StopGrab();
		m_lock.lock();
		if (SickCam::CAM_STATUS::All_OK == m_pCamera->setBufferCount(iBufferCount))
			bRet = true;
		m_lock.unlock();
		StartGrab();
	}
	
	return	bRet;
}

int CSick3DCamera::GetBufferCount()
{
	int	iBufferCount = 0;
	m_lock.lock();
	if (NULL != m_pCamera)
	{
		iBufferCount = m_pCamera->getBufferCount();
	}
	m_lock.unlock();

	return	iBufferCount;
}

void CSick3DCamera::GetDevicesIP(vector<string>& vecDeviceIP)
{
	vecDeviceIP = m_vecDeviceIP;
}


string CSick3DCamera::fnCalibToString(int iFrameID)
{
	std::ostringstream  xmlFile;
	xmlFile << "<icon_data_format name=\"\" valuetype=\"\">\n";
	xmlFile << "  <parameter name=\"size\">" << std::to_string(m_iImageWidth*3) << "</parameter>\n";
	xmlFile << "  <parameter name=\"version\">1</parameter>\n";
	xmlFile << "  <parameter name=\"layout\">SUBCOMPONENT</parameter>\n";
	xmlFile << "  <component name=\"Ranger3Range\" valuetype=\"Hi3D\">\n";
	xmlFile << "    <parameter name=\"size\">" << std::to_string(m_iImageWidth * 3) << "</parameter>\n";
	xmlFile << "    <parameter name=\"height\">1</parameter>\n";

	xmlFile << "    <worldrangetraits>\n";
	xmlFile << "      <parameter name=\"lower bound x\">" << std::to_string(m_cameraCalibInfo.lower_bound_x) << "</parameter>\n";
	xmlFile << "      <parameter name=\"lower bound r\">" << std::to_string(m_cameraCalibInfo.lower_bound_r) << "</parameter>\n";
	xmlFile << "      <parameter name=\"upper bound x\">" << std::to_string(m_cameraCalibInfo.upper_bound_x) << "</parameter>\n";
	xmlFile << "      <parameter name=\"upper bound r\">" << std::to_string(m_cameraCalibInfo.upper_bound_r) << "</parameter>\n";
	xmlFile << "      <parameter name=\"coordinate unit\"></parameter>\n";
	xmlFile << "    </worldrangetraits>\n";

	xmlFile << "    <genistreamtraits>\n";
	xmlFile << "      <parameter name=\"region id\">scan 3d extraction 1</parameter>\n";
	xmlFile << "      <parameter name=\"extraction method\">hi 3d</parameter>\n";
	xmlFile << "      <parameter name=\"output mode\">" << m_cameraCalibInfo.genistreamtraits.output_mode << "</parameter>\n";
	xmlFile << "      <parameter name=\"width\">" << std::to_string(m_param.m_iXe-m_param.m_iXs) << "</parameter>\n";
	//xmlFile << "      <parameter name=\"width\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.width) << "</parameter>\n";
	xmlFile << "      <parameter name=\"height\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.height) << "</parameter>\n";
	xmlFile << "      <parameter name=\"offset X\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.offset_X) << "</parameter>\n";
	xmlFile << "      <parameter name=\"offset Y\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.offset_Y) << "</parameter>\n";

	xmlFile << "      <parameter name=\"a axis range scale\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.a_axis_range_scale) << "</parameter>\n";
	xmlFile << "      <parameter name=\"a axis range offset\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.a_axis_range_offset) << "</parameter>\n";
	xmlFile << "      <parameter name=\"a axis range min\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.a_axis_range_min) << "</parameter>\n";
	//xmlFile << "      <parameter name=\"a axis range max\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.a_axis_range_max) << "</parameter>\n";
	xmlFile << "      <parameter name=\"a axis range max\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.a_axis_range_max-1) << "</parameter>\n";
	xmlFile << "      <parameter name=\"a axis range missing\">" << m_cameraCalibInfo.genistreamtraits.a_axis_range_missing.c_str() << "</parameter>\n";
	xmlFile << "      <parameter name=\"a axis range missing value\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.a_axis_range_missing_value) << "</parameter>\n";

	xmlFile << "      <parameter name=\"b axis range scale\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.b_axis_range_scale) << "</parameter>\n";
	xmlFile << "      <parameter name=\"b axis range offset\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.b_axis_range_offset) << "</parameter>\n";
	xmlFile << "      <parameter name=\"b axis range min\">" << m_cameraCalibInfo.genistreamtraits.b_axis_range_min << "</parameter>\n";
	xmlFile << "      <parameter name=\"b axis range max\">" << m_cameraCalibInfo.genistreamtraits.b_axis_range_max << "</parameter>\n";
	xmlFile << "      <parameter name=\"b axis range missing\">" << m_cameraCalibInfo.genistreamtraits.b_axis_range_missing << "</parameter>\n";
	xmlFile << "      <parameter name=\"b axis range missing value\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.b_axis_range_missing_value) << "</parameter>\n";

	xmlFile << "      <parameter name=\"c axis range scale\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.c_axis_range_scale) << "</parameter>\n";
	xmlFile << "      <parameter name=\"c axis range offset\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.c_axis_range_offset) << "</parameter>\n";
	xmlFile << "      <parameter name=\"c axis range min\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.c_axis_range_min) << "</parameter>\n";
	xmlFile << "      <parameter name=\"c axis range max\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.c_axis_range_max) << "</parameter>\n";
	xmlFile << "      <parameter name=\"c axis range missing\">" << m_cameraCalibInfo.genistreamtraits.c_axis_range_missing << "</parameter>\n";
	xmlFile << "      <parameter name=\"c axis range missing value\">" << std::to_string(m_cameraCalibInfo.genistreamtraits.c_axis_range_missing_value) << "</parameter>\n";

	xmlFile << "      <parameter name=\"unit\">" << "millimeter" << "</parameter>\n";
	xmlFile << "    </genistreamtraits>\n";
	
	//write data info

	xmlFile << "    <subcomponent name=\"" << "Range" << "\" valuetype=\"" << "WORD" << "\">\n";
	xmlFile << "      <parameter name=\"size\">" << std::to_string(m_iImageWidth*2) << "</parameter>\n";
	xmlFile << "      <parameter name=\"width\">" << std::to_string(m_iImageWidth) << "</parameter>\n";
	xmlFile << "    </subcomponent>\n";

	xmlFile << "    <subcomponent name=\"" << "Intensity" << "\" valuetype=\"" << "BYTE" << "\">\n";
	xmlFile << "      <parameter name=\"size\">" << std::to_string(m_iImageWidth) << "</parameter>\n";
	xmlFile << "      <parameter name=\"width\">" << std::to_string(m_iImageWidth) << "</parameter>\n";
	xmlFile << "    </subcomponent>\n";

	xmlFile << "  </component>\n";
	xmlFile << "      <parameter name=\"version\">" << "8.9.0.6" << "</parameter>\n";
	xmlFile << "      <parameter name=\"frame id\">" << iFrameID << "</parameter>\n";
	xmlFile << "      <parameter name=\"frame completeness\">" << "complete" << "</parameter>\n";
	xmlFile << "      <parameter name=\"delivered frame height\">" << m_iImageHeight << "</parameter>\n";
	xmlFile << "  <genistream>\n";
	xmlFile << "</icon_data_format>";

	return	xmlFile.str();
}