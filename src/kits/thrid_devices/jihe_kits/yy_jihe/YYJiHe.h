#pragma once

//#include"JCWDataDef.h"
#include"./jcw/JCWDataDef.h"
#include"./jcw/JCWMes.h"
#include"../JiHeStruct.h"
#include<mutex>
#include<vector>
#if defined(_WIN32) && (defined (_M_IX86) || defined (__i386__))
#define JHAPI2CALL __stdcall
#else
#define JHAPI2CALL
#endif

namespace _Kits
{
	class YYJiHe 
	{
	public:
		YYJiHe();
		virtual  ~YYJiHe();

		bool Open(const std::string &str);/*打开*/
		bool Close();/*关闭*/
        bool Refresh(CGWJJCDataBase &jiHeData);

     private:
    		static  void JHAPI2CALL fnResultCallback(const void* pTag, JCWDVER jver, const void* data, UINT iDataSize);
    		static  void JHAPI2CALL fnResultCallback(JCWDVER jver, const void* data, UINT iDataSize);
     private:
			JCWHANDLE m_pJiHe;
        	CGWJJCDataBase m_jiHeData;
        	CMoHaoInfo2 m_moHaoData;

        	static std::mutex m_mutex;
        	static std::vector<JCWD> m_vctJcwd;
	};

}