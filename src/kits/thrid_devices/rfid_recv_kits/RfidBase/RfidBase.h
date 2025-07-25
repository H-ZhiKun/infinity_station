#pragma once

#include <QString>
#include <yaml-cpp/yaml.h>
#include <QObject>
#include <QVariant>
/*
    RFID基类

    需要RFID tagId数据连接sendOutData信号即可
*/

namespace _Kits
{
    class RfidBase : public QObject
    {
        Q_OBJECT

      public:
        RfidBase() = default;
        virtual ~RfidBase() = default;

        virtual bool Init(const YAML::Node &) = 0;
        virtual bool Start() = 0;
        virtual bool Stop() = 0;
        virtual bool saveNaturalData(const QString &save_naturalData_path, const std::vector<uint8_t> &, const QString &);

      public slots:
        virtual void OnrecvData(const QByteArray &data) {};
        virtual void onisSaveNaturalData(bool) = 0;

      signals:
        void sendOutData(const QString &tagId);             // 接收数据的信号
        void sendRfidBackCheckData(const QByteArray &data); // 接收回查数据的信号
        void sendRfidNaturalData(const std::vector<uint8_t>, const QString);

      protected:
        // virtual bool saveNaturalData(const QString &save_naturalData_path, const std::vector<uint8_t> &,const QString &);
        bool _mb_isSaveNaturalData = false;
        QString _mstr_natural_data_path = "";
    };

} // namespace _Kits