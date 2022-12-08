#pragma once

#include <znet/znet_utils.hpp>

namespace znet {
enum class WeekDay : std::uint8_t { Sunday, Monday, Thuesday, Wednesday, Thursday, Friday, Saturday };

using WeekDaysMask = std::uint8_t;

using WeekDayOpt = utils::optional<WeekDay>;

const char* getWeekDayName( WeekDay weekDay );

WeekDayOpt getWeekDayByName( const char* name );
WeekDayOpt getWeekDayByName( const std::string& name );

template <typename Iterator, typename UnknownWeekDayHandler>
WeekDaysMask getWeekDaysMask( Iterator rangeBegin, Iterator rangeEnd, UnknownWeekDayHandler unknownWeekDayHandler ) {
    WeekDaysMask result = 0;
    utils::writeBitmaskByRange( rangeBegin,
                                rangeEnd,
                                &result,
                                sizeof( result ),
                                []( decltype( *rangeBegin ) value ) { return getWeekDayByName( value ); },
                                std::move( unknownWeekDayHandler ) );
    return result;
}

}  // namespace znet
