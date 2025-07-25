#include "BaumerArc.h"
#include "kits/common/object_pool/ObjectPool.h"
#include <QDebug>
#include <memory>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "kits/common/log/CRossLogger.h"
namespace _Kits
{
    BaumerArc::BaumerArc() : BaumerBase(), m_bSuccess(false)
    {
    }
    BaumerArc::~BaumerArc() noexcept
    {
        closeCamera();
    }

    bool BaumerArc::init(BGAPI2::Device *pDevice)
    {
        if (pDevice == nullptr)
        {
            LogError("BaumerBase::init: Device pointer is null.");
            return false;
        }

        m_pDevice = pDevice;
        return true;
    }

    void BGAPI2CALL BaumerArc::BufferHandler(void *callBackOwner, BGAPI2::Buffer *pBufferFilled)
    {
        BaumerArc *pCamera = static_cast<BaumerArc *>(callBackOwner);
        if (pBufferFilled == NULL)
        {
            return;
        }
        if (pBufferFilled->GetIsIncomplete())
        {
            pBufferFilled->QueueBuffer();
        }
        else
        {
            pCamera->storeImg(pBufferFilled);
            pBufferFilled->QueueBuffer();
        }
    }
    void BaumerArc::storeImg(BGAPI2::Buffer *pBufferFilled)
    {
        if (m_name_.empty()) // 获取到名字再传输
        {
            return;
        }

        uint64_t width = pBufferFilled->GetWidth();
        uint64_t height = pBufferFilled->GetHeight();
        unsigned char *imageData = static_cast<unsigned char *>(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset();
        auto imgBuffer = m_ImageBufferPools->getObject(width, height, 3);
        auto img_size = pBufferFilled->GetImageLength();
        // 直接使用 img->data，不需要每次 resize
        if (imgBuffer->data.size() >= img_size)
        {
            memcpy(imgBuffer->data.data(), imageData, img_size);
            imgBuffer->name = m_name_;
            imgBuffer->addr = m_ip_;
            imgBuffer->height = height;
            imgBuffer->width = width;
            imgBuffer->timestamp = pBufferFilled->GetTimestamp();
            imgBuffer->pixFormat = (int)TIS_Info::EnumPublic::AVPixelFormat::AV_PIX_FMT_GRAY8;
            emit sendArcImage(imgBuffer);
        }
        else
        {
            qDebug() << "image data size error.";
        }
    }
    bool BaumerArc::start()
    {
        openDataStream();
        if (m_bSuccess)
        {
            m_ip_ = m_pDevice->GetRemoteNode("GevCurrentIPAddress")->GetValue().get();
            m_ImageBufferPools = std::make_shared<ObjectPool<TIS_Info::ImageBuffer>>();
        }
        return m_bSuccess;
    }

    void BaumerArc::openDataStream()
    {
        try
        {
            auto pStreams = m_pDevice->GetDataStreams();
            pStreams->Refresh();
            if (pStreams->size() > 0)
            {
                m_pStream = (*pStreams)[0];
                if (!m_pStream->IsOpen())
                {
                    m_pStream->Open();
                    addBufferToStream();
                    m_pStream->RegisterNewBufferEvent(BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
                    m_pStream->RegisterNewBufferEventHandler(this, (BGAPI2::Events::NewBufferEventHandler)&BaumerArc::BufferHandler);
                    m_pStream->StartAcquisitionContinuous();
                    if (m_pDevice->GetRemoteNode(SFNC_ACQUISITION_START)->IsWriteable())
                    {
                        m_pDevice->GetRemoteNode(SFNC_ACQUISITION_START)->Execute();
                    }
                    m_bSuccess = true;
                }
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Error Type:" << ex.GetType().get();
            qDebug() << "Error function: " << ex.GetFunctionName().get();
            qDebug() << "Error description: " << ex.GetErrorDescription().get();

            if (m_pDevice != nullptr)
            {
                m_pDevice->Close();
                m_pDevice = nullptr;
            }

            if (m_pStream != nullptr)
            {
                m_pStream->Close();
                m_pStream = nullptr;
            }
        }
    }
    void BaumerArc::addBufferToStream()
    {
        if (m_pStream->IsOpen())
        {
            auto pBufferList = m_pStream->GetBufferList();
            for (int i = 0; i < 10; i++)
            {
                auto buffer = new BGAPI2::Buffer;
                pBufferList->Add(buffer);
                m_vBuffers.push_back(buffer);
            }
            // 不知道baumer的demo为什么要先申请，再调用QueueBuffer, 照抄的
            for (auto iter = pBufferList->begin(); iter != pBufferList->end(); ++iter)
            {
                iter->second->QueueBuffer();
            }
        }
    }

    bool BaumerArc::closeCamera()
    {
        try
        {
            closeDataStream();

            if (m_pDevice != NULL)
            {
                m_pDevice->GetRemoteNode("AcquisitionStop")->Execute();
            }

            return true;
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Error Type:" << ex.GetType().get();
            qDebug() << "Error function: " << ex.GetFunctionName().get();
            qDebug() << "Error description: " << ex.GetErrorDescription().get();
            return false;
        }
    }

    void BaumerArc::closeDataStream()
    {

        // 停止相机的采集流
        try
        {
            if (m_pStream != NULL)
            {
                m_pStream->StopAcquisition();
                m_pStream->GetBufferList()->DiscardAllBuffers();
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Error Type:" << ex.GetType().get();
            qDebug() << "Error function: " << ex.GetFunctionName().get();
            qDebug() << "Error description: " << ex.GetErrorDescription().get();
        }
        // 释放相机的采集流
        try
        {
            if (m_pStream != NULL)
            {
                while (m_pStream->GetBufferList()->size() > 0)
                {
                    auto *buffer = m_pStream->GetBufferList()->begin()->second;
                    m_pStream->GetBufferList()->RevokeBuffer(buffer);
                    delete buffer;
                }
                // 关闭相机的数据流
                m_pStream->Close();
                // 关闭相机的设备
                m_pDevice->Close();
            }
        }
        catch (BGAPI2::Exceptions::IException &ex)
        {
            qDebug() << "Error Type:" << ex.GetType().get();
            qDebug() << "Error function: " << ex.GetFunctionName().get();
            qDebug() << "Error description: " << ex.GetErrorDescription().get();
        }
    }
} // namespace _Kits