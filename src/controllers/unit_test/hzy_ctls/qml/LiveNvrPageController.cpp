#include "LiveNvrPageController.h"
#include "kits/common/log/CRossLogger.h"
#include <QCryptographicHash> // 新增MD5支持头文件
#include "service/AppFramework.h"
#include <QStringList>
#include <qvariant.h>
#include "tis_global/Function.h"
namespace _Controllers
{
    using namespace _Service;
    using namespace _Kits;

    LiveNvrPageController::LiveNvrPageController()
    {
        m_livenvr = std::make_unique<_Kits::LiveNvr>();
    }
    void LiveNvrPageController::onRecordButtonClicked(const QVariant &data)
    {
        /* data 必须是一个 map，里面至少包含 camera_id 和 state 两个字段 */
        if (!data.canConvert<QVariantMap>())
            return;

        QVariantMap map = data.toMap();
        const QString cameraId = map.value("camera_id").toString();
        const bool    state    = map.value("state").toBool();

        if (cameraId.isEmpty())
            return;          

        /* 如果需要登录检查，统一放在这里 */
        if (!m_livenvr->checkLogin())
            return;

        if (state)
            recordStart("E:/test","test.mp4","10.10.10.123");   // 把 camera_id 传进去，方便内部区分
        else
            recordStop("test");
    }

    void LiveNvrPageController::recordStart(const QString &videoDir, const QString &videoName, std::string cameraIp)
    {
        TIS_Info::NvrDataInfo NvrDataInfo;
        NvrDataInfo.beginTime = QDateTime::currentDateTime();
        NvrDataInfo.lChannel = -1;
        QString fullVideoName = videoName + ".mp4";
        NvrDataInfo.strPath = videoDir + fullVideoName;
        NvrDataInfo.endTime = QDateTime::currentDateTime();
        m_NvrDataInfo = NvrDataInfo;
        if(App().invokeModuleAsync(TIS_Info::LiveNvrService::onNvrDataInfoRecv, NvrDataInfo))
        {
            m_livenvr->getChannelByCameraIP(QString::fromStdString(cameraIp));
        }
    }
    void LiveNvrPageController::recordStop(const QVariant & data)
    {
        App().invokeModuleAsync(TIS_Info::LiveNvrService::onEndRecord, data);
    }

} // namespace _Controllers
// namespace _Controllers