#include "znet/cc_0x4e_schedule_entry_lock.hpp"

#include "znet/znet_node.hpp"
#include "znet/cc_0x63_user_code.hpp"
#include "znet/znet_requests_manager.hpp"

#include "znet_schedule_helper.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

ZCc0x4EScheduleEntryLock::ZCc0x4EScheduleEntryLock( ZChannel& parent, znet_command_class_version_t version )
: ZClass( parent, version )
, m_pollingHandler( *this ) {}

bool ZCc0x4EScheduleEntryLock::interview( InterviewCallback callback, WeakJobContext context ) {
    return singleInterviewRequestImpl( *this,
                                       ScheduleEntryLockSupportedRequest( getNodeId(), getChannelId() ),
                                       context,
                                       [this]( const znet_cmd_schedule_entry_lock_supported_report_t& report ) {
                                           m_weekDaySlots = report.week_day_slots;
                                           m_yearDaySlots = report.year_day_slots;
                                           m_dailyRepeatingSlots = report.daily_repeating_slots;
                                       },
                                       std::move( callback ) );
}

bool ZCc0x4EScheduleEntryLock::requestValue( const RequestValue& requestValue ) {
    return requestAllSlots(
        requestValue.resultCallback ? std::make_shared<RequestValue::Callback>( requestValue.resultCallback ) : nullptr,
        requestValue.jobContext );
}

bool ZCc0x4EScheduleEntryLock::requestWeekDaySchedule( const znet_schedule_id_t& scheduleId,
                                                       WeekDayCallback onSuccess,
                                                       OnFailCallback onFail,
                                                       WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest(
        WeekDayScheduleRequest( getNodeId(), getChannelId(), scheduleId.user_id, scheduleId.slot_id ),
        context,
        std::move( onSuccess ),
        std::move( onFail ),
        std::max( getParentNode().getRequestDelay(), DefaultPostponedRequestDelay ) );
}

bool ZCc0x4EScheduleEntryLock::requestYearDaySchedule( const znet_schedule_id_t& scheduleId,
                                                       YearDayCallback onSuccess,
                                                       OnFailCallback onFail,
                                                       WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest(
        YearDaySchduleRequest( getNodeId(), getChannelId(), scheduleId.user_id, scheduleId.slot_id ),
        context,
        std::move( onSuccess ),
        std::move( onFail ),
        std::max( getParentNode().getRequestDelay(), DefaultPostponedRequestDelay ) );
}

bool ZCc0x4EScheduleEntryLock::requestDailyRepeatingSchedule( const znet_schedule_id_t& scheduleId,
                                                              DailyRepeatingCallback onSuccess,
                                                              OnFailCallback onFail,
                                                              WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest(
        DailyRepeatingScheduleRequest( getNodeId(), getChannelId(), scheduleId.user_id, scheduleId.slot_id ),
        context,
        std::move( onSuccess ),
        std::move( onFail ),
        std::max( getParentNode().getRequestDelay(), DefaultPostponedRequestDelay ) );
}

bool ZCc0x4EScheduleEntryLock::requestTimeZone( TimeZoneCallback onSuccess,
                                                OnFailCallback onFail,
                                                WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest(
        ScheduleTimeZoneRequest( getNodeId(), getChannelId() ), context, std::move( onSuccess ), std::move( onFail ) );
}

bool ZCc0x4EScheduleEntryLock::setWeekDaySchedule( const znet_week_day_schedule_t& schedule ) {
    return applySchedule( ModifySchedule, schedule );
}

bool ZCc0x4EScheduleEntryLock::removeWeekDaySchedule( const znet_schedule_id_t& scheduleId ) {
    znet_week_day_schedule_t schedule{ .schedule_id = scheduleId };

    return applySchedule( EraseSchedule, schedule );
}

bool ZCc0x4EScheduleEntryLock::setYearDaySchedule( const znet_year_day_schedule_t& schedule ) {
    return applySchedule( ModifySchedule, schedule );
}

bool ZCc0x4EScheduleEntryLock::removeYearDaySchedule( const znet_schedule_id_t& scheduleId ) {
    const znet_year_day_schedule_t schedule{ .schedule_id = scheduleId };

    return applySchedule( EraseSchedule, schedule );
}

bool ZCc0x4EScheduleEntryLock::setDailyRepeatingSchedule( const znet_daily_repeating_schedule_t& schedule ) {
    return applySchedule( ModifySchedule, schedule );
}

bool ZCc0x4EScheduleEntryLock::removeDailyRepeatingSchedule( const znet_schedule_id_t& scheduleId ) {
    const znet_daily_repeating_schedule_t schedule{ .schedule_id = scheduleId };

    return applySchedule( EraseSchedule, schedule );
}

void ZCc0x4EScheduleEntryLock::setTimeOffset( const znet_cmd_schedule_entry_lock_time_offset_t& timeOffset ) {
    znet_node_cc_schedule_entry_lock_time_offset_set( getNodeId(), getChannelId(), &timeOffset );
}

bool ZCc0x4EScheduleEntryLock::isPolling() const {
    return m_pollingHandler.inProgress();
}

bool ZCc0x4EScheduleEntryLock::isTimeZoneSupported() const {
    return getVersion() > 1;
}

uint8_t ZCc0x4EScheduleEntryLock::getWeekDaySlots() const {
    return m_weekDaySlots;
}

uint8_t ZCc0x4EScheduleEntryLock::getYearDaySlots() const {
    return m_yearDaySlots;
}

uint8_t ZCc0x4EScheduleEntryLock::getDailyRepeatingSlots() const {
    return m_dailyRepeatingSlots;
}

bool ZCc0x4EScheduleEntryLock::storeData( IFile& file ) const {
    bool result = file.write( &m_weekDaySlots, sizeof( m_weekDaySlots ) );
    result &= file.write( &m_yearDaySlots, sizeof( m_yearDaySlots ) );
    result &= file.write( &m_dailyRepeatingSlots, sizeof( m_dailyRepeatingSlots ) );

    return result;
}

bool ZCc0x4EScheduleEntryLock::restoreData( IFile& file, int version ) {
    bool result = true;
    if( version >= majorFormatVersion_v3 ) {
        result = file.read( &m_weekDaySlots, sizeof( m_weekDaySlots ) );
        result &= file.read( &m_yearDaySlots, sizeof( m_yearDaySlots ) );
        result &= file.read( &m_dailyRepeatingSlots, sizeof( m_dailyRepeatingSlots ) );
    }
    return result;
}

bool ZCc0x4EScheduleEntryLock::requestAllSlots( RequestValue::CallbackPtr callbackPtr, WeakJobContext context ) const {
    const auto userCodeCc = getParentNode().getCommandClass<ZCc0x63UserCode>( getChannelId() );
    assert( userCodeCc );
    if( !userCodeCc )
        return false;

    return m_pollingHandler.start( userCodeCc->getUserIdsCache().getUserIds(), std::move( callbackPtr ), context );
}

bool ZCc0x4EScheduleEntryLock::applySchedule( ScheduleAction action, const znet_week_day_schedule_t& schedule ) {
    return ZNET_ERR_OK ==
           znet_node_cc_schedule_entry_lock_week_day_set( getNodeId(), getChannelId(), action, &schedule );
}

bool ZCc0x4EScheduleEntryLock::applySchedule( ScheduleAction action, const znet_year_day_schedule_t& schedule ) {
    const auto adjustedSchedule = ScheduleHelper::adjustSchedule( schedule, getParentNode().getTimeOffset() );

    return ZNET_ERR_OK ==
           znet_node_cc_schedule_entry_lock_year_day_set( getNodeId(), getChannelId(), action, &adjustedSchedule );
}

bool ZCc0x4EScheduleEntryLock::applySchedule( ScheduleAction action, const znet_daily_repeating_schedule_t& schedule ) {
    const auto adjustedSchedule = ScheduleHelper::adjustSchedule( schedule, getParentNode().getTimeOffset() );

    return ZNET_ERR_OK == znet_node_cc_schedule_entry_lock_daily_repeating_set(
                              getNodeId(), getChannelId(), action, &adjustedSchedule );
}

}  // namespace znet
