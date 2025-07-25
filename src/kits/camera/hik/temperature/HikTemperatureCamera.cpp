#include "kits/common/log/CRossLogger.h"
#include "HikTemperatureCamera.h"
#include <string>
#include <QXmlStreamWriter>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QImage>
#include <iostream>

namespace _Kits
{

    HikTemperatureCamera::HikTemperatureCamera(/* args */)
    {
    }

    HikTemperatureCamera::~HikTemperatureCamera()
    {
        stopGrab();
    }

    int HikTemperatureCamera::init()
    {
        if (m_initFlag.load())
        {
            return 0;
        }
        auto ret = NET_DVR_Init();
        if (!ret)
        {
            auto err = NET_DVR_GetLastError();
            LogError("NET_DVR_Init failed!:{}", err);
            return -1;
        }
        NET_DVR_SetConnectTime(2000, 1);
        NET_DVR_SetReconnect(10000, true);
        m_initFlag.store(true);
        return 0;
    }
    int HikTemperatureCamera::openCamera(const YAML::Node &config)
    {
        if (m_startFlag.load())
        {
            return 0;
        }

        if (!m_initFlag.load())
        {
            int ret = init();
            if (0 != ret)
            {
                return -1;
            }
        }

        auto strDeviceIP = config["deviceip"].as<std::string>();
        auto nDeviceport = config["deviceport"].as<int>();
        auto strUserName = config["username"].as<std::string>();
        auto strPassword = config["password"].as<std::string>();
        m_nChannel = config["channel"].as<int>();

        auto irconfig = config["irdata"];
        auto getTemperatrueType = irconfig["irtype"].as<uint32_t>();

        NET_DVR_USER_LOGIN_INFO struLoginInfo = {0}; // 登录设备前的配置信息，至少需要先知道用户名、密码、Ip、port 等
        NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {
            0}; // 登录设备成功后，设备将自身的信息回写到这个结构体，如序列号、通道信息等，后面全局都会使用
        struLoginInfo.bUseAsynLogin = false;
        struLoginInfo.wPort = nDeviceport;
        std::memcpy(struLoginInfo.sDeviceAddress, strDeviceIP.c_str(), strDeviceIP.length());
        std::memcpy(struLoginInfo.sUserName, strUserName.c_str(), strUserName.length());
        std::memcpy(struLoginInfo.sPassword, strPassword.c_str(), strPassword.length());

        m_lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
        if (m_lUserID < 0)
        {
            auto err = NET_DVR_GetLastError();
            LogError("NET_DVR_Login_V40 failed, error code: {}", err);
            return -1;
        }

        if (0 == getTemperatrueType)
        {
            auto ret = getTemperatureByCallback(m_nChannel);
            if (0 != ret)
            {
                LogError("getTemperatureByCallback failed");
                return -2;
            }
        }

        getP2PParam();
        LogInfo("NET_DVR_Login_V40 success");
        m_startFlag.store(true);
        return 0;
    }

    int HikTemperatureCamera::getTemperatureByCallback(const int Channel)
    {
        NET_DVR_REALTIME_THERMOMETRY_COND struCond = {0};
        struCond.dwSize = sizeof(NET_DVR_REALTIME_THERMOMETRY_COND);
        struCond.dwChan = Channel;
        struCond.byRuleID = 0;
        auto getThermInfoCallback = [](DWORD dwType, void *lpBuffer, DWORD dwBufLen, void *pUserData) {
            HikTemperatureCamera *pHikCamera = (HikTemperatureCamera *)pUserData;
            if (nullptr != pHikCamera)
            {
                pHikCamera->handleCallbackData(dwType, lpBuffer, dwBufLen);
            }
        };

        m_lRealTimeInfoHandle =
            NET_DVR_StartRemoteConfig(m_lUserID, NET_DVR_GET_REALTIME_THERMOMETRY, &struCond, sizeof(struCond), getThermInfoCallback, this);
        if (m_lRealTimeInfoHandle < 0)
        {
            auto err = NET_DVR_GetLastError();
            LogError("NET_DVR_StartRemoteConfig failed, error code: {}", err);
            return -1;
        }

        return 0;
    }
    int HikTemperatureCamera::startGrab()
    {
        NET_DVR_SetCapturePictureMode(JPEG_MODE);

        setP2PParam();
        // getDeviceAbility();
        m_captureFlag.store(true);
        m_captureImageThread = std::thread(&HikTemperatureCamera::captureImagePreviewThread, this);
        return 0;
    }

    void HikTemperatureCamera::stopGrab()
    {
        m_captureFlag.store(false);
        if (m_captureImageThread.joinable())
        {
            m_captureImageThread.join();
        }
    }
    void HikTemperatureCamera::closeCamera()
    {
        if (m_startFlag.load())
        {
            NET_DVR_StopRemoteConfig(m_lRealTimeInfoHandle);
            // 注销用户
            NET_DVR_Logout(m_lUserID);
            // 释放SDK资源
            NET_DVR_Cleanup();
        }

        m_startFlag.store(false);
    }

    void HikTemperatureCamera::handleCallbackData(DWORD dwType, void *lpBuffer, DWORD dwBufLen)
    {
        if (dwType == NET_SDK_CALLBACK_TYPE_DATA)
        {
            NET_DVR_THERMOMETRY_UPLOAD struThermometryBuff = {0};
            std::memcpy(&struThermometryBuff, lpBuffer, sizeof(NET_DVR_THERMOMETRY_UPLOAD));
            if (1 == struThermometryBuff.byRuleID)
            {
                LogTrace("struThermometryBuff:{},{},{}",
                         struThermometryBuff.struLinePolygonThermCfg.fMaxTemperature,
                         struThermometryBuff.struLinePolygonThermCfg.fMinTemperature,
                         struThermometryBuff.struLinePolygonThermCfg.fAverageTemperature);
                m_avgtemp.store(struThermometryBuff.struLinePolygonThermCfg.fAverageTemperature);
                m_maxtemp.store(struThermometryBuff.struLinePolygonThermCfg.fMaxTemperature);
                m_mintemp.store(struThermometryBuff.struLinePolygonThermCfg.fMinTemperature);
                m_maxpointx.store(struThermometryBuff.struHighestPoint.fX);
                m_maxpointy.store(struThermometryBuff.struHighestPoint.fY);
            }
        }
        else if (dwType == NET_SDK_CALLBACK_TYPE_STATUS)
        {
            // DWORD dwStatus = *(DWORD*)lpBuffer;
            // if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
            // {
            // 	printf("dwStatus:NET_SDK_CALLBACK_STATUS_SUCCESS\n");
            // }
            // else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
            // {
            // 	DWORD dwErrCode = *(DWORD*)((char *)lpBuffer + 4);
            // 	printf("NET_DVR_GET_MANUALTHERM_INFO failed, Error code %d\n", dwErrCode);
            // }
        }
    }
    int HikTemperatureCamera::getP2PParam()
    {
        // 输出参数
        NET_DVR_XML_CONFIG_INPUT struInput = {0};
        struInput.dwSize = sizeof(struInput);

        // 输出参数
        NET_DVR_XML_CONFIG_OUTPUT struOutputParam = {0};
        struOutputParam.dwSize = sizeof(struOutputParam);

        // URL
        char szUrl[512] = {0};
        memset(szUrl, 0, sizeof(szUrl));
        sprintf(szUrl, "%s", "GET /ISAPI/Thermal/channels/1/thermometry/1");
        struInput.lpRequestUrl = szUrl;
        struInput.dwRequestUrlLen = strlen(szUrl);

        // 获取时输入为空
        struInput.lpInBuffer = NULL;
        struInput.dwInBufferSize = 0;

        // 分配输出内存
        //  char szGetOutput[16 * 1024] = { 0 };
        auto pOutBuf = std::make_unique<char[]>(XML_ABILITY_OUT_LEN);
        struOutputParam.lpOutBuffer = pOutBuf.get();
        struOutputParam.dwOutBufferSize = XML_ABILITY_OUT_LEN;

        // 输出状态
        char szStatusBuf[1024] = {0};
        struOutputParam.lpStatusBuffer = szStatusBuf;
        struOutputParam.dwStatusSize = sizeof(szStatusBuf);

        if (!NET_DVR_STDXMLConfig(m_lUserID, &struInput, &struOutputParam))
        {
            LogError("NET_DVR_STDXMLConfig failed, error code: %d\n", NET_DVR_GetLastError());
            return -1;
        }
        else
        {
            int ret = parseRectXML(pOutBuf.get());
        }
        return 0;
    }
    int HikTemperatureCamera::parseRectXML(const char *xmldata)
    {
        QXmlStreamReader xml(xmldata);
        bool inRegionCoordinates = false;
        int curX = 0, curY = 0;
        QString currentTag;
        int normalwidth = 0;
        int normalheight = 0;

        while (!xml.atEnd())
        {
            xml.readNext();

            if (xml.isStartElement())
            {
                currentTag = xml.name().toString();
                if ("RegionCoordinates" == currentTag)
                {
                    inRegionCoordinates = true;
                    curX = curY = 0;
                }
            }
            else if (xml.isEndElement())
            {
                if ("RegionCoordinates" == xml.name() && inRegionCoordinates)
                {
                    // Update bounding box
                    m_minx = qMin(m_minx, curX);
                    m_maxx = qMax(m_maxx, curX);
                    m_miny = qMin(m_miny, curY);
                    m_maxy = qMax(m_maxy, curY);
                    inRegionCoordinates = false;
                }
            }
            else if (xml.isCharacters() && !xml.isWhitespace())
            {
                if (inRegionCoordinates)
                {
                    if ("positionX" == currentTag)
                    {
                        curX = xml.text().toInt();
                    }
                    else if ("positionY" == currentTag)
                    {
                        curY = xml.text().toInt();
                    }
                }
                else
                {
                    if ("normalizedScreenWidth" == currentTag)
                    {
                        normalwidth = xml.text().toInt();
                    }
                    else if ("normalizedScreenHeight" == currentTag)
                    {
                        normalheight = xml.text().toInt();
                    }
                }
            }
        }
        LogDebug("min:{},{},max:{},{}", m_minx, m_minx, m_maxx, m_maxy);
        if (xml.hasError())
        {
            LogError("xml parse error");
            return -1;
        }
        // std::cout << "xml parse success" << normalheight  << ":" << normalwidth << std::endl;
        emit IRTempRectSignal(m_minx, m_miny, m_maxx, m_maxy, normalwidth, normalheight);
        return 0;
    }
    int HikTemperatureCamera::setP2PParam()
    {
        auto StatusBuffer = std::make_unique<char[]>(10240);
        auto OutBuffer = std::make_unique<char[]>(10240);

        NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
        NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
        char distance_unit[] = "centimeter";

        QString xmlStr;
        QXmlStreamWriter writer(&xmlStr);

        writer.setAutoFormatting(true);
        writer.writeStartDocument();

        writer.writeStartElement("PixelToPixelParam");

        writer.writeTextElement("id", QString::number(m_nChannel));
        writer.writeTextElement("maxFrameRate", QString::number(600));
        writer.writeTextElement("reflectiveEnable", "1");
        writer.writeTextElement("reflectiveTemperature", QString::number(20.00));
        writer.writeTextElement("emissivity", QString::number(0.96));
        // writer.writeTextElement("distance", QString::number(300));
        writer.writeTextElement("refreshInterval", QString::number(50));
        writer.writeTextElement("distanceUnit", QString::fromUtf8(distance_unit));
        writer.writeTextElement("temperatureDataLength", QString::number(2));

        // 写入 JpegPictureWithAppendData 嵌套结构
        writer.writeStartElement("JpegPictureWithAppendData");
        writer.writeTextElement("jpegPicEnabled", "1");

        // 兼容两个 visiblePic 字段
        writer.writeTextElement("visiblePicEnable", "0");
        writer.writeTextElement("visiblePicEnabled", "0");
        writer.writeEndElement(); // JpegPictureWithAppendData

        writer.writeEndElement(); // PixelToPixelParam
        writer.writeEndDocument();

        lpInputParam.dwSize = sizeof(lpInputParam);
        lpOutputParam.dwSize = sizeof(lpOutputParam);

        std::string strurl = "PUT /ISAPI/Thermal/channels/1/thermometry/pixelToPixelParam";
        if (1 == m_nChannel)
        {
            strurl = "PUT /ISAPI/Thermal/channels/1/thermometry/pixelToPixelParam";
        }
        else if (2 == m_nChannel)
        {
            strurl = "PUT /ISAPI/Thermal/channels/2/thermometry/pixelToPixelParam";
        }

        lpInputParam.lpRequestUrl = strurl.data();
        lpInputParam.dwRequestUrlLen = strurl.length();

        lpInputParam.lpInBuffer = xmlStr.data();
        lpInputParam.dwInBufferSize = xmlStr.length();

        lpOutputParam.lpOutBuffer = OutBuffer.get();
        lpOutputParam.dwOutBufferSize = 10240;

        lpOutputParam.lpStatusBuffer = StatusBuffer.get();
        lpOutputParam.dwStatusSize = 10240;

        if (NET_DVR_STDXMLConfig(m_lUserID, &lpInputParam, &lpOutputParam))
        {
            LogInfo("Set pixelToPixelParam Success!");
            return 0;
        }
        else
        {
            LogError("Set pixelToPixelParam Failed!:{}", NET_DVR_GetLastError());
            return -1;
        }
        return -1;
    }
    int HikTemperatureCamera::captureImagePreviewThread()
    {
        NET_DVR_JPEGPICTURE_WITH_APPENDDATA struJpegData = {0};
        // char szLan[256] = { 0 };
        auto pPicOutBuf = std::make_unique<char[]>(XML_ABILITY_OUT_LEN);
        auto pP2POutBuf = std::make_unique<char[]>(XML_ABILITY_OUT_LEN);
        auto pVisibleOutBuf = std::make_unique<char[]>(10 * 1024 * 1024);
        if (struJpegData.pJpegPicBuff == NULL)
        {
            struJpegData.pJpegPicBuff = pPicOutBuf.get();
            memset(struJpegData.pJpegPicBuff, 0, XML_ABILITY_OUT_LEN);
        }
        if (struJpegData.pP2PDataBuff == NULL)
        {
            struJpegData.pP2PDataBuff = pP2POutBuf.get();
            memset(struJpegData.pP2PDataBuff, 0, XML_ABILITY_OUT_LEN);
        }

        if (struJpegData.pVisiblePicBuff == NULL) // 可见光图至少为4M
        {
            struJpegData.pVisiblePicBuff = pVisibleOutBuf.get();
            memset(struJpegData.pVisiblePicBuff, 0, 10 * 1024 * 1024);
        }

        // DWORD lpSizeReturned = 0;
        while (m_captureFlag.load())
        {
            auto bret = NET_DVR_CaptureJPEGPicture_WithAppendData(m_lUserID, m_nChannel, &struJpegData);
            if (!bret)
            {
                LONG errorCode = 0;
                char *strErr = NET_DVR_GetErrorMsg(&errorCode);
                LogError("NET_DVR_CaptureJPEGPicture_WithAppendData errorCode:{}, strErr:{}", errorCode, strErr);
            }
            else
            {
                int imagesize = struJpegData.dwJpegPicWidth * struJpegData.dwJpegPicHeight;
                if (nullptr == m_IRImageBuffer)
                {
                    m_IRImageBuffer = std::make_shared<TIS_Info::IRImageBuffer>(imagesize, imagesize * 4);
                }
                m_IRImageBuffer->width = struJpegData.dwJpegPicWidth;
                m_IRImageBuffer->height = struJpegData.dwJpegPicHeight;
                m_IRImageBuffer->avgtemp = m_avgtemp.load();
                m_IRImageBuffer->maxtemp = m_maxtemp.load();
                m_IRImageBuffer->mintemp = m_mintemp.load();
                m_IRImageBuffer->maxpointx = m_maxpointx.load();
                m_IRImageBuffer->maxpointy = m_maxpointy.load();
                m_IRImageBuffer->iamgesize = struJpegData.dwJpegPicLen;
                std::copy(struJpegData.pJpegPicBuff, struJpegData.pJpegPicBuff + struJpegData.dwJpegPicLen, m_IRImageBuffer->data.begin());
                std::copy(
                    struJpegData.pP2PDataBuff, struJpegData.pP2PDataBuff + struJpegData.dwP2PDataLen, m_IRImageBuffer->tempdata.begin());
                emit IRimageReadySignal(m_IRImageBuffer);
                LogTrace("NET_DVR_CapturePictureBlock_New:width:{},hegith:{},piclen:{},datalen:{}",
                         struJpegData.dwJpegPicWidth,
                         struJpegData.dwJpegPicHeight,
                         struJpegData.dwJpegPicLen,
                         struJpegData.dwP2PDataLen);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
        return 0;
    }

} // namespace _Kits