#include "LiveNvrController.h"
#include "kits/common/log/CRossLogger.h"
#include <QCryptographicHash> // 新增MD5支持头文件

#include "service/AppFramework.h"
#include <QStringList>
#include <qvariant.h>
#include <QJSValue>
#include <QJSValueIterator>
#include <QJSEngine>
namespace _Controllers
{
    using namespace _Service;
    using namespace _Kits;

    LiveNvrController::LiveNvrController()
    {
        m_livenvr = std::make_unique<_Kits::LiveNvr>();
    }
    void LiveNvrController::onRecordButtonClicked(const QVariant &data)
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
    void LiveNvrController::onChanelReceived(int channel)
    {
        if(channel)
        {
            m_NvrDataInfo.lChannel = channel;
        }
        else
        {
            m_NvrDataInfo.lChannel = -1;
        }
    }
    void LiveNvrController::onNvrDataInfoRecv(const QVector<TIS_Info::NvrDataInfo> &data)
    {
        if (data.isEmpty())
            return;

        m_vecVideoInfo = data;
    }

    void LiveNvrController::recordStart(const QString &videoDir, const QString &videoName, std::string cameraIp)
    {
        TIS_Info::NvrDataInfo NvrDataInfo;
        NvrDataInfo.beginTime = QDateTime::currentDateTime();
        NvrDataInfo.lChannel = -1;
        QString fullVideoName = videoName + ".mp4";
        NvrDataInfo.strPath = videoDir + fullVideoName;
        NvrDataInfo.endTime = QDateTime::currentDateTime();
        m_NvrDataInfo = NvrDataInfo;

        m_livenvr->getChannelByCameraIP(QString::fromStdString(cameraIp));
        
    }

void LiveNvrController::recordStop(const QVariant &data)
{
    if (!data.canConvert<QVariantMap>())
        return;

    QVariantMap map = data.toMap();
    const QString cameraId = map.value("camera_id").toString();
    if (cameraId.isEmpty())
        return;

    m_NvrDataInfo.endTime   = QDateTime::currentDateTime();

    /* 2. 拉取该通道下待下载的文件列表 */

    m_livenvr->loadInVideInfoOfOneChannel(m_NvrDataInfo);

    /* 3. 倒序遍历 + 删除，避免迭代器失效 */
    for (int i = static_cast<int>(m_vecVideoInfo.size()) - 1; i >= 0; --i)
    {
        if (m_livenvr->downloadOneFileBy(m_vecVideoInfo[i]))
        {
            m_vecVideoInfo.erase(m_vecVideoInfo.begin() + i);
        }
    }

}



} // namespace _Controllers
// namespace _Controllers