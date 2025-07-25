#include "RemoteSubscriber.h"
#include "kits/common/log/CRossLogger.h"
#include "tis_global/Struct.h"
namespace _Kits
{
    bool RemoteSubscriber::initialize(const std::string &protocol,
                                      const std::string &ip,
                                      const std::string &port,
                                      int timeout,
                                      int hwm,
                                      const std::vector<std::string> &topics)
    {
        try
        {
            setupSocket(ZMQ_SUB);
            parseConfig(false, protocol, ip, port, timeout, hwm);

            int linger = 0;
            socket_->set(zmq::sockopt::linger, linger);

            m_topics = topics;

            for (const auto &topic : m_topics)
            {
                socket_->set(zmq::sockopt::subscribe, topic);
            }

            socket_->connect(address_);
            connected_ = true;

            m_bRunning = true;
            m_thWorker = std::thread(&RemoteSubscriber::messageLoop, this);

            return true;
        }
        catch (const std::exception &e)
        {
            LogError("RemoteSubscriber initialize error: {}", e.what());
            return false;
        }
    }

    void RemoteSubscriber::stop()
    {
        m_bRunning = false;
        if (m_thWorker.joinable())
        {
            m_thWorker.join();
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

    void RemoteSubscriber::registerCallback(const std::string &topic, RemoteMessageCallback callback)
    {
        std::lock_guard<std::mutex> lock(m_mtxCallback);
        m_callbacks[topic] = callback;
        socket_->set(zmq::sockopt::subscribe, topic);
    }

    void RemoteSubscriber::unregisterCallback(const std::string &topic)
    {
        std::lock_guard<std::mutex> lock(m_mtxCallback);
        m_callbacks.erase(topic);
    }

    void RemoteSubscriber::messageLoop()
    {
        while (m_bRunning)
        {
            try
            {
                zmq::multipart_t multipart(*socket_);

                if (multipart.size() == 4)
                {
                    TIS_Info::RemoteMessage msg;
                    msg.topic = multipart[0].to_string();
                    msg.message_id = multipart[1].to_string();
                    msg.json_data = multipart[2].to_string();

                    const auto &binary_part = multipart[3];
                    msg.binary_data.assign(static_cast<const uint8_t *>(binary_part.data()),
                                           static_cast<const uint8_t *>(binary_part.data()) + binary_part.size());

                    std::lock_guard<std::mutex> lock(m_mtxCallback);
                    auto it = m_callbacks.find(msg.topic);
                    if (it != m_callbacks.end())
                    {
                        it->second(msg);
                    }
                }
            }
            catch (const zmq::error_t &e)
            {
                if (e.num() != ETERM)
                {
                    LogError("RemoteSubscriber receive error: {}", e.what());
                }
                break;
            }
            catch (const std::exception &e)
            {
                LogError("RemoteSubscriber message error: {}", e.what());
            }
        }
    }
} // namespace _Kits