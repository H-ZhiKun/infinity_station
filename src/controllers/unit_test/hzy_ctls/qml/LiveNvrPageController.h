#pragma once
#include "kits/common/controller_base/ControllerBase.h"
#include "kits/thrid_devices/live_nvr/LiveNvr.h"
#include "kits/communication/http_client/HttpClient.h"
#include <qvariant.h>
#include <string>
#include <memory>
#include <QDateTime>

namespace _Controllers
{
    class LiveNvrPageController : public ControllerBase<LiveNvrPageController>
    {
      public:
        LiveNvrPageController();
        ~LiveNvrPageController() = default;
        void onRecordButtonClicked(const QVariant &);
        void recordStart(const QString &videoDir, const QString &videoName, std::string cameraId);
        void recordStop(const QVariant &data);

        TASK_LIST_BEGIN
        QML_ADD(TIS_Info::QmlCommunication::QmlActions::VideoRecordButton, LiveNvrPageController::onRecordButtonClicked);
        TASK_LIST_END

      private:
        TIS_Info::NvrDataInfo m_NvrDataInfo;
        TIS_Info::LogInfo m_LogInfo;
        QString m_strUrl;
        std::unique_ptr<_Kits::LiveNvr> m_livenvr = nullptr;
        long m_lChannel = 0;
    };
} // namespace _Controllers
