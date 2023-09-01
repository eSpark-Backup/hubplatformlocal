#include <znet/znet_week_day.hpp>

namespace znet {

namespace {
const std::initializer_list<const char* const> weekDaysList = { "sunday",   "monday", "tuesday", "wednesday",
                                                                "thursday", "friday", "saturday" };

const utils::RangedList<WeekDay, const char*> weekDaysRangedList{ { WeekDay::Sunday, weekDaysList } };
}  // namespace

const char* getWeekDayName( WeekDay weekDay ) {
    auto result = utils::findInRangedList( weekDaysRangedList, weekDay );
    return result ? *result : nullptr;
}

WeekDayOpt getWeekDayByName( const char* name ) {
    return utils::findIndexInRangedList( weekDaysRangedList,
                                         [name]( const char* weekName ) { return strcmp( name, weekName ) == 0; } );
}

WeekDayOpt getWeekDayByName( const std::string& name ) {
    return getWeekDayByName( name.c_str() );
}

}  // namespace znet
