#pragma once
#include <QObject>

class DetectCtrl:public QObject
{
    Q_OBJECT
private:
    /* data */
public:
    DetectCtrl(/* args */);
    ~DetectCtrl();

    int init();
    int dectect();
};

