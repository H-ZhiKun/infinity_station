
#ifndef _JCWPOSITION_H_
#define _JCWPOSITION_H_
#include "JCWMes.h"
//#include <string>

#ifdef _DEBUG
#define _DEBUG_ME
#endif

#ifndef MAX_POSITION_NAME_LENGHT
#define MAX_POSITION_NAME_LENGHT 32
#endif

#ifndef MAX_MAO_NAME_LENGHT
#define MAX_MAO_NAME_LENGHT 16
#endif

enum JC_POSI_EXTTYPE
{
	JCPOSI_NO_DEFINE = 0,	//未定义
	JCPOSI_RFID_ID = 1,		//读取到RFID标签的位置	string
	JCPOSI_USER_MARK = 2,	//用户备注的信息，用于行车过程中备注一些信息，比如到哪个站了 string
	JCPOSI_TCMSDATA = 3,	//接收到TCMS数据		POSI_TCMSDATA
	JCPOSI_PULSECOUNT = 4,	//脉冲传感器的计数值	UINT64
	JCPOSI_GPS = 5,			//GPS信息			JCP_GPS
};

enum JC_POLE_TYPE
{
	JCPOLE_POLE = 0,	//普通支柱
	JCPOLE_MAO = 1,		//锚段关节
};

//车辆的运行状态
enum JC_TRAINSTATE
{
	TR_STOP,	//停车
	TR_RUNNING,	//运行中
};


//车辆运行方向
enum ValueRunDirection
{
	VALUE_DIRECTION_REV = -1,		//反向
	VALUE_DIRECTION_DEC = -1,		//公里标递减方向
	VALUE_DIRECTION_UNKNOW = 0,	//
	VALUE_DIRECTION_INC = 1,		//公里标递增方向
	VALUE_DIRECTION_SET = 1,		//正向
};

typedef struct
{
	double		dKiloMark;		//公里标，单位km
	UINT64		uiDistance;		//累计的距离，单位1mm，只要车动了，就累加距离
	UINT64		uiPulseID;		//脉冲iD，使用脉冲编码器的统计编码信息，或者其他统计的标识距离的信号
	float		fSpeedKMH;			//运行的速度
}JCP_Distance;		//检测距离信息


typedef struct
{
	char		strStation[MAX_POSITION_NAME_LENGHT];	//站区名
	char		strMaoName[MAX_POSITION_NAME_LENGHT];	//锚段名
	char		strPoleName[MAX_POSITION_NAME_LENGHT];	//支柱名
	int			iPoleMark;		//定位标志，比如是否锚段关节
}JCP_PoleName;	//支柱名


typedef struct
{
public:

	char			strLineName[MAX_POSITION_NAME_LENGHT];			//线路名称，比如xxx一号线上行
	int				iLID;		//数据库中的线路id
	int				iSID;		//保留，暂时不启用
	int				iMID;		//保留，暂时不启用
	int				iUID;		//支柱号在数据库中的id; USER_ID
	ValueRunDirection rundir;	//测量的运行方向，公里标递增递减
	JCP_PoleName	pole;		//当前的支柱位置
	JCP_Distance	distance;	//当前的距离信息、速度、公里标
	JC_TRAINSTATE	state;		//检测车状态
	double			dPoleSpace;	//距离支柱的距离
	POINTF			ptJH;		//数据库中提供的几何参数设计值或者测量值

}JCP_Position;					//检测位置信息	


typedef struct 
{
	SHORT sVer;
	SHORT sSize;
	UINT  uiRecvFrameCount;		//接收数据的总帧数
	CHAR  uiReserve[256 - 8];	//保留的数据区
}JCP_Statistics;

//*******************检测定位接口 **************************


//设置测速脉冲传感器测量的脉冲数（累积数）
JCW_RET Jcw_POSI_SetPulseCount(JCWHANDLE h, INT64 iPulseCount);

//设置TCMS数据；在使用此信号源做定位的时候调用
//fSpeedKM_H 车速(km/h)
//iStationIDNow		 当前站区ID；
//iStationIDNext，	下一站ID
//iStationIDBegin	起始站id
//iStationIDEnd		终点站ID
//iNextStationOffsetMeter		离站距离 ，单位(m)
//return			如果定位功能正常运行，返回JCW_OK，否则返回JCW_ERROR_UNKNOW
JCW_RET Jcw_POSI_SetTCMSData(JCWHANDLE h, float fSpeedKM_H, int iStationIDNow, int iStationIDNext, int iStationIDBegin, int iStationIDEnd, int iNextStationOffsetMeter);

//设置刚刚接收到的电子标签
//strRFIDLabel 接收到的电子标签文本
//return 如果定位功能正常运行，返回JCW_OK，否则返回JCW_ERROR_UNKNOW
JCW_RET Jcw_POSI_SetRFIDLabel(JCWHANDLE h, const char* strRFIDLabel);


//启动定位功能，在软件创建实例后调用一次
//h 实例句柄
//strTaskAddress 检测任务的配置文件，如果为网络地址，则为使用远程定位服务的数据( eg: 127.0.0.1:)
JCW_RET Jcw_POSI_Start(JCWHANDLE h, const char* strTaskAddress);

//结束定位功能。任务结束时候执行
//h 实例句柄
JCW_RET Jcw_POSI_Stop(JCWHANDLE h);

//获取实时定位的信息，当前计算的公里标
//pPosiData 当前实时的定位点的信息
JCW_RET Jcw_POSI_GetRealTimePosition(JCWHANDLE h, OUT JCP_Position* pPosiData);

//获取最新的检测结果的信息
//pJHData = 接触网检测的数据
//pPosition = 数据对应的定位点的信息
//iTimeout = 等待超时(ms)，0 表示不等待，没有结果直接返回；-1表示无限等待，其他表示超时时间，单位毫秒
//其他说明：等待查询数据，如果检测了有结果数据时候，返回JCW_OK，此时数据可用。并且应该即可再查询是否有结果
//return 如果检测了有结果数据时候，返回JCW_OK；如果没有可用数据，则返回 JCW_ERROR_UNKNOW（-1）
JCW_RET Jcw_POSI_PopResult(JCWHANDLE h, OUT JCWJH* pJHData, OUT JCP_Position* pPosition, int iTimeout);

JCW_RET Jcw_POSI_PopResult_jcwd(JCWHANDLE h, OUT JCWD* pJHData, OUT JCP_Position* pPosition, int iTimeout);

JCW_RET Jcw_POSI_GetStatistics(JCWHANDLE h, OUT JCP_Statistics* pData);

enum POSI_SAVEMODE
{
	POSI_SAVEMODE_CLOSE = -1,			//关闭保存功能
	POSI_SAVEMODE_SPLIT_LINE = 0,		//按照线路分割文件
	POSI_SAVEMODE_SPLIT_STATION = 1,	//按照站区分割文件
	//POSI_SAVEMODE_SPLIT_TIME = 2,		//按照时间长度分割文件
	//POSI_SAVEMODE_SPLIT_FILESIZE = 3,	//按照文件大小分割文件
	POSI_SAVEMODE_SPLIT_TASK = 4,
	POSI_SAVEMODE_ONE = 0xff,			//只保存为一个文件

};

enum POSI_FILEFORMAT
{
	POSI_FMT_SQLITE = 0,	//Sqlite 文件格式
	POSI_FMT_CSV = 1,		//csv格式
	POSI_FMT_JDBG = 2,		//衍衍自定义文件格式
	POSI_FMT_KDB = 2,		//衍衍自定义文件格式
};

//JCWHANDLE h 实例句柄
//iSaveMode 保存模式，参考枚举 POSI_SAVEMODE 
//iFormat   保存的文件格式 参考枚举 POSI_FILEFORMAT
//strDstPath 保存的文件名,POSI_SAVEMODE_ONE 直接按照名字保存， 分割模式下，会在名字后面追加分割命名
JCW_RET Jcw_POSI_SaveToFile(JCWHANDLE h, int iSaveMode, int iFormat, LPCTSTR strDstPath);


#endif //_JCWPOSITION_H_