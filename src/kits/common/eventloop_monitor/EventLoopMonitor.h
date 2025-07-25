#pragma once
#include <QElapsedTimer>
#include <QObject>
#include <QTimer>

namespace _Kits
{

    class EventLoopMonitor : public QObject
    {
        Q_OBJECT
      public:
        explicit EventLoopMonitor(QObject *parent = nullptr);

        void init();

      private slots:
        void onAboutToBlock();
        void onAwake();
        void reportStatistics();

      private:
        void emergencyHandler();
        void printStackTraceIfPossible();

      private:
        QElapsedTimer m_timer;                 // 用于测量 awake 和 block 的持续时间
        QTimer m_reportTimer;                  // 每隔一段时间（默认1分钟）触发统计报告
        int m_awakeCount = 0;                  // 本周期内唤醒次数统计
        int m_blockCount = 0;                  // 本周期内休眠次数统计
        qint64 m_activeTime = 0;               // 本周期内总活跃（awake）时间，单位：毫秒
        qint64 m_blockTime = 0;                // 本周期内总休眠（block）时间，单位：毫秒
        qint64 m_maxAwakeDuration = 0;         // 单次最大活跃时长（最长的一次awake）
        qint64 m_maxBlockDuration = 0;         // 单次最大休眠时长（最长的一次block）
        int m_heavyAwakeCount = 0;             // 活跃时长超过重负载阈值的 awake 次数
        int m_heavyBlockCount = 0;             // 休眠时长超过重负载阈值的 block 次数
        qint64 m_heavyDurationThreshold = 200; // 判定为“重负载”的时间阈值，默认200毫秒
        int m_emergencyCount = 0;              // 紧急处理被触发的次数统计
    };

    /*================ EventLoopMonitor 健康评价指南 ================

   本模块通过统计 event loop 的运行状态，评估应用负载和健康程度。
   核心参考指标及健康评价标准如下：

   【1】Awake count / Block (Sleep) count 比例（awakeBlockRatio）
       - 正常：约 1:1
       - 异常：awakeBlockRatio > 3, 3是我拍脑壳估计的，实际需要大量的测试报告验证！
         - 说明事件循环被异常频繁地唤醒，可能由于高频定时器、事件风暴或循环信号等问题。
         - 达到异常阈值时，将触发紧急处理（emergencyHandler）。

   【2】总活跃时间（m_activeTime）vs 总休眠时间（m_blockTime）
       - 正常：m_blockTime 远大于 m_activeTime
         - Event loop 大部分时间处于休眠状态，表示负载适中或偏轻，系统健康。
       - 注意：m_activeTime 接近或大于 m_blockTime
         - 持续处理大量事件，CPU负载可能偏高，需要关注业务量或处理逻辑是否合理。

   【3】最大单次活跃时长（m_maxAwakeDuration）
       - 正常：单次活跃时间短（通常个位数到几十毫秒）。
       - 异常：单次活跃时间超过 heavyDurationThreshold（默认200ms）， 200ms意味着帧率只有5帧了。
         - 可能存在同步IO、长计算或未拆分的耗时任务，阻塞了 Event loop。

   【4】最大单次休眠时长（m_maxBlockDuration）
       - 正常：单次休眠时长随应用设计不同，数十毫秒到数秒皆可能合理。
       - 注意：
         - 如果休眠异常长（如几十秒以上），可能是外部事件源枯竭或程序运行错误。

   【5】重大载荷次数（m_heavyAwakeCount / m_heavyBlockCount）
       - heavyAwakeCount（长时间活跃次数）：
         - 频繁出现时，表明事件处理存在性能瓶颈，应分析关键路径。
       - heavyBlockCount（长时间休眠次数）：
         - 多数情况下无需担心，除非伴随性能问题，应确认事件源活跃性是否合理。

   【6】紧急处理次数（m_emergencyCount）
       - 紧急处理是为了防止 event loop 因异常唤醒频率而堆积未处理事件。
       - 正常：应接近于 0。
       - 若频繁触发，表明系统存在事件风暴、死循环或异常高负载问题，应立即排查。

   ==================================================================*/

} // namespace _Kits
