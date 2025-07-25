#include "service/AppFramework.h"
#include <QSharedMemory>
#include <qdebug>

int main(int argc, char *argv[])
{
    // 捕获异常 一般情况不开启
    // _Kits::Utils::initCrashHandler();
    QSharedMemory sharedMemory("togee_infinity_station");
    // QSharedMemory sharedMemory("IRtemp");
    if (sharedMemory.attach())
    {
        qDebug() << "Another instance is already running!";
        return 1; // 正常退出，返回非零错误码
    }
    if (!sharedMemory.create(1))
    {
        qDebug() << "Failed to create shared memory!";
        return 1; // 正常退出，返回非零错误码
    }
    return _Service::_AppFramework::_InnerApp().run(argc, argv);
}
