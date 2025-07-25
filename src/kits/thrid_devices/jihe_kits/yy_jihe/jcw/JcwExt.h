#pragma once
#include "JCWDataDef.h"


//创建一个实例
HANDLE CreateJcwExtInst();


//销毁一个实例
void DestoryJcwExtInst(HANDLE hExtInst);

//几何检测数据按照导线分组排序
//排序后的结果，也存入pJH中
//结果数据需要去检查每个数据(jcx）判断他们是否存在有效值
int Jcwjh_GroupSort(HANDLE hExtInst, JCWJH* pJH);


//几何参数按线排序
//hExtInst 拓展对象句柄；如果只有一个地方用此函数，传null就可以
//pJH 原先按导高排序的数据
//jcwRet 排序后的结果
//return JCW_OK
int Jcwjh_SortJcxGroup(HANDLE hExtInst, const JCWJH* pJH, OUT JCWJH& jcwRet);