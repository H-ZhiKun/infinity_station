#include "EventLoopMonitor.h"
#include "kits/common/log/CRossLogger.h"
#include <QAbstractEventDispatcher>
#include <QCoreApplication>
#include <QThread>
#ifdef Q_OS_UNIX
#include <execinfo.h>
#endif

namespace _Kits
{

    EventLoopMonitor::EventLoopMonitor(QObject *parent) : QObject(parent)
    {
    }

    void EventLoopMonitor::init()
    {
        QAbstractEventDispatcher *dispatcher = QAbstractEventDispatcher::instance(QThread::currentThread());

        if (dispatcher)
        {
            connect(dispatcher, &QAbstractEventDispatcher::aboutToBlock, this, &EventLoopMonitor::onAboutToBlock);
            connect(dispatcher, &QAbstractEventDispatcher::awake, this, &EventLoopMonitor::onAwake);
            LogInfo("[EventLoopMonitor] Event loop monitoring started");
        }
        else
        {
            LogError("[EventLoopMonitor] Failed to find event dispatcher!");
        }

        m_timer.start();
        m_reportTimer.setInterval(60000); // 每分钟统计一次
        connect(&m_reportTimer, &QTimer::timeout, this, &EventLoopMonitor::reportStatistics);
        m_reportTimer.start();
    }

    void EventLoopMonitor::emergencyHandler()
    {
        LogWarn("[EventLoopMonitor] Emergency handler triggered");
        // 触发紧急处理流程

        const int maxProcessEventsRounds = 10;
        int round = 0;
        QElapsedTimer localTimer;
        localTimer.start();

        while (round < maxProcessEventsRounds)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
            ++round;
        }
        // 紧急处理结束，处理轮次：{}
        LogWarn("[EventLoopMonitor] Emergency handling completed, rounds processed: {}", round);
        ++m_emergencyCount;
    }

    void EventLoopMonitor::onAboutToBlock()
    {
        qint64 elapsed = m_timer.elapsed();
        m_activeTime += elapsed;
        ++m_blockCount;

        if (elapsed > m_maxAwakeDuration)
            m_maxAwakeDuration = elapsed;
        if (elapsed > m_heavyDurationThreshold)
            ++m_heavyAwakeCount;

        m_timer.restart();
    }

    void EventLoopMonitor::onAwake()
    {
        qint64 elapsed = m_timer.elapsed();
        m_blockTime += elapsed;
        ++m_awakeCount;

        if (elapsed > m_maxBlockDuration)
            m_maxBlockDuration = elapsed;
        if (elapsed > m_heavyDurationThreshold)
            ++m_heavyBlockCount;

        m_timer.restart();
    }

    void EventLoopMonitor::reportStatistics()
    {

        int totalEvents = m_awakeCount + m_blockCount;
        bool needEmergency = false;

        LogInfo("[EventLoopMonitor] ===== 1-minute Statistics Report =====");
        LogInfo("[EventLoopMonitor] Awake count: {}", m_awakeCount);                                              // 唤醒次数
        LogInfo("[EventLoopMonitor] Block count: {}", m_blockCount);                                              // 睡眠次数
        LogInfo("[EventLoopMonitor] Total active time: {} ms", m_activeTime);                                     // 总活跃时间
        LogInfo("[EventLoopMonitor] Total block time: {} ms", m_blockTime);                                       // 总睡眠时间
        LogInfo("[EventLoopMonitor] Max single awake duration: {} ms", m_maxAwakeDuration);                       // 最大单次活跃时长
        LogInfo("[EventLoopMonitor] Max single block duration: {} ms", m_maxBlockDuration);                       // 最大单次睡眠时长
        LogInfo("[EventLoopMonitor] Heavy awake count (>{}ms): {}", m_heavyDurationThreshold, m_heavyAwakeCount); // 活跃超时次数
        LogInfo("[EventLoopMonitor] Heavy block count (>{}ms): {}", m_heavyDurationThreshold, m_heavyBlockCount); // 睡眠超时次数
        LogInfo("[EventLoopMonitor] Emergency count: {}", m_emergencyCount);                                      // 紧急处理次数
        LogInfo("[EventLoopMonitor] Total events: {}", totalEvents);                                              // 总事件数

        constexpr double awakeBlockRatioThreshold = 3.0;
        constexpr double activeTimeRatioThreshold = 0.7; // 活跃时间占比超过70%
        constexpr int heavyAwakeThreshold = 10;          // 单位时间内超时活跃次数过多
        constexpr int heavyBlockThreshold = 10;          // 单位时间内超时休眠次数

        // ==== 触发紧急处理条件 ==== 在头文件中查看评价指南
        double awakeBlockRatio = (m_blockCount == 0) ? m_awakeCount : (double)m_awakeCount / (double)m_blockCount;
        if (awakeBlockRatio > awakeBlockRatioThreshold)
        {
            LogError("[EventLoopMonitor] Awake/Block ratio: {:.2f}", awakeBlockRatio); // 唤醒/休眠 比例
            needEmergency = true;
        }

        double totalDuration = m_activeTime + m_blockTime;
        double activeRatio = (totalDuration > 0) ? (double)m_activeTime / totalDuration : 0.0;
        if (activeRatio > activeTimeRatioThreshold)
        {
            LogError("[EventLoopMonitor] Active time ratio too high! Active time is {:.2f}% of total time", activeRatio * 100.0);
            needEmergency = true;
        }

        if (m_heavyAwakeCount >= heavyAwakeThreshold)
        {
            LogError("[EventLoopMonitor] Too many heavy awake events! Heavy awake count: {}", m_heavyAwakeCount);
            needEmergency = true;
        }

        if (m_heavyBlockCount >= heavyBlockThreshold)
        {
            // 一般可观测，不必强制紧急处理
            LogError("[EventLoopMonitor] Too many heavy sleep events! Heavy sleep count: {}", m_heavyBlockCount);
        }

        if (needEmergency)
        {
            // emergencyHandler();
            // printStackTraceIfPossible();
        }

        // 重置周期统计
        m_awakeCount = 0;
        m_blockCount = 0;
        m_activeTime = 0;
        m_blockTime = 0;
        m_maxAwakeDuration = 0;
        m_maxBlockDuration = 0;
        m_heavyAwakeCount = 0;
        m_heavyBlockCount = 0;
    }

    void EventLoopMonitor::printStackTraceIfPossible()
    {
#ifdef Q_OS_UNIX
        void *array[20];
        int size = backtrace(array, 20);
        char **strings = backtrace_symbols(array, size);

        LogWarn("[EventLoopMonitor] ===== Stack Trace ====="); // 当前堆栈回溯
        for (int i = 0; i < size; ++i)
        {
            LogWarn("{}", strings[i]);
        }
        free(strings);
#else
        LogWarn("[EventLoopMonitor] Stack trace not supported on this platform"); // 当前平台不支持堆栈回溯
#endif
    }

} // namespace _Kits