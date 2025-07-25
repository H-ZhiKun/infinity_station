#pragma once

#include <QObject>

namespace _Kits
{

    class speedBase : public QObject
    {
        Q_OBJECT

        public:
            explicit speedBase(QObject *parent = nullptr);
            virtual ~speedBase();
            virtual std::vector<uint32_t> parseDynamicCounterResponse(const std::vector<uint8_t>& response, int channelCount);

    };

} // namespace _Kits