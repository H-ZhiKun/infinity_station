#include "TestController.h"
#include "kits/common/log/CRossLogger.h"
#include "service/AppFramework.h"
#include <qglobal.h>
#include <qlogging.h>
#include <opencv2/core/cuda.hpp>
#include <iostream>
#include <qvariant.h>

using namespace _Controllers;
using namespace _Kits;
using namespace _Service;
void TestTaskController::testDeviceInfo(TIS_Info::QmlCommunication::ForQmlSignals, const QVariant &)
{
    // [Unit Test0] 信号传递验证: 无耦合转发信号及数据
    _Kits::LogError("test");
    // cv::cuda::printCudaDeviceInfo(cv::cuda::getDevice());
    // qDebug() << "OpenCV CUDA enabled: " << cv::cuda::getCudaEnabledDeviceCount();
}

void TestQmlController::testPageChanged(const QVariant &data)
{
    LogInfo("onPageChanged: {}", data.toString().toStdString());
    return;
}
