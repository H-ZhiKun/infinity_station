#include "BaumerCamera.h"

using namespace _Kits;

void BGAPI2CALL BufferHandler(void *callBackOwner,
                              BGAPI2::Buffer *pBufferFilled)
{
    try
    {
        qDebug() << "bufferhandle in";
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
            uint64_t width = pBufferFilled->GetWidth();
            uint64_t height = pBufferFilled->GetHeight();
            unsigned char *imageData =
                static_cast<unsigned char *>(pBufferFilled->GetMemPtr());
            //     static_cast<unsigned char *>(pBufferFilled->GetMemPtr())
            // unsigned char *imageData =
            //     static_cast<unsigned char *>(pBufferFilled->GetMemPtr()) +
            //     pBufferFilled->GetImageOffset();
            pCamera->storeImg(imageData,
                              pBufferFilled->GetPixelFormat().get(),
                              width,
                              height,
                              pBufferFilled->GetFrameID());
            pBufferFilled->QueueBuffer();
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        //  ex.GetType().get();
        // LogError("Error function: {}", ex.GetFunctionName().get());
        // LogError("Error description: {}", ex.GetErrorDescription().get());
    }
}

BaumerCamera::BaumerCamera(BGAPI2::Device *bgapi_device,
                           const YAML::Node &config)
    : pDev_(bgapi_device), param_(config)
{
}

BaumerCamera::~BaumerCamera()
{
}
void BaumerCamera::initParams()
{
    try
    {
        double exposeTime = param_["expose"].as<double>() * 1.0;
        int gain = param_["gain"].as<int>();
        pDev_->GetRemoteNode("TriggerMode")->SetString("Off");
        pDev_->GetRemoteNode(SFNC_EXPOSUREAUTO)->SetString("Off");
        pDev_->GetRemoteNode(SFNC_EXPOSURETIME)->SetDouble(exposeTime);

        // pDev_->GetRemoteNode(SFNC_TRIGGERMODE)->SetValue("Off");
        pDev_->GetRemoteNode(SFNC_GAINAUTO)->SetString("Off");
        pDev_->GetRemoteNode(SFNC_GAIN)->SetInt(gain);
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "error description" << ex.GetErrorDescription().get();
    }
}

void BaumerCamera::setSnStr(const std::string &sn)
{
    snStr = sn;
}

void BaumerCamera::storeImg(unsigned char *buffer,
                            const std::string &pixelFormat,
                            uint64_t width,
                            uint64_t height,
                            uint64_t frameId)
{
    if (pixelFormat == "Mono8")
    {

        qDebug() << "imgstore in mono8 width:" << width << "height:" << height;

        QImage img(buffer, width, height, QImage::Format_Grayscale8);
        std::lock_guard lock(mtxcrt);
        // img.save(
        //     "D:/workspace/togee_infinity_station/x64-windows-debug/test.jpg");
        matBuffer_.push(img);
    }
    else if (pixelFormat == "Mono10")
    {
        uint16_t *mono10Data = reinterpret_cast<uint16_t *>(buffer);
        QImage img(width, height, QImage::Format_Grayscale8);
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                // 获取 10 位灰度值
                uint16_t mono10Value = mono10Data[y * width + x];

                // 将 10 位数据缩放到 8 位
                uint8_t grayValue =
                    static_cast<uint8_t>((mono10Value * 255) / 1023);

                // 设置像素值
                img.setPixel(x, y, grayValue);
            }
        }
        std::lock_guard lock(mtxcrt);
        img.save(
            "D:/workspace/togee_infinity_station/x64-windows-debug/test.jpg");
        matBuffer_.push(img);
    }

    // emit sendImage(snStr, img);
}
void BaumerCamera::initialize()
{
    if (openDevice())
    {
        bOpen_ = true;
        initParams();
        if (addBufferToStream())
        {
            startGrab();
        }
    }
}
bool BaumerCamera::openDevice()
{
    bool ret = false;
    try
    {
        std::string devStatus = pDev_->GetAccessStatus().get();
        qDebug() << "open devStatus 0:" << devStatus;
        pDev_->Open();
        // pDev_->OpenReadOnly();
        qDebug() << "open devStatus 1:" << devStatus;
        ret = true;
    }
    catch (BGAPI2::Exceptions::ResourceInUseException &ex)
    {
        qDebug() << "ErrorDescription" << ex.GetErrorDescription().get();
    }
    catch (BGAPI2::Exceptions::AccessDeniedException &ex)
    {
        qDebug() << "error AccessDeniedException"
                 << ex.GetErrorDescription().get();
    }
    try
    {

        BGAPI2::DataStreamList *datastream_list = pDev_->GetDataStreams();
        datastream_list->Refresh();
        if (datastream_list->size() > 0)
        {
            pStream_ = datastream_list->begin()->second;
            pStream_->Open();
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        ret = false;
        qDebug() << "error Type" << ex.GetType().get();
        qDebug() << "error FunctionName" << ex.GetFunctionName().get();
        qDebug() << "error description" << ex.GetErrorDescription().get();
    }
    return ret;
}
bool BaumerCamera::addBufferToStream()
{
    bool ret = false;

    try
    {
        pDev_->GetDataStreams()->Refresh();
        BGAPI2::BufferList *bufferList = pStream_->GetBufferList();
        for (int i = 0; i < 5; i++)
        {
            BGAPI2::Buffer *buffer = new BGAPI2::Buffer();
            bufferList->Add(buffer);
            buffer->QueueBuffer();
            streamBuffers_.push_back(buffer);
        }
        pStream_->RegisterNewBufferEvent(
            BGAPI2::Events::EVENTMODE_EVENT_HANDLER);
        pStream_->RegisterNewBufferEventHandler(
            this, (BGAPI2::Events::NewBufferEventHandler)&BufferHandler);
        qDebug() << "process in addbuffer";
        pStream_->StartAcquisitionContinuous();
        qDebug() << "process in start continuous";
        ret = true;
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {

        qDebug() << "error Description" << ex.GetErrorDescription().get();
        qDebug() << "error functionname" << ex.GetFunctionName().get();
        qDebug() << "error type " << ex.GetType().get();
    }
    return ret;
}

void BaumerCamera::deinitialized()
{
    stopGrab();
    clearBufferFromStream();
    stopDevice();
    // 由这里统一释放，避免异常导致的泄漏问题
    for (auto &it : streamBuffers_)
    {
        delete it;
    }
    streamBuffers_.clear();
}

void BaumerCamera::clearBufferFromStream()
{
    try
    {
        if (pStream_ != nullptr)
        {
            pStream_->StopAcquisition();
            pStream_->UnregisterNewBufferEvent();
            // pStream_->RegisterNewBufferEvent(BGAPI2::Events::EVENTMODE_UNREGISTERED);
            auto bufferList = pStream_->GetBufferList();
            bufferList->DiscardAllBuffers();
            BGAPI2::BufferList *buffer_list = pStream_->GetBufferList();
            while (buffer_list->size() > 0)
            {
                BGAPI2::Buffer *buffer = buffer_list->begin()->second;
                buffer_list->RevokeBuffer(buffer);
            }
            pStream_->Close();
            pStream_ = nullptr;
        }
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "error type " << ex.GetType().get();
        qDebug() << "error function" << ex.GetFunctionName().get();
        qDebug() << "Error description" << ex.GetErrorDescription().get();
    }
}

void BaumerCamera::stopDevice()
{
    if (pDev_ == nullptr)
    {
        return;
    }
    try
    {
        pDev_->Close();
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Error Type: {}" << ex.GetType().get();
        qDebug() << "Error function:" << ex.GetFunctionName().get();
        qDebug() << "Error Description" << ex.GetErrorDescription().get();
    }
}

void BaumerCamera::startGrab()
{
    try
    {
        qDebug() << "process in start grab";
        if (pDev_->GetRemoteNode(SFNC_ACQUISITION_START)->IsWriteable())
        {
            pDev_->GetRemoteNode(SFNC_ACQUISITION_START)->Execute();
            bActive_ = true;
        }

        // return true;
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "Baumer camera error:" << ex.GetErrorDescription().get();
        // return false;
    }
}

void BaumerCamera::stopGrab()
{
    bActive_ = false;
    if (pDev_ == nullptr)
    {
        return;
    }
    try
    {
        if (pDev_->GetRemoteNodeList()->GetNodePresent("AcquisitionAbort"))
        {
            pDev_->GetRemoteNode("AcquisitionAbort")->Execute();
        }
        if (pDev_->GetRemoteNode(SFNC_ACQUISITION_STOP)->IsWriteable())
        {
            pDev_->GetRemoteNode(SFNC_ACQUISITION_STOP)->Execute();
        }
        pDev_->GetRemoteNode("TriggerMode")->SetString("Off");
    }
    catch (BGAPI2::Exceptions::IException &ex)
    {
        qDebug() << "error type" << ex.GetType().get();
        qDebug() << "error funchtionName" << ex.GetFunctionName().get();
        qDebug() << "error Description " << ex.GetErrorDescription().get();
    }
}

bool BaumerCamera::getInitialized()
{
    return bActive_;
}

QImage BaumerCamera::getCurrentImage()
{
    std::lock_guard lock(mtxcrt);
    if (!matBuffer_.empty())
    {
        auto img = matBuffer_.front();
        matBuffer_.pop();
        return img;
    }
    return {};
}