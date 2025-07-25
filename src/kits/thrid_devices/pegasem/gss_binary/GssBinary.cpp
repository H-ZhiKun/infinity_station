#include "GssBinary.h"
#include "kits/common/log/CRossLogger.h"

_Kits::GssBinary::GssBinary()
{
}

_Kits::GssBinary::~GssBinary()
{
}

bool _Kits::GssBinary::start(const YAML::Node &config)
{
    try
    {
        if (isOpen())
        {
            stop();
        }

        auto portName = config["com"].as<std::string>();
        auto baudRate = config["baudRate"].as<int>();
        auto dataBits = config["dataBits"].as<int>();
        auto stopBits = config["stopBits"].as<int>();
        auto parity = config["parity"].as<int>();

        _m_port.setPortName(QString::fromStdString(portName));
        _m_port.setBaudRate(QSerialPort::BaudRate(baudRate));
        _m_port.setDataBits(QSerialPort::DataBits(dataBits));
        _m_port.setStopBits(QSerialPort::StopBits(stopBits));
        _m_port.setParity(QSerialPort::Parity(parity));
        _m_port.setFlowControl(QSerialPort::NoFlowControl);
    }
    catch (YAML::Exception &e)
    {
        _Kits::LogError("GssBinary start failed: {}", e.what());
        return false;
    }

    return true;
}

bool _Kits::GssBinary::stop()
{
    _m_port.close();
    return true;
}

bool _Kits::GssBinary::isOpen()
{
    return _m_port.isOpen();
}