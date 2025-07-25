#pragma once

#include <QObject>
#include "kits/common/log/CRossLogger.h"
#include <QSerialPort>
#include <yaml-cpp/yaml.h>

namespace _Kits
{
    class GssBinary : public QObject
    {
        Q_OBJECT

      public:
        GssBinary();
        ~GssBinary();

        bool start(const YAML::Node &config);
        bool stop();
        bool isOpen();

      signals:
        void recvData(QByteArray);

      private:
        QSerialPort _m_port;

        inline static const std::string START_CMD = "A\r";
        inline static const std::string BIN_CMD = "@B\r";
    };
} // namespace _Kits