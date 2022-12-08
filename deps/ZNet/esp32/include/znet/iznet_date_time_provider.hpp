#pragma once

#include <znet/znet.h>

namespace znet {

class IDateTimeProvider {
public:
    using DateTime = znet_date_time_t;
    using TimeZoneSettings = znet_time_zone_settings_t;

public:
    virtual ~IDateTimeProvider() = default;

    virtual DateTime getUtcDateTime() const = 0;

    virtual DateTime getLocalDateTime() const = 0;

    virtual TimeZoneSettings getTimeZoneSettings() const = 0;

    virtual void setTimeZoneSettings( const char* zoneName, const char* zone ) = 0;
};

}  // namespace znet
