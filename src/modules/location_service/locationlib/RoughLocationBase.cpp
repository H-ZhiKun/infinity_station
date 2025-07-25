#include "RoughLocationBase.h"
// #include "kits/common/thread_pool/ConcurrentPool.h"
#include "kits/common/log/CRossLogger.h"

using namespace _Kits;
RoughLocationBase::RoughLocationBase() : LocationBaseInterface()
{
}

RoughLocationBase::~RoughLocationBase()
{
    release();
}

int RoughLocationBase::init(const YAML::Node &config)
{
    // 初始化资源
    m_refreshRate = config["refresh_rate"].as<int>();

    return 0;
}

int RoughLocationBase::start()
{
    // 开始运行
    m_runThreadFlag.store(true);
    m_runThread = std::thread(&RoughLocationBase::run, this);
    return 0;
}

void RoughLocationBase::stop()
{
    // release();
    release();
}

void RoughLocationBase::release()
{
    m_runThreadFlag.store(false);
    if (m_runThread.joinable())
    {
        m_runThread.join();
    }
    m_locationBaseInfo.reset();
}

inline double getDuration(std::chrono::steady_clock::time_point &startTime, std::chrono::steady_clock::time_point &endTime)
{
    endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    startTime = endTime;
    return static_cast<double>(duration.count());
}

void RoughLocationBase::run()
{
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point endTime = startTime;
    while (m_runThreadFlag)
    {
        auto durationInMilliseconds = getDuration(startTime, endTime);
        m_locationBaseInfo.UpdateOnce(durationInMilliseconds * 0.001f);
        std::this_thread::sleep_for(std::chrono::milliseconds(m_refreshRate));
    }
}

void RoughLocationBase::setMoveDis(double dis)
{
    m_moveDis = dis;
}
void RoughLocationBase::adjustData(double dis, double speed)
{
    if (dis >= 0)
    {
        // m_moveDis=dis;
        dis = dis * 1000.0f;
        m_locationBaseInfo.setDistance(dis);
    }

    if (speed >= 0)
    {
        m_locationBaseInfo.setSpeed(speed * 1000.0f / 3600.0f);
    }
}

void RoughLocationBase::updataData(double &dis, double &speed)
{
    // 发送数据
    dis = m_locationBaseInfo.getDistance() / 1000.0f;
    speed = m_locationBaseInfo.getSpeed() * 3600.0f / 1000.0f;
}

double RoughLocationBase::getSpeed()
{
    return m_locationBaseInfo.getSpeed() * 3600.0f / 1000.0f;
}

double RoughLocationBase::getDistance()
{
    return m_locationBaseInfo.getDistance() / 1000.0f;
}

double RoughLocationBase::getMoveDis()
{
    return m_moveDis;
}