#include "kits/thrid_devices/arc_kits/dwcarcuv/dwcarcuv.h"
#include "kits/common/database/orm/TableStructs.h"
#include "kits/common/log/CRossLogger.h"
using namespace _Kits;
using namespace _Kits::_Orm;

namespace _Kits
{

    DWCArcUV::DWCArcUV(QObject *parent)
        : QObject(parent), m_pulseCount(0), m_totalPulseCount(0),
          m_lastVoltage(0.0), m_arcThreshold(0.0)
    {
    }

    void DWCArcUV::setArcThreshold(double threshold)
    {
        m_arcThreshold = threshold;
    }

    void DWCArcUV::setDeviceIndex(const std::string &index) // 设置设备编号
    {
        devIndex = index;
    } 

    void DWCArcUV::OnDWCVoltageReceived(float voltage)
    {
        // 发送电压数据信号
        emit voltageReceived(voltage);

        // 检查是否发生燃弧
        bool isArc = checkArcOccurrence(voltage);

        // 更新统计信息
        updateArcStatistics(isArc);

        // 更新上一次电压值
        m_lastVoltage = voltage;
    }
    bool DWCArcUV::checkArcOccurrence(double voltage)
    {
        return voltage > m_arcThreshold;
    }

    void DWCArcUV::updateArcStatistics(bool isArc) 
    {
        // 使用成员变量替代静态变量，保证线程安全和对象独立性
        struct ArcEventContext {
            QDateTime startTime;
            bool inProgress = false;
            int arcCount = 0;
            int pulseCount = 0;
            double lastVoltage = std::numeric_limits<double>::quiet_NaN(); // 使用NaN表示未初始化
            int eventId = 0;
        };
        
        static thread_local ArcEventContext ctx; // 线程局部存储
        
        // 初始化电压值
        if (std::isnan(ctx.lastVoltage)) {
            ctx.lastVoltage = m_lastVoltage;
            return; // 第一次调用只初始化
        }

        // 计算电压变化是否构成脉冲
        const double voltageDelta = std::abs(m_lastVoltage - ctx.lastVoltage);
        const bool isPulse = voltageDelta > (m_arcThreshold * 0.5);
        ctx.lastVoltage = m_lastVoltage;

        // 电弧事件处理
        if (isArc) {
            if (!ctx.inProgress) {
                // 开始新事件
                ctx.startTime = QDateTime::currentDateTime();
                ctx.inProgress = true;
                ctx.arcCount = 1;
                ctx.pulseCount = isPulse ? 1 : 0;
            } else {
                // 持续事件
                ctx.arcCount++;
                ctx.pulseCount += isPulse ? 1 : 0;
            }
        } 
        else if (ctx.inProgress) {
            // 脉冲计数（即使当前不是电弧）
            ctx.pulseCount += isPulse ? 1 : 0;

            // 检查事件是否结束（使用可配置的超时时间）
            constexpr int kArcTimeoutMs = 100; // 可改为成员变量
            if (ctx.startTime.msecsTo(QDateTime::currentDateTime()) > kArcTimeoutMs) {
                // 准备事件数据
                TIS_Info::arcData arc_data;
                arc_data.cameraName = m_cameraName; // 直接使用std::string
                arc_data.arcId = getArcID(); // 前置递增保证ID唯一
                arc_data.arccount = ctx.arcCount;
                arc_data.arctime = ctx.startTime.msecsTo(QDateTime::currentDateTime());
                arc_data.arcpulse = ctx.pulseCount;
                arc_data.occurtime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss").toStdString();
                arc_data.fileName = ""; 
                arc_data.arcDeviceIndex = devIndex;


                // 发送事件数据
                emit sendArcStatistics(arc_data);
                WriteBatchToVector(arc_data);

                WriteDataToDatabase(); // 写入数据库

                // 重置上下文
                ctx.inProgress = false;
                ctx.arcCount = 0;
                ctx.pulseCount = 0;

                LogInfo("Arc event recorded: ID={}, Duration={}ms, Count={}, Pulses={}",
                    arc_data.arcId, arc_data.arctime, arc_data.arccount, arc_data.arcpulse);
            }
        }
    }
    void DWCArcUV::WriteBatchToVector(const TIS_Info::arcData& data)
    {

        std::lock_guard<std::mutex> lock(m_mutex);

        _Kits::_Orm::arc_data sql_arc;

        sql_arc.created_time = QString::fromStdString(data.occurtime);              // 创建时间
        sql_arc.updated_time = QString::fromStdString(data.occurtime);        // 更新时间   
        sql_arc.arc_count = data.arccount;                                          // 燃弧总数
        sql_arc.arc_time = data.arctime;                                              // 燃弧持续时间              
        sql_arc.arc_pulse = data.arcpulse;                                            // 燃弧脉冲个数
        sql_arc.arcvideo_path = data.fileName; // 视频路径
        sql_arc.camera_name = QString::fromStdString(data.cameraName);       // 相机名称
        sql_arc.arc_device_index = QString::fromStdString(data.arcDeviceIndex); // 燃弧传感器编号

        m_arc_Vector.push_back(std::move(sql_arc));
    }

    void DWCArcUV::WriteDataToDatabase()
    {
        // 这里实现将 data 写入数据库的逻辑
        _Kits::ConcurrentPool::runTask([this]() {
            std::lock_guard<std::mutex> lock(m_mutex);

            // 写入准备就绪的缓冲区
            if (!m_arc_Vector.empty())
            {
                try
                {
                    auto result = _Kits::SqlInsert<arc_data>().insert(m_arc_Vector).exec();

                    if (result)
                    {
                        LogInfo("Successfully inserted {} arc records into database", m_arc_Vector.size());
                    }
                    else
                    {
                        LogError("Failed to insert arc records into database");
                    }

                    m_arc_Vector.clear();
                }
                catch (const std::exception &e)
                {
                    LogError("Database insertion error: {}", e.what());
                }
            }
        });
    }

        // 这部分考虑一下有没有必要录像拼接路径
    void DWCArcUV::OnVideoPathRecv(const TIS_Info::arcData &arc_data)
    {
        try
        {
            uint16_t id = arc_data.id;
            QString videoPath = arc_data.fileName;
            qDebug() << "Received video path data:" << videoPath;
            qDebug() << "ID:" << id << ", Video Path:" << videoPath;
            auto update = SqlUpdate<_Kits::_Orm::arc_data>()
                .set("arcvideo_path", videoPath)
                .where("id", _Kits::OperatorComparison::Equal, id)
                .exec();
            if (!update)
            {
                LogError("Failed to update video path data");
            }

        }
        catch (const std::exception &e)
        {
            LogError("Error processing video path data: {}", e.what());
        }
    }

    int DWCArcUV::getArcID()
    {
        auto select = SqlSelect<_Kits::_Orm::arc_data>()
            .select(QStringList())
            .orderBy("id")
            .paginate(1, 1);

        if (!select.exec())
        {
            return -1;
        }

        auto res = select.getResults();

        if (res.empty())
        {
            return -1;
        }

        return res[0].id;
    }

    void DWCArcUV::setCameraName(const std::string &cameraName) 
    {
        m_cameraName = cameraName;
    }

} // namespace _Kits