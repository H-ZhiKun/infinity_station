#include "CorrugationService.h"
#include "kits/common/log/CRossLogger.h"
#include <functional>
#include <json/value.h>
#include <qobject.h>
#include <qtimer.h>

// using namespace _Modules;
using namespace _Kits;
namespace _Modules
{

    CorrugationService::CorrugationService(QObject *parent)
    {
    }
    CorrugationService::~CorrugationService() noexcept
    {
    }

    bool CorrugationService::init(const YAML::Node &config)
    {

        auto ret = m_CorrugationBaseptr->init(config);
        if (0 == ret)
        {

            auto recvCallback = std::bind(&CorrugationService::recvCorrugationData, this, std::placeholders::_1);
            m_CorrugationBaseptr->setCallBack(recvCallback);
            m_CorrugationBaseptr->start();
        }

        return true;
    }
    bool CorrugationService::start()
    {
        return true;
    }
    bool CorrugationService::stop()
    {
        if (nullptr == m_CorrugationBaseptr)
        {
            return false;
        }
        m_CorrugationBaseptr->stop();
        return true;
    }

    void CorrugationService::setupData(int n)
    {
        QVariantMap js;
        js["test"] = n;
        emit updateData(js);
    }

    void CorrugationService::recvCorrugationData(const CorrugationData &corrugationData)
    {
        // 处理波磨数据
    }

    bool CorrugationService::sendData(const QVariant &msg)
    {
        if (nullptr == m_CorrugationBaseptr)
        {
            return false;
        }
        uint8_t *data = nullptr;
        int len = 0;
        auto ret = m_CorrugationBaseptr->sendData(data, len);
        if (0 != ret)
        {
            return false;
        }
        return true;
    }

} // namespace _Modules