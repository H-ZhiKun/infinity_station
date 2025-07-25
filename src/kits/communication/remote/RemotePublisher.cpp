
#include <vector>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include "RemotePublisher.h"
#include "kits/common/log/CRossLogger.h"
#include "tis_global/Struct.h"
namespace _Kits
{
    bool RemotePublisher::initialize(const std::string &protocol, const std::string &ip, const std::string &port, int timeout, int hwm)
    {
        try
        {
            setupSocket(ZMQ_PUB);
            parseConfig(true, protocol, ip, port, timeout, hwm);

            int linger = 0;
            socket_->set(zmq::sockopt::linger, linger);

            socket_->bind(address_);
            connected_ = true;

            m_bRunning = true;
            m_thWork = std::thread(&RemotePublisher::processMessage, this);

            return true;
        }
        catch (const std::exception &e)
        {
            LogError("RemotePublisher initialize error: {}", e.what());
            return false;
        }
    }

    void RemotePublisher::stop()
    {
        m_bRunning = false;
        m_cvMsg.notify_all();
        if (m_thWork.joinable())
        {
            m_thWork.join();
        }

        if (socket_)
        {
            socket_->close();
        }

        if (context_)
        {
            context_->close();
        }

        connected_ = false;
    }

    bool RemotePublisher::publish(const TIS_Info::RemoteMessage &msg)
    {
        if (!connected_)
            return false;

        try
        {
            zmq::multipart_t multipart;
            multipart.add(zmq::message_t(msg.topic));
            multipart.add(zmq::message_t(msg.message_id));
            multipart.add(zmq::message_t(msg.json_data));
            multipart.add(zmq::message_t(msg.binary_data.data(), msg.binary_data.size()));

            multipart.send(*socket_);
            return true;
        }
        catch (const zmq::error_t &e)
        {
            LogError("RemotePublisher publish error: {}", e.what());
            return false;
        }
    }

    bool RemotePublisher::publishAsync(const TIS_Info::RemoteMessage &msg)
    {
        if (!connected_)
            return false;

        try
        {
            std::lock_guard<std::mutex> lock(m_mtxMsg);
            zmq::multipart_t mutiMsg;
            mutiMsg.add(zmq::message_t(msg.topic));
            mutiMsg.add(zmq::message_t(msg.message_id));
            mutiMsg.add(zmq::message_t(msg.json_data));
            mutiMsg.add(zmq::message_t(msg.binary_data.data(), msg.binary_data.size()));
            m_messages.push(std::move(mutiMsg));
            m_cvMsg.notify_one();
            return true;
        }
        catch (const zmq::error_t &e)
        {
            LogError("RemotePublisher publishAsync error: {}", e.what());
            return false;
        }
    }
    void RemotePublisher::processMessage()
    {
        constexpr size_t MAX_BATCH_SIZE = 10; // 添加批量大小限制
        std::vector<zmq::multipart_t> batchMessages;
        batchMessages.reserve(MAX_BATCH_SIZE); // 预分配内存

        while (m_bRunning)
        {
            {
                std::unique_lock<std::mutex> lock(m_mtxMsg);
                m_cvMsg.wait(lock, [this] { return !m_bRunning || !m_messages.empty(); });

                if (!m_bRunning)
                    return;

                // 限制每次处理的批量大小
                while (!m_messages.empty() && batchMessages.size() < MAX_BATCH_SIZE)
                {
                    batchMessages.push_back(std::move(m_messages.front()));
                    m_messages.pop();
                }
            }

            if (!batchMessages.empty())
            {
                batchSend(batchMessages);
                batchMessages.clear();
            }
        }
    }
    void RemotePublisher::batchSend(std::vector<zmq::multipart_t> &batchs)
    {
        try
        {
            for (auto &msg : batchs)
            {
                msg.send(*socket_);
            }
        }
        catch (const zmq::error_t &e)
        {
            LogError("RemotePublisher send error: {}", e.what());
        }
    }
} // namespace _Kits