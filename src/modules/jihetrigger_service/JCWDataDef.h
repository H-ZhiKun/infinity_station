
#ifndef _JCWDATA_DEF_H_
#define _JCWDATA_DEF_H_

#pragma once 
#include <windows.h>
#include <stdint.h>
#ifndef _WIN32
typedef struct tagPOINTF
{
	float x;
	float y;
} 	POINTF;
#else
#include <OCIdl.h>
#endif

//测量的最大导线的数量
#define JCWJH_MAX_LINE_NUM 4
#define JCW_INVALIDVALUE 0x10000

#define GRADE_INVALIDVALUE 0
#define MOHAO_INVALIDVALUE 0
#define MOHAO_MIN_VALUE 0.01	//最小的磨耗有效值，0.01就认为是误差


#define JCW_OK 0						//执行正确完成
#define JCW_ERROR_UNKNOW -1				//发生未知错误
#define JCW_INVALID_HANDLE -2			//无效的实例句柄
#define JCW_ERR_INVALID_DEVID -3		//无效设备ID
#define JCW_ERR_ALREADY_START -4		//已启动，不能更改设置
#define JCW_ERR_NO_START -5				//未启动，不能更改设置
#define JCW_ERR_INVALID_PARAMETER -6	//无效参数值，参数超出有效范围，或者参数组合无效
#define JCW_ERR_CFG_FILE_NO_EXIST -7	//文件不存在
#define JCW_ERR_CFG_INVALID_CONTENT -8	//配置文件无效内容，或者文件格式不正确 
#define JCW_ERR_CFG_DEVICE_NULL -9		//未配置设备，设备的配置数量为空
#define JCW_ERR_CREATE_COMM_FAIL -10	//创建通信失败
#define JCW_ERR_DEV_CONN_FAIL -11		//链接到设备失败
#define JCW_ERR_EMPTY -13				//数据为空
#define JCW_ERR_MODE -14				//模式错误


#define JCW_ERR_NOIMPLEMENT -404	//未实现

typedef void* JCWHANDLE;
typedef int JCW_RET;

enum JCWDVER
{
	JVER_JCWJH = 0,		//jcwjh 结构体
	JVER_JCWD = 1,		//jcwd 结构体
	JVER_JSON = 2,		//json格式
	JVER_PROTOC = 3,	//protobuf数据流
};


//设备ID编制
enum JMDEVID
{
	JMID_GLOBAL = 0,		//全局对象，比如调整整体的导高、拉出值
	
	JMID_COMP_LEFT = 1,		//左补偿
	JMID_COMP_RIGHT = 2,	//右补偿

	JMID_GEO_DEV0 = 11,		//几何参数1
	JMID_GEO_DEV1 = 12,		//几何参数2
	JMID_GEO_DEV2 = 13,		//几何参数3
	JMID_GEO_DEV3 = 14,
	JMID_GEO_DEV4 = 15,
	JMID_GEO_DEV5 = 16,
	JMID_GEO_DEV6 = 17,
	JMID_GEO_DEV7 = 18,


	JMID_TRACK3_LEFT = 21,	//第三轨左轨
	JMID_TRACK3_RIGHT = 22, //第三轨右轨


	JMID_COMP_LEFT_RF = 101,	//特殊左补偿
	JMID_COMP_RIGHT_RF = 102,	//特殊右补偿
};

enum JCWPosi
{
	JCXP_UNKNOW = 0,	//未知
	JCXP_LINE = 1,		//导线位置
	JCXP_DROPPER = 2,	//吊弦位置
	JCXP_POLE = 3,		//支柱位置
	JCXP_ELECCONN = 4,
	JCXP_MAO = 5,
	JCXP_POLE_NO_WORKER = 6,	//非支的定位点
};				//导线定位点类

//接触线类型
enum JCXType
{
	JCXT_UNKNOW = 0,	//未知导线类型
	JCXT_FLEX = 1,		//柔性导线
	JCXT_RIGI = 2,		//刚性导线
	JCXT_RF = 3,		//刚柔性过渡
	JCXT_TRACK3	= 0x31,	//第三轨
};


//IO输出口工作模式
enum JCWOUTPUT
{
	JCWOUTPUT_OFF = 0,			//断开
	JCWOUTPUT_ON = 1,			//闭合
	JCWOUTPUT_EXPOSURE = 2,		//同步曝光
	JCWOUTPUT_TRIGER = 3,		//触发模式
	JCWOUTPUT_SIMULATION = 4,	//模拟触发
};


enum JCWD_ORDER
{
	JCWD_ORDER_HEIGHT = 0,			//按照导高排序
	JCWD_ORDER_LINE = 1,			//按照导线分组排序
	JCWD_ORDER_JCX_WORKER = 2,		//按照工作支的方式排序
};


//导线处理的信息
union JcwJcxProcInfo
{
	struct
	{
		uint8_t uiJcxGrayConstant;		//导线成像的对比度
		uint8_t uiJcxGrayBackAver;		//图像背景灰度；
		uint8_t uiJcxLaserWidth;		//激光线的宽度
		uint8_t	iPointNum;				//点的数量
		uint8_t uiScore;				//检测得分;0~100
		uint8_t iPosiMethod;			//判断定位点的方法
		uint8_t	iPosiOutputTimeMsDelay;	//输出定位IO信号的延迟；单位ms；最大255ms;
		uint8_t	uiImageGrayAver;		//成像的平均灰度
	};

	int64_t iValue;

};


union JcwCamStateInfo
{
	struct
	{
		int8_t		iCamTempture;		//相机温度
		uint8_t		iCamTempture1;		//温度点1
		uint8_t		uiGain10;	//		//图像增益 * 10
		uint8_t		iCamStateFlag;		//各个状态	参考_JcwCamStateFlag
		uint16_t	uiCamGrabRate;		//相机帧率(fps)
		uint16_t	uiExposureTimeUS;	//曝光时间(us)
	}info;
	int64_t iValue;
};



//检测相机的状态标识 
enum _JcwCamStateFlag
{
	JCWCAMSTATE_LASER_OFF = 0x01,			//激光是否关闭
	JCWCAMSTATE_EXPOSURE_AUTO = 0x02,		//自动曝光
	JCWCAMSTATE_FRAMERATE_AUTO = 0x04,		//自动调节帧率
	JCWCAMSTATE_OFFSETY_AUTO = 0x08,		//是否自动调整图像窗口
	JCWCAMSTATE_SOFT_SYNC_SLAVE = 0x10,		//是否软同步状态
	JCWCAMSTATE_TRIGER_GRAB = 0x20,			//外触发采集图像
	JCWCAMSTATE_ISO_OUTPUT_ON = 0x40,			//IO-Output-状态是否ON
	JCWCAMSTATE_CAM_DISCONNECT_ERROR = 0x80,		//相机工作工作发生错误，或者未连接
};


union JcwCamGroupStateFlag
{
	struct
	{
		int8_t iStateFlag[8];		//对应_JcwCamStateFlag；排序按照JMID_DEV_GEO0 ~JMID_DEV_GEO7
	}cams;
	int64_t iValue;
};


#ifndef _JCSYNCTIME_
#define _JCSYNCTIME_

//系统同步数据
struct JCSyncTime
{
public:
	uint64_t	ui64DevTime;		//时间戳
	int64_t	ft;					//本地的FILETIME，参考Windows api
	int		iReserve;			//保留参数
};
#endif


//接触线磨耗值
struct JCXMH
{
public:
	JCXMH()
	{
		Reset();
		return;
	}

	void Reset()
	{
		iLineRidus_UM = 0;	//0 = 未知
		fMsH = 0;			//磨损厚度, <0 = 未知
		sPM = 0;
		fBotLen = 0;		//底面磨损面的长度 <= 0 = 未知
		sResver1 = 0;
		sResver2 = 0;
		return;
	}

public:
	USHORT   iLineRidus_UM;		//导线直径，指定导线的类型.单位(um);eg 13200,15200
	SHORT	 sPM;				//导线偏磨，单位角度(0.01°)
	float	 fMsH;				//磨损的厚度(mm)	 <=0 = 无效值，表示未测量到结果；有效测量最小值 >=0.01
	SHORT	 sResver1;			//保留，不要使用
	SHORT	 sResver2;			//保留，不要使用
	float	 fBotLen;			//底面的长度(mm)	 <= 0 = 无效值，表示未测量到结果 有效测量最小值 >=0.01
};


//接触线测量数据
struct JCXData
{
public:
	JCXData()
	{
		Reset();
		return;
	}

	void Reset()
	{
		pntLinePos.x = JCW_INVALIDVALUE;
		pntLinePos.y = JCW_INVALIDVALUE;
		mh.Reset();
		lt = JCXT_UNKNOW;
		posi = JCXP_UNKNOW;
		return;
	}
public:
	POINTF pntLinePos;	//导线空间位置;x = 拉出值; y = 导高
	JCXMH  mh;			//磨耗值

	JCXType lt;			//接触线类型
	JCWPosi posi;		//导线是否定位导线
};


//几何参数检测的结果结构体，保留用于兼容旧版本程序。
struct JCWJH
{
public:
	JCWJH()
	{
		Reset();
		return;
	}

	void Reset()
	{
		sVersion = 3;	//版本2，时间戳使用FILETIME.版本1，时间戳单位是s; V3：增加定位器坡度
		sBagSize = sizeof(*this);
		uiFrameNo = 0;
		sGrade = 0;		
		dTimestamp = 0;
		uiFileTime = 0;
		uiLineNum = 0;
		uiLineCompsateNum = 0;
		ilt = 0;

		for (int i = JCWJH_MAX_LINE_NUM; i--;)
		{
			jcx[i].Reset();
			jcxComp[i].Reset();
			ucDataDevSrc[i] = 0;
		}

		pntCompLeft.x = JCW_INVALIDVALUE;
		pntCompLeft.y = JCW_INVALIDVALUE;
		pntCompRight.x = JCW_INVALIDVALUE;
		pntCompRight.y = JCW_INVALIDVALUE;
		return;
	}

	bool HasCompste()
	{
		return (pntCompLeft.x != JCW_INVALIDVALUE) && (pntCompRight.x != JCW_INVALIDVALUE);
	}

	int GetWorkerIndex(UINT i)
	{
		int iIndex = 3;
		if (i < uiLineNum)
		{
			int iMov = i * 2;
			iIndex = (ucWokerIndex >> iMov) & 0x03;
		}
		return iIndex;
	}
	
public:
	short	sVersion;		//数据的版本
	short	sBagSize;		//数据包大小
	UCHAR	ucDataDevSrc[4];//数据来源于哪个设备
	SHORT	sGrade;			//定位器坡度  单位0.01°;比如数值是 2010，则定位器坡度 = 2010 * 0.01° = 20.1°; 0 表示没有检测到定位器坡度
	UCHAR	ucWokerIndex;	//标记导线的工作支顺序，每根导线用2bit标识序号
	CHAR	cTemperature;	//检测环境的温度℃
	UINT32	uiFrameNo;		//帧号
	double	dTimestamp;		//接收时候的时间戳，单位秒
	UINT64  uiFileTime;		//测量的时间，参考windows FILETIME

	UINT	uiLineNum;		//测量到的导线数量，对应 数组：jcx
	JCXData jcx[JCWJH_MAX_LINE_NUM];		//未补偿的几何参数，通过 uiLineNum 表示有效内容数量
	JCXData jcxComp[JCWJH_MAX_LINE_NUM];	//补偿后的几何参数;通过 uiLineCompsateNum 表示有效内容数量

	POINTF	pntCompLeft;		//左补偿 ，x表水平，y表垂直；单位mm
	POINTF	pntCompRight;		//右补偿

	JCWPosi posi;				//是否定位点
	int		ilt;				//导线的类型
	UINT	uiLineCompsateNum;	//补偿后有效的导线数量，对应数组:jcxComp
};


//接触线测量数据
struct JCXD
{
public:

#define  JCX_VER 1

	JCXD()
	{
		Reset();
		return;
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
		this->usVer = JCX_VER;
		this->usSize = sizeof(*this);

		pntLinePos.x = JCW_INVALIDVALUE;
		pntLinePos.y = JCW_INVALIDVALUE;
		pntCompPos.x = JCW_INVALIDVALUE;
		pntCompPos.y = JCW_INVALIDVALUE;

		fLineRidus = MOHAO_INVALIDVALUE;
		fPianMo = MOHAO_INVALIDVALUE;
		fMohaoH = MOHAO_INVALIDVALUE;
		fMhBotLen = MOHAO_INVALIDVALUE;

		lt = JCXT_UNKNOW;
		posi = JCXP_UNKNOW;
		iJcxId = 0;
		iJcDevId = 0;
		fHLPAngle = 0;
		ucWeight = 0;
		iJcxStateInfo.iValue = 0;
		iCamStateInfo.iValue = 0;
		
		return;
	}
public:
	uint16_t usVer;				//数据版本，接受数据的时候可以做检查
	uint16_t usSize;			//数据包大小，数据包可能会拓展一些信息
	POINTF pntLinePos;			//导线空间位置;x = 拉出值; y = 导高
	POINTF pntCompPos;			//补偿后的导高拉出值

	//磨耗结果
	float   fLineRidus;			//导线直径，测量导线的类型.单位(mm);
	float	fPianMo;			//导线偏磨，单位角度(0.01°)
	float	fMohaoH;			//磨损的厚度(mm)	 <=0 = 无效值，表示未测量到结果；有效测量最小值 >=0.01
	float	fMhBotLen;			//底面的长度(mm)	 <= 0 = 无效值，表示未测量到结果 有效测量最小值 >=0.01

	JCXType lt;					//接触线类型
	JCWPosi posi;				//导线是否定位导线
	float   fHLPAngle;			//偏转角; 单位(°)，针对汇流排，相对于车顶平面的偏转角。有些相机并不支持此功能
	float	fFallOffsetMM;		//导线跟汇流排脱落距离(mm)
	uint32_t iJcxId;			//接触线编号，通过导线归组，组id
	uint8_t  iJcDevId;			//检测设备的id JMDEVID
	uint8_t	 ucWeight;			//权重
	int8_t	iMidPercent;			//靠近中间位置比例：-100~100
	JcwCamStateInfo	iCamStateInfo;		//对应相机状态信息
	JcwJcxProcInfo	iJcxStateInfo;		//导线处理状态信息

	int64_t	iCamAppVer;				//测量相机软件的版本；a.b.c.d(uint64[0:15].uint64[16:31].uint64[32:47].uint64[48:63])
	int64_t	iR2;				//保留字节，用于将来拓展功能使用
	int64_t	iR3;				//保留字节，用于将来拓展功能使用
	
};

struct JCWD
{
public:
#define JCWD_VER 1
	JCWD()
	{
		Reset();
		return;
	}

	void Reset()
	{
		memset(&fGrade, 0, sizeof(*this) - (((char*)&fGrade) - ((char*)this)));

		ucVersion = JCWD_VER;
		usBagSize = sizeof(*this);

		ucOrderType = JCWD_ORDER_HEIGHT;
		iTimeDelay_MS = 0;
		uiTimeDelay_MS_CL = 0;
		uiTimeDelay_MS_CR = 0;
		uiTimeDelay_MS_GEO = 0;

		iTimeStampUS = 0;
		uiFrameNo = 0;
		fGrade = GRADE_INVALIDVALUE;
		uiFileTime = 0;
		uiLineNum = 0;
		posi = JCXP_UNKNOW;
		iUserID = 0;
		iJcxWorkerFlag = 0;
		iJHCamStateFlag.iValue = 0;

		for (int i = JCWJH_MAX_LINE_NUM; i--;)
		{
			jcx[i].Reset();
		}

		pntCompLeft.x = JCW_INVALIDVALUE;
		pntCompLeft.y = JCW_INVALIDVALUE;
		pntCompRight.x = JCW_INVALIDVALUE;
		pntCompRight.y = JCW_INVALIDVALUE;

		pntCompLeft2.x = JCW_INVALIDVALUE;
		pntCompLeft2.y = JCW_INVALIDVALUE;
		pntCompRight2.x = JCW_INVALIDVALUE;
		pntCompRight2.y = JCW_INVALIDVALUE;

		return;
	}

	//是否存在补偿值
	bool HasCompste()
	{
		return (pntCompLeft.x != JCW_INVALIDVALUE) && (pntCompRight.x != JCW_INVALIDVALUE);
	}

	//获取是否定位的标识
	JCWPosi GetPosiflag()
	{
		JCWPosi jp = JCXP_UNKNOW;
		for (int i = JCWJH_MAX_LINE_NUM; i--; )
		{
			if (jcx[i].posi > jp)
			{
				jp = jcx[i].posi;
			}
		}
		return jp;
	}

	//获取补偿后导线的数量，取决于有没有测到导线，有没有测到有效的补偿值
	uint32_t GetCompNum()
	{
		uint32_t uiRet = 0;
		if (JCWD_ORDER_HEIGHT == ucOrderType)
		{
			for (int i = 0; i < uiLineNum; i++)
			{
				if (JCW_INVALIDVALUE != jcx[i].pntCompPos.x)
				{
					uiRet++;
				}
			}
		}
		else if (JCWD_ORDER_LINE == ucOrderType || JCWD_ORDER_JCX_WORKER == ucOrderType)
		{
			for (int i = 0; i < JCWJH_MAX_LINE_NUM; i++)
			{
				if (JCW_INVALIDVALUE != jcx[i].pntCompPos.x)
				{
					uiRet++;
				}
			}
		}


		return uiRet;
	}

public:
	uint8_t	 ucVersion;		//数据的版本
	uint8_t	 ucOrderType;	//排序的方式，0 = 按导高从低到高排序，1 = 按导线分组穿插排序 ，参考上方枚举定义： JCWD_ORDER
	uint16_t usBagSize;		//数据包的大小

	uint16_t iTimeDelay_MS;	//数据的延迟时间，单位ms； = time.now() - 测量时刻的时间(iTimeStampUS)；0~65534 为准确数；65535说明延迟的时间很大，时间已溢出
	uint8_t  uiTimeDelay_MS_CL;	//左补偿的延迟时间（单位ms），255表示延迟大到容不下； 表示从测量~系统接收（非用户接收）的延迟
	uint8_t  uiTimeDelay_MS_CR;	//右补偿的延迟时间（单位ms），255表示延迟大到容不下； 表示从测量~系统接收（非用户接收）的延迟

	int64_t	 iTimeStampUS;	//测量的时间戳，单位us，通过时间间隔，了解相机的测量密度；时间戳是本机的时间戳。通过时间戳可以获知当前数据的延迟
	int64_t	 uiFrameNo;		//测量帧号，帧号是连续累加1的。所以可通过号码的连续性，判断是否丢帧
	int64_t  uiFileTime;	//测量的时间，参考windows FILETIME； 时间是运行本程序的计算机的时间。
	int64_t	 iUserID;		//用户设定的id

	uint32_t	 uiLineNum;					//测量到的导线数量，对应 数组：jcx
	uint32_t	uiTimeDelay_MS_GEO;			//由4个uint8组合，表示4个几何相机的接收延迟，超过4个几何相机的无法表示

	JCXD	 jcx[JCWJH_MAX_LINE_NUM];		//几何参数，通过 uiLineNum 表示有效内容数量
	float	 fGrade;						//定位器坡度  单位°; 0 表示没有检测到定位器坡度

	POINTF	pntCompLeft;				//左补偿 ，x表水平，y表垂直；单位mm
	POINTF	pntCompRight;				//右补偿
		
	POINTF	pntCompLeft2;				//左补偿 ，x表水平，y表垂直；单位mm，非工作轨位置
	POINTF	pntCompRight2;				//右补偿			//非工作轨位置，道岔位置

	JCWPosi	posi;						//定位的标识，通过各种渠道判断的结果，不一定来源于导线的识别

	int32_t	iJcxWorkerFlag;				//接触线工作支标识；每4bit代表数组的index，然后其中的数值标识按照工作支排序的序号
	uint16_t iGradeLenght;				//定位器坡度的长度
	uint16_t iR1;						//保留，用于将来拓展其他的信息

	JcwCamStateInfo	iCamStateInfo_CL;			//左补偿相机状态信息
	JcwCamStateInfo	iCamStateInfo_CR;			//右补偿相机状态信息

	uint64_t iSysMoveDistanceMM;		//整套相机的整体运行距离（累积距离)，单位mm。需要用户提供车速才有此数据，误差也受速度误差影响。
	float	 fMovSpeedKMH;				//当前的运行速度(km/h)
	uint32_t iSampFrameNo;				//距离采样帧号，每按距离采样一次数据，帧号自加1；可通过号码的连续性，判断是否丢帧
	uint16_t iSampFrameInterval;		//距离采样中间间隔的帧数量；真实最大值是65534; 如果是65535，则说明间隔的帧溢出，以至于不记录具体的数量。0 表示连续数据，非距离采样数据。
	uint16_t iMovDistanceDelay_CM;		//数据的延迟距离(单位:cm) = 当前最新位置 - 数据测量时的位置（this->iSysMoveDistanceMM）;	
	uint32_t iJcwlibVer;				//处理库的版本 byte3.bytep[2:1].byte0; eg:23.303.1
	JcwCamGroupStateFlag	iJHCamStateFlag;			//几何相机的处理状态，8个byte，每个byte对应一个相机，表示各个相机的工作状态
};


#endif // !_JCWDATA_DEF_H_