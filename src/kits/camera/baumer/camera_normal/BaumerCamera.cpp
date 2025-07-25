#include "BaumerCamera.h"
#include "kits/common/object_pool/ObjectPool.h"
#include <QDebug>
#include <memory>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include "kits/common/log/CRossLogger.h"
#include "tis_global/EnumPublic.h"

namespace _Kits
{
    BaumerCamera::BaumerCamera() : BaumerBase(), m_bSuccess(false)
    {
    }
    BaumerCamera::~BaumerCamera()
    {
        closeCamera();
    }

    bool BaumerCamera::init(BGAPI2::Device *pDevice)
    {
        if (pDevice == nullptr)
        {
            LogError("BaumerBase::init: Device pointer is null.");
            return false;
        }

        m_pDevice = pDevice;
        return true;
    }

    void BaumerCamera::storeImg(BGAPI2::Buffer *pBufferFilled)
    {
        if (m_name_.empty()) // 获取到名字再传输
        {
            return;
        }

        uint64_t width = pBufferFilled->GetWidth();
        uint64_t height = pBufferFilled->GetHeight();

        unsigned char *imageData = static_cast<unsigned char *>(pBufferFilled->GetMemPtr()) + pBufferFilled->GetImageOffset();
        auto imgBuffer = m_ImageBufferPools->getObject(width, height, 3);
        // 直接使用 img->data，不需要每次 resize
        auto img_size = pBufferFilled->GetImageLength();
        if (imgBuffer->data.size() >= img_size)
        {
            std::memcpy(imgBuffer->data.data(), imageData, img_size);
            imgBuffer->name = m_name_;
            imgBuffer->addr = m_ip_;
            imgBuffer->height = height;
            imgBuffer->width = width;
            imgBuffer->timestamp = pBufferFilled->GetTimestamp();
            imgBuffer->pixFormat = (int)TIS_Info::EnumPublic::AVPixelFormat::AV_PIX_FMT_GRAY8;
            emit sendImage(imgBuffer);
        }
        else
        {
            qDebug() << "image data size error.";
        }
    }
    bool BaumerCamera::start()
    {
        openDataStream();
        if (m_bSuccess)
        {
            m_ip_ = m_pDevice->GetRemoteNode("GevCurrentIPAddress")->GetValue().get();
            m_ImageBufferPools = std::make_shared<ObjectPool<TIS_Info::ImageBuffer>>();
        }
        return m_bSuccess;
    }

    void BaumerCamera::openDataStream()
    {
        try
        {
            auto pStreams = m_pDevice->GetDataStreams();
            pStreams->Refresh();
            if (pStreams->size() > 0)
            {
                m_pStream = pStreams->begin()->second;
                if (!m_pStream->IsOpen())
                {
                    m_pStream->Open();
                    addBufferToStream();
                    m_pStream->RegisterNewBufferEvent(BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
                    m_pStream->RegisterNewBufferEventHandler(
                        this, static_cast<BGAPI2::Events::NewBufferEventHandler>([](void *callBackOwner, BGAPI2::Buffer *pBufferFilled) {
                            try
                            {
                                BaumerCamera *pCamera = static_cast<BaumerCamera *>(callBackOwner);
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
                            catch (BGAPI2::Exceptions::IException &ex)
                            {
                                qDebug() << "Error Type:" << ex.GetType().get();
                                qDebug() << "Error function: " << ex.GetFunctionName().get();
                                qDebug() << "Error description: " << ex.GetErrorDescription().get();
                            }
                        }));

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
    void BaumerCamera::addBufferToStream()
    {
        if (m_pStream->IsOpen())
        {
            auto pBufferList = m_pStream->GetBufferList();
            for (int i = 0; i < 5; i++)
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

    bool BaumerCamera::closeCamera()
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

    void BaumerCamera::closeDataStream()
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