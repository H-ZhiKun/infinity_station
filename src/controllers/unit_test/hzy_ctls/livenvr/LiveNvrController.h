#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include "kits/thrid_devices/live_nvr/LiveNvr.h"
#include "kits/communication/http_client/HttpClient.h"
#include <qvariant.h>
#include <string>
#include <memory>
#include <QDateTime>
#include "tis_global/Struct.h"
#include "tis_global/Function.h"

namespace _Controllers
{
    class LiveNvrController : public ControllerBase<LiveNvrController>
    {
      public:
        LiveNvrController();
        ~LiveNvrController() = default;
        void recordStop(const QVariant &);
        void onChanelReceived(int);
        void onRecordButtonClicked(const QVariant &data);
        void onNvrDataInfoRecv(const QVector<TIS_Info::NvrDataInfo>&);
        void recordStart(const QString &videoDir, const QString &videoName, std::string cameraId);

        TASK_LIST_BEGIN
        ASYNC_TASK_ADD(TIS_Info::LiveNvrService::sendChanel, LiveNvrController::onChanelReceived);
        ASYNC_TASK_ADD(TIS_Info::LiveNvrService::sendEndRecordState, LiveNvrController::recordStop);
        ASYNC_TASK_ADD(TIS_Info::LiveNvrService::sendNvrDataInfoToController, LiveNvrController::onNvrDataInfoRecv);

        QML_ADD(TIS_Info::QmlCommunication::QmlActions::VideoRecordButton, LiveNvrController::onRecordButtonClicked);
        TASK_LIST_END

      private:
        TIS_Info::NvrDataInfo m_NvrDataInfo;
        QVector<TIS_Info::NvrDataInfo> m_vecVideoInfo;
        TIS_Info::LogInfo m_LogInfo;
        QString m_strUrl;
        std::unique_ptr<_Kits::LiveNvr> m_livenvr = nullptr;
        int m_lChannel = 0;
    };
} // namespace _Controllers
