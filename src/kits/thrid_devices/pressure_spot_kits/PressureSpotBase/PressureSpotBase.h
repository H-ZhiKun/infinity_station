#pragma once

#include <QObject>

namespace _Kits
{
    class PressureSpotBase : public QObject
    {
        Q_OBJECT

    public:
        PressureSpotBase()=default;
        virtual ~PressureSpotBase() noexcept;

        virtual bool Init() = 0;
        virtual bool Start() = 0;
        virtual bool Stop() = 0;

    protected slots:

    signals:

    };

} // namespace _Kits