#include "HikBase.h"

_Kits::HikBase::HikBase(QObject *parent)
    : QObject(parent), m_lUserID(-1), m_lRealPlayHandle(-1), m_lPort(-1), m_ImageBufferPools(nullptr), mi_imageWidth(0), mi_imageHeight(0),
      mb_isStop(false)
{
}

_Kits::HikBase::~HikBase()
{
}

void _Kits::HikBase::onWriteSubTitle(std::vector<QString> subTitle)
{
}