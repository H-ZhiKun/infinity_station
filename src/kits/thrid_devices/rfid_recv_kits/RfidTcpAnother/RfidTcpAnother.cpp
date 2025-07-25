#include "RfidTcpAnother.h"
#include <vector>
#include <string>
#include <cstring>
#include <sstream>

_Kits::RfidTcp::RfidTcp()
    : m_pTcpClient(nullptr), mqui_port(0)
{
}

_Kits::RfidTcp::~RfidTcp()
{
}

bool _Kits::RfidTcp::Init(const YAML::Node & config)
{
    if (config["host"].IsDefined() && config["port"].IsDefined())
    {
        mqstr_host = config["host"].as<std::string>().c_str();
        mqui_port = config["port"].as<quint16>();
        m_pTcpClient = std::make_unique<TcpClient>(mqstr_host, mqui_port);
    }
    else
    {
        return false;
    }

    return true;
}

bool _Kits::RfidTcp::Start()
{
    if (!m_pTcpClient)
    {
        return false;
    }

    // 连接信号槽
    connect(m_pTcpClient.get(), &TcpClient::recvData,
            this, &RfidTcp::OnrecvData);

    return true;
}

bool _Kits::RfidTcp::Stop()
{
    if (m_pTcpClient)
    {
        m_pTcpClient.release();
    }

    return true;
}

void _Kits::RfidTcp::OnrecvData(const QByteArray &data)
{
    // 使用vector替代原始数组
    std::vector<uint8_t> pBuf(128, 0);
    std::copy(data.begin(), data.end(), pBuf.begin());
    if (!data.isEmpty()) {
        pBuf[data.size()] = '\0'; // 安全设置结束符
    }

    // 转换为标准字符串
    const std::string strRecvd(reinterpret_cast<char*>(pBuf.data()));

    if (strRecvd.rfind("EVNTTAG", 0) == 0) { // 标准starts_with语义
        std::vector<uint8_t> databuf;
        size_t i = 25;
        
        while (i < strRecvd.size() - 1) {
            if (strRecvd[i] == '%' && i+2 < strRecvd.size()) {
                // 使用标准库解析十六进制
                uint8_t value;
                std::stringstream ss;
                ss << std::hex << strRecvd.substr(i+1, 2);
                ss >> value;
                databuf.push_back(value);
                i += 3;
            } else {
                databuf.push_back(strRecvd[i++]);
            }
        }

        // 转换标签ID
        int tagid = marktoint(databuf.data());
        std::string strTagid = std::to_string(tagid);
        
        
        emit sendOutData(strTagid);
    }
    else if (strRecvd.rfind("RPLYHELO", 0) == 0) {
        //if (m_startEvent) m_startEvent->SetEvent();
    
        // ???不知道是什么目的
    }
    else {
        // 使用标准日志接口
        // logger->info("Unparsed packet: {}", strRecvd);
    }
}

int _Kits::RfidTcp::marktoint(const uint8_t* mark)
{
    return static_cast<int>(
        (mark[1] & 0x3f) << 22 | 
        (mark[2]  << 14) | 
        (mark[3]  << 6)  | 
        (mark[4] & 0xfc) >> 2
    );
}