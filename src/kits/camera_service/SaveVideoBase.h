
#pragma once
#include <QObject>
#include <QImage>
#include <QVariant>
#include "encodevideo.h"

namespace _Kits
{

class SaveVideoBase:public QObject
{

    Q_OBJECT

  public:
  SaveVideoBase(QObject *parent = nullptr){}
    virtual ~SaveVideoBase(){}
    virtual int initSaveVideo(const QVariant &var) = 0;
    virtual int setSubtitle(int x_StartPos, int y_StartPos, int offset_dis) = 0;

public slots:
    virtual void pushImage(const QImage& img){return;}
    virtual int startRecord(const QVariant &var, std::string& filepath){return 0;};
    virtual void stopRecord(){return;}
};
}