#pragma once

#include <znet/iznet_date_time_provider.hpp>

namespace znet {

class DefaultDateTimeProvider : public virtual IDateTimeProvider {
public:
    DateTime getUtcDateTime() const override;

    DateTime getLocalDateTime() const override;

    TimeZoneSettings getTimeZoneSettings() const override;

    void setTimeZoneSettings( const char* zoneName, const char* zone ) override;
};

}  // namespace znet
