#pragma once
#include<iostream>

namespace _Kits
{
    enum LINE_TYPE			
    {
        //接触网线路类型,请勿修改声明顺序，绑定了几何参数程序的控件ID
        LT_UNKNOW	= -1,		    //未知
        LT_HARD		= 0,			//0x00刚性   
        LT_SOFTONE	= 1,			//0x01柔性单支  
        LT_SOFTTWO	= 2,			//0x02柔性双支
        LT_INTERIM	= 3,			//0x03过渡(刚柔)
        LT_INTERIMR	= 4,		    //0x04柔刚过渡 //
        LT_USANGUI  = 5,			//0x05上接触三轨
        LT_DSANGUI	= 6,			//0x06下接触三轨
        LT_RESERVED2= 7,	        //0x07保留2
        LT_PENGZHANGYUANJIAN = 8,	//膨胀元件
    };
    
    enum POSTYPE
    {
        PT_NONE = 0,	//无检测结果
        PT_LINE = 1,    //普通导线
        PT_DIST = 2,	//跨距定位
        PT_ZIG = 3, 	//拉出值定位
        PT_FIMAGE = 4, //前图像定位（只有一个图像定位相机时使用前图像定位）
        PT_BIMAGE = 5, //后图像定位
        PT_HAND = 6,	//手动定位
        PT_LIGHT = 7,	//光电定位
        PT_OTHER = 8, //其它定位信号
        PT_DIAOXUAN = 9, //吊弦
        PT_MAODUAN_XIACA = 10, //锚段关节(锚段或线岔换工作支位置)上的图像定位
        PT_POLEMETER = 11,//公里标
        PT_OCR = 12,//OCR定位
        PT_GPS = 13,
        PT_RFID = 14,
        PT_GYK = 15,
        PT_DB = 16,
        PT_OCR_GYK = 17,
        PT_OCR_GPS = 18,
        PT_LEAVE_MAODUAN_CHECK = 19,  //出锚 进行了校准位置
        PT_ZHONG_XIN_ZHU = 20, 
        PT_LIGHT_BACK = 21,
        PT_LIGHT_FORWARD = 22,
        PT_LEAVE_MAODUAN_NOCHECK = 23,  //出锚 没有校准
        PT_FENDUAN_JUEYUANQI = 24 //分段绝缘器
    };
    
    struct BCData
    {
        float fHeiLeft;
        float fZigLeft;
        float fHeiRight;
        float fZigRight;
        int iStatus;
        BCData()
        {
            memset(this, 0x00, sizeof(*this));
        }
    };
    
    
    struct  CMoHaoInfo2
    {
        char	cVideoName[50];
        int		iFrame;
        int		iValueCount;
        float	fMohaoValue[4];//磨耗值
        float	fRadis[4];		//导线半径
        float	fLineWid[4];	//被磨面宽
        float	fRemainHei[4];		//残存高度
        float	fPianMo[4];		//偏磨值
        int		iTypeExe;	//分析获取磨耗值的方法 1导线圆拟合、2找汇流排交叉点，再延伸、3磨得多的直接找底面直线长度
        float	fMoHaoRate; //磨耗角度
        CMoHaoInfo2()
        {
            memset(this, 0, sizeof(CMoHaoInfo2));
        }
    };
    
    struct  CGWJJCDataBase
    {
        public:
        char	strStationName[50]; //区间名
        char	strMaoduanName[50]; //锚段名
        char	strPoleName[50];    //杆号名
    
        int		iDataBaseID;            //当前数据(数据库编号ID)	
        double  dTimeStamp;				//更新时间标识：时间戳
        int		uiWheelPulse;			//车轮脉冲
    
    
        char16_t	bLineDir;				//true:上行 ；flase :下行
        char16_t	bTrainMoveDir;		    //运动方向 
        char16_t	ucLineType;				//线路类型
        char16_t	ucPoleLocateType;		//定位方式
    
        float	fPerPoleDistance;       //数据库跨距(米)
        float	fSpeed;					//当前速度(km/h)
        double  dTrainMoveDistance;		//列车行车里程(从检测时开始累计)-(KM).
        double  dPoleKiloMeter;			//线路数据库公里标(km)
        float	fPoleMeter;				//当前跨距内行车距离(米)*/
    
        
        float	fZigs[4];				//检测拉出值数组
        float	fHeis[4];				//检测导值数组
    
        float	fZigsBC[4];				//补偿后的拉出值
        float	fHeisBC[4];				//补偿后的导高
    
        float	fCxLeft;				//左水平补偿
        float	fCyLeft;				//左高度补偿
        float	fCxRight;				//右水平补偿 
        float	fCyRight;				//右高度补偿
    
        int		iMonitorFrameNum;		//监控桢
    
        float   fTemperature;			//温度
        float   fHumidity;				//湿度
    
        time_t		tTime;
    };
}