#include "ConnectivityManagerBase.h"
#include <QDateTime> // 时间戳生成
#include <QDir>      // 目录操作
#include <QFileInfo> // 文件路径处理
#include <QDebug>    // 调试输出
#include <chrono>
_Kits::ConnectivityManagerBase::ConnectivityManagerBase(QObject *parent) : QObject(parent)
{
}

_Kits::ConnectivityManagerBase::~ConnectivityManagerBase()
{
}
