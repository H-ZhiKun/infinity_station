#ifndef _JCWMES_H_
#define _JCWMES_H_

#include "JCWDataDef.h"

#ifndef IN //函数传参数标识符，没有任何程序功能，仅仅标识是一个存输入型参数
#define IN
#endif

#ifndef OUT	//函数传参数标识符，没有任何程序功能，仅仅标识是一个输出型参数
#define OUT
#endif

#ifndef REF	//函数传参数标识符，没有任何程序功能，仅仅标识是引用参数，可以传值，也可以返回结果
#define REF
#endif


#ifdef __cplusplus
extern "C" {
#endif
	//获取开发库版本号。字符编码GB2312
	const char*	Jcw_GetLibVersionStr();


	//获取错误代码，对应的错误文字描述， 编码GB2312
	const char*	Jcw_GetErrorCodeDescribeStr(JCW_RET jErrCode);


	//功能：构造一个实例
	JCWHANDLE Jcw_InitInstance();

	//功能：释放一个实例
	JCW_RET Jcw_UninitInstance(JCWHANDLE h);

	//写入配置文件
	JCW_RET Jcw_SaveConfig(JCWHANDLE h, const char* strCfgFile);

	//读取一份配置
	JCW_RET Jcw_LoadConfig(JCWHANDLE h, const char* strCfgFile);

	//功能：设置设备连接地址信息
	//id			设备ID
	//strDevIP		设备的IP地址
	//uiDevPort		设备的通信端口；非特殊情况，填写0，表示使用默认端口
	JCW_RET Jcw_SetDevAddress(JCWHANDLE h, JMDEVID id, const char* strDevIP, UINT uiDevPort);



	//功能：获取已经设置的设备地址列表
	//idArray		设备ID的缓冲区
	//uiBuffNum		缓冲区的数量，数组大小；可理解为 uiBuffNum = sizeof(idArray) / sizeof(JMDEVID)
	//uiDevNumber	已设置的设备数量
	JCW_RET Jcw_GetDevIDList(JCWHANDLE h, JMDEVID* idArray, IN UINT uiBuffNum, OUT UINT& uiDevNumber);


	//功能：获取设备连接地址信息
	//id			设备ID
	//strDevIP		设备的IP地址
	//uiDevPort		设备的通信端口；非特殊情况，填写0，表示使用默认端口
	JCW_RET Jcw_GetDevAddress(JCWHANDLE h, JMDEVID id, OUT char* strDevIPBuff, OUT UINT& uiDevPort);


	//功能：获取设备的型号，序列号
	//id			设备ID
	//strModel		设备的型号; 要求内存长度 >= 64
	//strSN			设备的序号 要求内存长度 >= 64
	JCW_RET Jcw_GetDevModelSN(JCWHANDLE h, JMDEVID id, OUT char* strModel, OUT char* strSN);

	//点云回调函数
	//const void* 用户提供的参数，回调时传回
	//JCWJH&	检测几何参数结果
	//UINT		轮廓点云数组的长度
	//POINTF**	点云的空间点,二维数组，每个成员代表一导线的轮廓点
	//const int*		点的数量
	/*
	*用例
	*以下是回调函数
	void JcwProfileCallback(const void* ptag, const JCWJH& jcwjh, const UINT iProfileNum, const POINTF** pfarr, const int* pflen)
	{
		std::vector<POINTF> vecPf1, vecPf2, vecPf3, vecPf4;
		if(iProfileNum > 0)
		{
			vecPf1.assign(pfarr[0], pfarr[0] + pflen[0]);
		}
		if(iProfileNum > 1)
		{
			vecPf2.assign(pfarr[1], pfarr[1] + pflen[1]);
		}
		if(iProfileNum > 2)
		{
			vecPf3.assign(pfarr[2], pfarr[2] + pflen[2]);
		}
		if(iProfileNum > 2)
		{
			vecPf4.assign(pfarr[3], pfarr[3] + pflen[3]);
		}
	}
	*/
	typedef void(*Jcw_fnResultProfileCallBack)(const void*, const JCWJH&, const UINT, const POINTF**, const int*);
	JCW_RET Jcw_SetResultProfileCallBack(JCWHANDLE h, const void* pTag, Jcw_fnResultProfileCallBack fnCB);

	//回调函数的定义
	typedef void(*Jcw_fnJCWResultCallBack)(const void*, const JCWJH&);
	//功能：设置检测结果数据回调的接口函数指针，优先级是0； 0是最低，随数值逐步递增
	JCW_RET Jcw_SetResultCallBack(JCWHANDLE h, const void* pTag, Jcw_fnJCWResultCallBack fnCB);
	

	//回调函数的定义，修改了数据结构体为JCWD，JCWD拓展检测的信息更丰富。
	//参数1 const void* ;用户指定的指针
	//参数2 JCWDVER 数据版本
	//参数3 const void*：检测数据的指针，根据数据版本，指向不同的结构体类型；JVER_JCWJH 是JCWJH结构体的指针; JVER_JCWD 是JCWD的指针；JVER_JSON json字符串的文本指针(gb2312)
	//参数4 UINT 数据的大小
	typedef void(*Jcw_fnJCWDResultCallBack)(const void*, JCWDVER, const void*, UINT);
	//功能：设置检测结果数据回调的接口函数指针。回调的结果是JCWD，优先级是100； 0是最低，随数值逐步递增
	JCW_RET Jcw_SetJCWDCallBack(JCWHANDLE h, const void* pTag, JCWDVER jver, Jcw_fnJCWDResultCallBack fnCB);

	
	//设置检测数据，按照空间采样距离回调。需要调用 Jcw_SetMovSpeedKMH 配合使用。程序会根据程序，计算采样间隔对几何数据进行采样。
	//iCollectIntervalMM 数据的空间采样间隔，比如100mm输出一个数据; 有效范围值  0 ~ 255mm; 0 表示不按距离采样，直接输出每一条检测数据； 超过最大值则保留255mm的采样间隔
	//uiCacheLenght 数据滞后缓存的距离，适当的滞后处理，能提供检测数据的稳定性；单位mm，最大值 300,000(300m)
	JCW_RET	Jcw_SetJCWD_SpaceCollect_Param(JCWHANDLE h, uint32_t iCollectIntervalMM, uint32_t uiCacheLenghtMM);

	//功能：设置单一设备的检测数据的回调;优先级是0； 0是最低，随数值逐步递增
	JCW_RET Jcw_SetDevResultCallBack(JCWHANDLE h, JMDEVID id, const void* pTag, Jcw_fnJCWResultCallBack fnCB);

	//功能：设置单一设备的检测数据的回调。优先级是1
	JCW_RET Jcw_SetDevJCWDCallBack(JCWHANDLE h, JMDEVID id, const void* pTag, JCWDVER jver, Jcw_fnJCWDResultCallBack fnCB);


	
	//功能：弹出一条检测结果的数据，用在非回调模式中
	//程序缓存了部分数据在缓冲区中，通过此函数获取队列中的第一条数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//h 设备句柄 
	//pJcwd JCWD结构对象的指针
	//iTimeMSWait 等待的时间(ms)，0 = 不等待，-1 无限等待， >0 等待的毫秒数
	//return 如果有数据，返回JCW_OK，否则返回其他代码
	JCW_RET Jcw_PopJcwd(JCWHANDLE h, OUT JCWD* pJcwd, INT iTimeMSWait);

	//功能：弹出一条检测结果的数据，用在非回调模式中
	//程序缓存了部分数据在缓冲区中，通过此函数获取队列中的第一条数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//h 设备句柄 
	//pData JCWJH结构对象的指针
	//iTimeMSWait 等待的时间(ms)，0 = 不等待，-1 无限等待， >0 等待的毫秒数
	//return 如果有数据，返回JCW_OK，否则返回其他代码
	JCW_RET Jcw_PopResult(JCWHANDLE h, JCWJH* pData);

	//功能：弹出一条检测结果的数据，用在非回调模式中
	//程序缓存了部分数据在缓冲区中，通过此函数获取队列中的第一条数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//!!!注意:如果没有设置回调函数，此函数才会有数据返回;否则所有的结果通过回调传递到用户应用中，本函数无法查询到数据
	//h 设备句柄 
	//jver 要转换成哪种数据的类型; 目前支持 JCWD， JSON， JCWJH 三种数据类型。
	//pBuffer 数据将拷贝到此内存中
	//puiBuffSize指向数据大小的变量，传入时表达缓冲区的大小，返回后，表示数据的长度
	//iTimeMSWait 等待的时间(ms)，0 = 不等待，-1 无限等待， >0 等待的毫秒数
	//return 如果有数据，返回JCW_OK，否则返回其他代码
	JCW_RET Jcw_PopJcwResult(JCWHANDLE h, JCWDVER jver, OUT void* pBuffer, REF UINT* puiBuffSize, UINT uiTimeoutMS);

	//功能：启动数据接收工作，设置参数完毕后调用本接口
	JCW_RET Jcw_Start(JCWHANDLE h);

	//功能：停止数据接收工作
	JCW_RET Jcw_Stop(JCWHANDLE h);

	//功能：判断系统是否启动检测
	JCW_RET Jcw_IsStart(JCWHANDLE h, int& iState);


	//设置检测平台（车辆）的移动速度；最少间隔一秒内应该调用更新一次车速（不管车速有没有变化。调用密度可以很高不影响性能，程序会抽样使用）；否则系统将失去车速的有关控制功能
	//fMovSpeedKM_H 移动的速度，单位km/h；数值>=0
	//return 是否设置成功
	JCW_RET Jcw_SetMovSpeedKMH(JCWHANDLE h, float fMovSpeedKM_H);

	//设置测速的车轮编码器的脉冲计数值；
	//iPulseCount 脉冲计数值
	//dWheelDiaMeter 车轮直径，单位mm
	//iPulsePreRound 车轮每转一周的脉冲数量
	JCW_RET Jcw_SetMovPulseCount(JCWHANDLE h, INT64 iPulseCount, double dWheelDiaMeter_mm, UINT iPulsePreRound);

	
	//设定某个相机的Out1输出口的工作模式
	//h 实例句柄
	//jmid 操作的某个相机; JMID_GLOBAL 则对所有相机进行操作
	//mode 工作模式； 如果是模拟触发模式，则相机过一段时间后，自动会切换到触发模式
	//return 设置结果
	//功能：设置触发模式
	//h 实例句柄
	//iTrigerMode 触发模式
	//uiTrigerIVL 触发间隔；只有在触发、模拟触发时生效; 参数等于 0 的时候，表示使用原先配置的值
	JCW_RET Jcw_SetOutputMode(JCWHANDLE h, JMDEVID id, JCWOUTPUT iOutputMode, UINT32 uiTrigerIVL);

	//功能：获取设置的触发模式的信息
	JCW_RET Jcw_GetOutputMode(JCWHANDLE h, JMDEVID id, JCWOUTPUT& iOutputMode, UINT32& uiTrigerIVL);

	//功能：获取指定触发模式的配置信息; 主要时获取触发、模拟触发的时间间隔配置
	JCW_RET Jcw_GetOutputModeConfig(JCWHANDLE h, JMDEVID id, JCWOUTPUT iOutputMode, UINT32& uiTrigerIVL);


	//uiTimeMs_TrigerIVL 触发间隔时间（ms)；只有在触发、模拟触发时生效; 参数等于 0 的时候，表示使用原先配置的值
	//fDistMeter_TrigerIVL 触发间隔距离，车辆必须距离上次移动多少米，才会输出下一个触发
	JCW_RET Jcw_SetOutputMode1(JCWHANDLE h, JMDEVID id, JCWOUTPUT iOutputMode, UINT32 uiTimeMs_TrigerIVL, float fDistMeter_TrigerIVL);

	//功能：获取设置的触发模式的信息
	//fDistMeter_TrigerIVL 触发间隔距离，车辆必须距离上次移动多少米，才会输出下一个触发
	JCW_RET Jcw_GetOutputMode1(JCWHANDLE h, JMDEVID id, JCWOUTPUT& iOutputMode, OUT UINT32& uiTimeMs_TrigerIVL, OUT float& fDistMeter_TrigerIVL);



	//JCW_RET Jcw_GetJcwCamWorkState(JCWHANDLE h, JMDEVID id, uint64_t* pStateFlag)
	//{
	//	*pStateFlag = 0;
	//	return JCW_OK;
	//}


	//设置接触网的设计信息，提相机检测提供参考
	//此参数是状态参数，随者车辆运行一定的时间，会跑偏的
	//fHeiMin 当前位置的最小导高; 填写为0 表示无效未知
	//fHeiMax 当前位置的最大导高; 填写为0 表示无效未知
	//jtype 接触网的类型； JCXT_UNKNOW  表示未知，需要相机自行判断
	JCW_RET Jcw_SetJcwInfo(JCWHANDLE h, float fHeiMin, float fHeiMax, JCXType jtype);

	//设置设备的同步ID，为客户程序关联数据同步
	//iSyncID 用户指定的ID，在之后的所有数据中，都将带有此数值的syncid，直到重新调用更新数值
	JCW_RET Jcw_SetSyncID(JCWHANDLE h, INT64 iSyncID);

	enum
	{
		JCW_DEVSTATE_NOWORK = -1,			//未工作
		JCW_DEVSTATE_DISCONNECT = 0,		//未连接
		JCW_DEVSTATE_WORKING = 1,			//正常工作状态	//命名不改，是为了兼容以前的代码


		JCW_DEVSTATE_INFO_STOP = -1,			//未工作
		JCW_DEVSTATE_ERROR_DISCONNECT = 0,		//连接不上相机，可能是相机未启动，网络未链接，配置IP错误等
		JCW_DEVSTATE_INFO_WORKING = 1,			//正常工作状态

		JCW_DEVSTATE_ERROR_NO_FRAME = -2,		//没有图像帧，可能是没有触发，或者帧率过低

		JCW_DEVSTATE_WARN_OVEREXPOSED = 2,		//图像过曝，不适用于日光下工作的相机置于日光下致使无法检测，或者相机曝光时间过大、直视太阳等高亮物，导致无法测量
		JCW_DEVSTATE_WARN_NO_LASER = 3,			//没有找到激光线/镜头脏污、工作范围内没有物体、曝光时间过低、环境光过亮掩盖了激光等
		JCW_DEVSTATE_WARN_NO_TARGET = 4,		//没有识别到目标
	};

	//功能：返回设备的连接状态，状态值参数以上枚举变量值
	//id 指定具体的相机；如果id == JMID_GLOBAL,则评估一个整体的结果
	//iState = JCW_DEVSTATE_NOWORK 。。。
	JCW_RET Jcw_GetDevState(JCWHANDLE h, JMDEVID id, OUT int& iState);

	//功能：获取设备的测量帧率
	JCW_RET Jcw_GetDevFPS(JCWHANDLE h, JMDEVID id, OUT float& fFPS);

	//获取指定功能数据的传输频率
	JCW_RET Jcw_GetFuncFPS(JCWHANDLE h, JMDEVID jid, int imid, OUT float& fFPS);

	//功能：获取设备连接的时长，单位秒(s)
	JCW_RET Jcw_GetDevConnTime(JCWHANDLE h, JMDEVID id, OUT UINT& uiTimeSec);


	/************调试文件操作功能**************/

	//功能：创建一个保存调试包的文件，并且开始保存
	//strFilePath 保存的文件名
	JCW_RET Jcw_CreateDebugDataFile(JCWHANDLE h, const char* strFilePath);

	//功能：停止保存调试数据，并且关闭结调试包的文件
	JCW_RET Jcw_CloseDebugDataFile(JCWHANDLE h);



	/************计算功能函数**************/

	// 	进行补偿计算
	// 	h 实例句柄
	// 	fXArray 拉出值数组.计算的结果回填到数组中
	// 	fYArray 导高数组.计算的结果回填到数组中
	// 	uiDataNum 数组长度
	// 	ptCompLeft 左补偿的数据
	// 	ptCompRight 右补偿的数据
	//return 如果计算成功呢，返回JCW_OK; 
	JCW_RET Jcw_CalcBuchang(JCWHANDLE h, float* fXArray, float* fYArray, UINT uiDataNum, POINTF ptCompLeft, POINTF ptCompRight);

	//保存激光点文件
	//strfilePath 要保存的文件名,gbk编码
	JCW_RET Jcw_CreateLaserPointFile(JCWHANDLE h, const char* strFilePath);

	//保存激光点文件
	//strfilePath 要保存的文件名,gbk编码
	JCW_RET Jcw_CreateLaserPointFile_Ex(JCWHANDLE h, const char* strFilePath, float fMovSpeedKmh_Min, float fCollectIntervalMeter);


	//关闭保存激光点文件
	JCW_RET Jcw_CloseLaserPointFile(JCWHANDLE h);





	//保存激光点文件
	//strfilePath 要保存的文件名,gbk编码
	JCW_RET Jcw_CreateLaserImageFile(JCWHANDLE h, const char* strFilePath, float fMovSpeedKmh_Min, float fCollectIntervalMeter);

	//关闭保存激光点文件
	JCW_RET Jcw_CloseLaserImageFile(JCWHANDLE h);

	//把JDBG文件转换为jcwd文件
	JCW_RET Jcw_JdbgToJCWD(const char* strFileJdbg, const char* strFileJcwd);
	
	//把JDBG文件转换为csv文件
	JCW_RET Jcw_JdbgToCsv(const char* strFileJdbg, const char* strFileCSV);

	JCW_RET Jcw_JdbgToCsv_MH(const char* strFileJdbg, const char* strFileCSV);

	//将接收的数据转发到指定的网络地址，通过udp包
	JCW_RET Jcw_ForwardJCWD(JCWHANDLE h, JCWDVER jver, const char* strIP, UINT uiPort);

	//结构体转换，用于将JCWJH 对象转换为 JCWD对象
	void	Jcw_JCWJH_to_JCWD(JCWJH* pJH, OUT JCWD* pJcwd);

	JCW_RET	Jcw_JSON_to_JCWD(const char* strJson, OUT JCWD* pJcwd);

	//将JCWD的数据转换为json格式
	int		Jcw_JCWD_to_JSON(const JCWD* pJcwd, OUT char* strBuffer, UINT iBufferSize);
#ifdef __cplusplus
};
#endif

#endif
