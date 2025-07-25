#include "YYJiHe.h"
#include "../JiHeStruct.h"
#include <math.h>


namespace _Kits
{
    std::mutex _Kits::YYJiHe::m_mutex;
    std::vector<JCWD> _Kits::YYJiHe::m_vctJcwd;
    YYJiHe::YYJiHe()
    {
        m_pJiHe = Jcw_InitInstance();
    }

    YYJiHe::~YYJiHe()
    {
        Jcw_UninitInstance(m_pJiHe);
    }

    bool YYJiHe::Open(const std::string &strJcw)
    {
        bool bRet = true;

        // std::string strJcw = ".\\config\\location\\jcwconfig.ini";
        if (Jcw_LoadConfig(m_pJiHe, strJcw.c_str()) != JCW_OK)
        {
            // qDebug()<<"Jcw_LoadConfig err. ini path is "<<strJcw.data();
            bRet = false;
        }

        if (Jcw_SetJCWDCallBack(m_pJiHe, this, JVER_JCWD, _Kits::YYJiHe::fnResultCallback) != JCW_OK)
        {
            // qDebug()<<"Jcw_SetJCWDCallBack err.";
        }
        if (Jcw_Start(m_pJiHe) != JCW_OK)
        {
            // qDebug()<<"Jcw_Start err.";
        }

        return bRet;
    }

    bool YYJiHe::Close()
    {
        bool bRet = true;
        if (Jcw_Stop(m_pJiHe) != JCW_OK)
            bRet = false;
        return bRet;
    }

    bool YYJiHe::Refresh(CGWJJCDataBase &jiHeData)
    {
        bool bRe = false;
        m_mutex.lock();
        auto jcwd = m_vctJcwd.front();
        m_vctJcwd.erase(m_vctJcwd.begin());
        m_mutex.unlock();

        int len_num = jcwd.uiLineNum>4?4:jcwd.uiLineNum;
        for (size_t i = 0; i < len_num; i++)
        {
            jiHeData.fZigs[i] = jcwd.jcx[i].pntLinePos.x;
            jiHeData.fHeis[i] = jcwd.jcx[i].pntLinePos.y;
            jiHeData.fZigsBC[i] = jcwd.jcx[i].pntCompPos.x;
            jiHeData.fHeisBC[i] = jcwd.jcx[i].pntCompPos.y;

            // mohao
            m_moHaoData.fMohaoValue[i] = jcwd.jcx[i].fMohaoH;
            m_moHaoData.fRadis[i] = jcwd.jcx[i].fLineRidus;
            m_moHaoData.fLineWid[i] = jcwd.jcx[i].fMhBotLen;

            JCWPosi posType = jcwd.GetPosiflag();
            if (posType == JCXP_POLE) // 支柱
            {
                jiHeData.ucPoleLocateType = PT_FIMAGE;
            }
            else if (posType == JCXP_DROPPER) // 吊弦
            {
                jiHeData.ucPoleLocateType = PT_DIAOXUAN;
            }
            else
            {
                jiHeData.ucPoleLocateType = PT_LINE;
            }

            // 补偿可能从udp发送过来
            jiHeData.fCxLeft = jcwd.pntCompLeft.x;
            jiHeData.fCyLeft = jcwd.pntCompLeft.y;
            jiHeData.fCxRight = jcwd.pntCompRight.x;
            jiHeData.fCyRight = jcwd.pntCompRight.y;
        }

        return bRe;
    }

    void YYJiHe::fnResultCallback(const void *pTag, JCWDVER jver, const void *data, UINT iDataSize)
    {
        YYJiHe *pMine = (YYJiHe *)pTag;
        if (pMine)
        {
            pMine->fnResultCallback(jver, data, iDataSize);
        }
    }

    void YYJiHe::fnResultCallback(JCWDVER jver, const void *data, UINT iDataSize)
    {
        static int jcwjhMaxLen = 50;

        // 后面改成队列
        if (m_vctJcwd.size() > jcwjhMaxLen)
        {
            m_vctJcwd.erase(m_vctJcwd.begin());
        }
        JCWD jcwjh = *(JCWD *)data;
        std::lock_guard<std::mutex> lock(m_mutex);
        m_vctJcwd.push_back(std::move(jcwjh));
    }
} // namespace _Kits
