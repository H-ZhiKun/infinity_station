#include "LocationBaseInfo.h"

double LocationBaseInfo::getDistance()
{
    return m_distance;
}
double LocationBaseInfo::getSpeed()
{
    return m_speed;
}
void LocationBaseInfo::UpdateOnce(const double &stime)
{
    // 更新距离
    m_distance = m_distance + m_speed * stime;
}
void LocationBaseInfo::setDistance(const double &distance)
{
    m_distance = distance;
}

void LocationBaseInfo::setSpeed(const double &speed)
{
    m_speed = speed;
}
void LocationBaseInfo::reset()
{
    m_distance = 0.0f;
    m_speed = 0.0f;
}