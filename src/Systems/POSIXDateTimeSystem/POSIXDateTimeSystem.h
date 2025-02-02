#pragma once

#include "Interface/DateTimeSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class POSIXDateTimeSystem
        : public ServiceBase<DateTimeSystemInterface>
    {
    public:
        POSIXDateTimeSystem();
        ~POSIXDateTimeSystem() override;

    protected:
        void getLocalDateTime( PlatformDateTime * const _dateTime ) const override;

    protected:
        TimeMilliseconds getLocalDateTimeMilliseconds() const override;

    protected:
        void getLocalDateTimeFromMilliseconds( TimeMilliseconds _time, PlatformDateTime * const _dateTime ) const override;

    protected:
        int32_t getTimeZoneOffset() const override;
    };
}
