#pragma once
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <string>
#include <memory>
namespace _Kits
{
    class RemoteBase
    {
      public:
        virtual ~RemoteBase() = default;

        bool isConnected() const
        {
            return connected_;
        }

      protected:
        std::atomic_bool connected_{false};
        std::unique_ptr<zmq::context_t> context_;
        std::unique_ptr<zmq::socket_t> socket_;
        std::string address_;
        std::string protocol_;
        int socket_type_;

        void setupSocket(int type)
        {
            context_ = std::make_unique<zmq::context_t>(1);
            socket_ = std::make_unique<zmq::socket_t>(*context_, type);
            socket_type_ = type;
        }

        void parseConfig(bool bIsPub, const std::string &protocol, const std::string &ip, const std::string &port, int timeout, int hwm)
        {
            protocol_ = protocol;
            address_ = protocol_ + "://" + ip + ":" + port;

            // 设置socket选项
            if (bIsPub)
            {
                socket_->set(zmq::sockopt::sndtimeo, timeout);
                socket_->set(zmq::sockopt::sndhwm, hwm);
            }
            else
            {
                socket_->set(zmq::sockopt::rcvtimeo, timeout);
                socket_->set(zmq::sockopt::rcvhwm, hwm);
            }
        }
    };
} // namespace _Kits