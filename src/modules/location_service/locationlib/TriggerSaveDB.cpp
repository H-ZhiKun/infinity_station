#include "TriggerSaveDB.h"
#include <iostream>
#include "kits/common/log/CRossLogger.h"
TriggerSaveDB::TriggerSaveDB(double distance)
{

    if (std::fabs(distance) < 100.0)
    {
        m_intervalDis = std::fabs(distance);
    }
    else
    {
        m_intervalDis = 25.0;
    }
}

TriggerSaveDB::~TriggerSaveDB() noexcept
{
}

bool TriggerSaveDB::isTrigger()
{

    if (std::fabs(m_curDis.load() - m_preDis.load()) > m_intervalDis)
    {
        m_preDis.store(m_curDis.load());
        return true;
    }
    return false;
}

void TriggerSaveDB::setcurDis(double dis)
{
    if (dis > 0)
        m_curDis.store(dis);
}