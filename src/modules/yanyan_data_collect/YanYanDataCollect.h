#pragma once
#include <atomic>
#include <json/json.h>
#include <memory>
#include <qtmetamacros.h>
#include "kits/yanyan/YanYanDeviceByMFC.h"
#include "kits/common/module_base/ModuleBase.h"
#include "tis_global/Struct.h"
#include "kits/common/utils/BlockingQueue.h"


/***************************************************************************
 * @file    YanYanDataCollect.h
 * @brief
 *
 *
 *
 * @note
 ***************************************************************************/
namespace _Modules
{
    class YanYanDataCollect : public _Kits::ModuleBase
    {
        Q_OBJECT
        DECLARE_MODULE(YanYanDataCollect)
      public:
        explicit YanYanDataCollect(QObject *parent = nullptr);
        virtual ~YanYanDataCollect() noexcept = default;
        YanYanDataCollect(const YanYanDataCollect &) = delete;
        YanYanDataCollect &operator=(const YanYanDataCollect &) = delete;

        virtual bool init(const YAML::Node &config) override;
        virtual bool start() override;
        virtual bool stop() override;

      signals:
        // void IRImageReady(std::shared_ptr<TIS_Info::IRImageBuffer> image);
        void rawDataReady(std::shared_ptr<TIS_Info::RawData>);
        void initSignal(int);
        void stopSignal();

      private:
        void handleCallbackData(JCWDVER, const void*, uint32_t);
        static void yanyanCallback(const void*, JCWDVER, const void*, UINT);

      private:
        std::atomic<bool> m_startFlag = false; // 启动标志
        std::unique_ptr<_Kits::YanYanDeviceByMFC> m_yanyanDevicePtr = nullptr;
        Jcw_fnJCWDResultCallBack m_callback = nullptr;
        std::shared_ptr<TIS_Info::RawData> m_yanyanRawData = nullptr;
        int m_saveRawDataFlag = 0;
    };
} // namespace _Modules
