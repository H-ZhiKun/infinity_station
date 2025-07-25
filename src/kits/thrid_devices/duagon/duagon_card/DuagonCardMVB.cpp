#include "DuagonCardMVB.h"
#include "kits/thrid_devices/duagon/mvb_tools/ByteArrayTools.h"
#include "kits/common/log/CRossLogger.h"
#include "tis_global/Struct.h"
#include <chrono>
#include <mutex>
#include <vector>

extern "C"
{
    // 头文件中涉及宏定义等预设条件，不能修改包含顺序
    /* ==========================================================================
     *
     *  Project specific Definitions used for Conditional Compiling
     *
     * ==========================================================================
     */
#ifdef TCN_PRJ
#include <tcn_prj.h>
#endif

    /* ==========================================================================
     *
     *  Include Files
     *
     * ==========================================================================
     */

    /* --------------------------------------------------------------------------
     *  TCN Software Architecture - common
     * --------------------------------------------------------------------------
     */
#include <tcn_def.h>

    /* --------------------------------------------------------------------------
     *  TCN Software Architecture - supervision
     * --------------------------------------------------------------------------
     */
#include <tcn_sv.h>
#include <tcn_sv_m.h>

    /* --------------------------------------------------------------------------
     *  TCN Software Architecture - process data
     * --------------------------------------------------------------------------
     */
#include <tcn_pd.h>

    /* --------------------------------------------------------------------------
     *  TCN Software Architecture - application layer
     * --------------------------------------------------------------------------
     */
#include <tcn_as.h>
    // 保持顺序
#include <tcn_ap.h>

    /* --------------------------------------------------------------------------
     *  TCN Software Architecture - extra (configuration manager)
     * --------------------------------------------------------------------------
     */
#include <tcn_cm.h>

    /* --------------------------------------------------------------------------
     *  Miscellaneous
     * --------------------------------------------------------------------------
     */
#include "dg_hdio.h"
}

using namespace _Kits;
DuagonCardMVB::DuagonCardMVB()
{
}
DuagonCardMVB::~DuagonCardMVB()
{
    m_bHolder = false;
    if (m_thProcess.joinable())
        m_thProcess.join();
}

bool DuagonCardMVB::isLittleEndian()
{
    int32_t num = 1;
    char bytes[sizeof(num)];
    std::memcpy(bytes, &num, sizeof(num)); // 安全拷贝内存
    return bytes[0] == 1;
}

bool DuagonCardMVB::init(const std::vector<TIS_Info::DuagonCardData> &initData)
{
    if (initData.empty())
    {
        LogError("init failed: data size = 0");
        return false;
    }

    auto minFinder =
        std::min_element(initData.begin(), initData.end(), [](const TIS_Info::DuagonCardData &a, const TIS_Info::DuagonCardData &b) {
            return a.m_interval < b.m_interval;
        });

    int minInterval = minFinder->m_interval.count();

    const int maxRetries = 3;
    const int retryDelayMs = 1000;

    auto retry = [&](const std::function<int()> &func, const char *desc) -> bool {
        for (int attempt = 1; attempt <= maxRetries; ++attempt)
        {
            int result = func();
            if (result == AS_OK || result == AP_OK)
                return true;
            LogError("{} failed (attempt {}/{}), error code: {}", desc, attempt, maxRetries, result);
            std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
        }
        return false;
    };

    if (!retry([&]() { return as_init_idx(0); }, "as_init_idx"))
        return false;

    if (!retry([&]() { return ap_init_idx(0); }, "ap_init_idx"))
        return false;

    if (!retry([&]() { return ap_ts_config_idx(0, minInterval, 0); }, "ap_ts_config_idx"))
        return false;

    /************************接收和发送端口配置**********************************/
    for (const auto &it : initData)
    {
        if (!configPorts(it.m_portId, it.m_portSize, it.m_bRead))
        {
            return false;
        }
    }

    /************************配置服务**********************************/
    if (!configService())
    {
        return false;
    }
    for (const auto &item : initData)
    {
        if (item.m_bRead)
        {
            m_userCardData.push_back(item);
        }
    }

    m_thProcess = std::thread(&DuagonCardMVB::threadProcess, this);
    bInit = true;
    return true;
}

void DuagonCardMVB::setCallBack(const std::function<void(const TIS_Info::DuagonCardData &)> &recvFunc)
{
    m_readCallbackfunc = recvFunc;
}

bool DuagonCardMVB::writeData(const TIS_Info::DuagonCardData &data)
{
    if (!bInit)
    {
        LogError("发送数据失败，板卡未完成初始化。");
        return false;
    }
    DS_NAME ds_name;
    ds_name.traffic_store_id = 0;         //	n:流量存储id
    ds_name.port_address = data.m_portId; //	n:端口
    AP_RESULT ap_result = AP_ERROR;
    {
        std::lock_guard<std::mutex> lock(m_mtxData);
        ap_result = ap_put_dataset_idx(&ds_name,
                                       (void *)data.m_portData.data(),
                                       0); //	n:将要发布的数据的DS_NAME,将要发送数据的起始地址，板卡下标：0
    }
    if (AP_OK != ap_result)
    {
        LogError("Pms Send to Target(0x{:x}) Error: {}", data.m_portId, static_cast<int>(ap_result));
        return false;
    }

    LogInfo("Pms Send to Target(0x{:x}) success:\n[{}]",
            data.m_portId,
            ByteArrayTools::hexString(data.m_portData.data(), data.m_portData.size()));
    return true;
}

bool DuagonCardMVB::readData(TIS_Info::DuagonCardData &data)
{
    DS_NAME ds_name;
    UNSIGNED16 pd_port_freshness = 0;
    ds_name.traffic_store_id = 0;         //	n:流量存储id:0
    ds_name.port_address = data.m_portId; //	n:端口
    AP_RESULT ap_result = AP_ERROR;
    {
        std::lock_guard<std::mutex> lock(m_mtxData);
        ap_result = ap_get_dataset_idx(&ds_name,
                                       (void *)data.m_portData.data(),
                                       &pd_port_freshness,
                                       0); //	n:将要接收的数据的DS_NAME（数据集名字），接受到的数据的起始地址，数据的新鲜度时间
    }
    if (ap_result != AP_OK)
    {
        LogError("read data from Target(0x{:x}) Error: {}", data.m_portId, static_cast<int>(ap_result));
        return false;
    }
    LogInfo("read data from Target(0x{:x}) success:\n[{}] ",
            data.m_portId,
            ByteArrayTools::hexString(data.m_portData.data(), data.m_portData.size()));
    return true;
}

bool DuagonCardMVB::configPorts(const int port, const int size, bool bRead)
{
    WORD16 pd_port_address = port;                                   //	端口号
    PD_PRT_ATTR pd_prt_attr;                                         //	n:端口属性
    pd_prt_attr.port_size = size;                                    //	端口尺寸
    pd_prt_attr.port_config = bRead ? PD_SINK_PORT : PD_SOURCE_PORT; // 端口_接收(PD_SINK_PORT)， 端口_发送（PD_SOURCE_PORT）
    pd_prt_attr.p_bus_specific = NULL;
    auto ap_result = ap_port_manage_idx(0,
                                        pd_port_address,
                                        PD_PRT_CMD_CONFIG,
                                        &pd_prt_attr,
                                        0); //	n:traffic store id:0,port
                                            // address:0x01,cmd:配置,端口属性，板卡下标：0
    if (ap_result != AP_OK)
    {
        LogError("ap_port_manage_idx Target(0x{:x}) Error {}", port, static_cast<int>(ap_result));
        return false;
    }
    LogInfo("ap_port_manage_idx Target(0x{:x}) Success.", port);
    return true;
}

bool DuagonCardMVB::configService()
{
    SV_MVB_CONTROL mvb_control;
    mvb_control.device_address = 0xE6; // MPU1_DEVICE_ADDRESS
    mvb_control.reserved1 = 0x00;
    mvb_control.t_ignore = 0x00;
    mvb_control.reserved2 = 0x00;
    mvb_control.command = SV_MVB_CTRL_CMD_BIT_SLA;
    mvb_control.command |= SV_MVB_CTRL_CMD_BIT_SLB;
    mvb_control.reserved3 = 0x0000;
    //	n：处理链路层特定的服务
    auto as_result = as_service_handler_idx(
        0, SV_MVB_SERVICE_WRITE_CONTROL, (void *)&mvb_control, 0); //	n:链路id:0，服务:write，服务参数，板卡下标：0
    if (as_result != AS_OK)
    {
        LogError("as_service_handler_idx failed,as_result:{}", (UNSIGNED16)as_result);
        return false;
    }
    LogInfo("as_service_handler_idx success.");
    return true;
}
void DuagonCardMVB::processItem(TIS_Info::DuagonCardData &item)
{
    if (item.m_bRead)
    { // 读取数据处理
        if (readData(item) && m_readCallbackfunc)
        {
            m_readCallbackfunc(item);
        }
    }
}
void DuagonCardMVB::threadProcess()
{
    while (m_bHolder)
    {
        // 计算下次唤醒时间
        auto nextWakeTime = std::chrono::steady_clock::time_point::max();

        auto now = std::chrono::steady_clock::now();
        for (auto &item : m_userCardData)
        {
            if (now >= item.m_nextClock)
            {
                // 执行任务
                item.m_nextClock = now + item.m_interval; // 先计算时延，减小误差
                processItem(item);
            }
            nextWakeTime = std::min(nextWakeTime, item.m_nextClock);
        }

        // 等待到下次执行时间
        if (nextWakeTime != std::chrono::steady_clock::time_point::max())
        {
            std::this_thread::sleep_until(nextWakeTime);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
