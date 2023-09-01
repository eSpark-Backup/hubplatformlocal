#include <znet/converters/time_zones_converter.hpp>

#include <cassert>
#include <cstring>

namespace znet {
namespace time_zones_converter {

struct TimeZonesData {
    int16_t hoursTzo;
    const char* const name;
    const char* const zone;
};

static const TimeZonesData timeZonesData[] = { { 0, "Atlantic/Reykjavik", "GMT0" },

                                               { 1, "Europe/Dublin", "IST-1GMT0,M10.5.0,M3.5.0/1" },
                                               { 2, "Europe/Amsterdam", "CET-1CEST,M3.5.0,M10.5.0/3" },
                                               { 3, "Europe/Kiev", "EET-2EEST,M3.5.0/3,M10.5.0/4" },
                                               { 4, "Asia/Dubai", "<-04>4" },
                                               { 5, "Indian/Maldives", "<-05>5" },
                                               { 6, "Asia/Bishkek", "<-06>6" },
                                               { 7, "Asia/Bangkok", "<-07>7" },
                                               { 8, "Asia/Hong_Kong", "HKT-8" },
                                               { 9, "Asia/Tokyo", "JST-9" },
                                               { 10, "Australia/Brisbane", "AEST-10" },
                                               { 11, "Australia/Sydney", "AEST-10AEDT,M10.1.0,M4.1.0/3" },
                                               { 12, "Pacific/Fiji", "<+12>-12<+13>,M11.2.0,M1.2.3/99" },
                                               { 13, "Antarctica/McMurdo", "NZST-12NZDT,M9.5.0,M4.1.0/3" },
                                               { 14, "Pacific/Apia", "<+13>-13<+14>,M9.5.0/3,M4.1.0/4" },

                                               { -1, "Atlantic/Cape_Verde", "<-01>1" },
                                               { -2, "America/Miquelon", "<-03>3<-02>,M3.2.0,M11.1.0" },
                                               { -3, "America/Buenos_Aires", "<+03>-3" },
                                               { -4, "America/Detroit", "EST5EDT,M3.2.0,M11.1.0" },
                                               { -5, "America/Chicago", "CST6CDT,M3.2.0,M11.1.0" },
                                               { -6, "America/Denver", "MST7MDT,M3.2.0,M11.1.0" },
                                               { -7, "America/Vancouver", "PST8PDT,M3.2.0,M11.1.0" },
                                               { -8, "America/Nome", "AKST9AKDT,M3.2.0,M11.1.0" },
                                               { -9, "America/Adak", "HST10HDT,M3.2.0,M11.1.0" },
                                               { -10, "Pacific/Honolulu", "HST10" },
                                               { -11, "Pacific/Midway", "SST11" } };

const std::pair<const char*, const char*> getTimeZone( int16_t hoursTzo ) {
    for( const auto& data : timeZonesData ) {
        if( data.hoursTzo == hoursTzo ) {
            return std::make_pair( data.name, data.zone );
        }
    }

    return std::make_pair( "", "" );
}

}  // namespace time_zones_converter
}  // namespace znet
