#pragma once

#include <znet/znet_command_class.hpp>

#include <znet/enums/schedule_entry_lock.hpp>
#include <znet/znet_schedule_polling_handler.hpp>
#include <znet/znet_job.hpp>

#include <chrono>

namespace znet {

class ZCc0x4EScheduleEntryLock final : public ZClass {
public:
    using WeekDayCallback = std::function<void( const znet_week_day_schedule_t& )>;
    using YearDayCallback = std::function<void( const znet_year_day_schedule_t& )>;
    using DailyRepeatingCallback = std::function<void( const znet_daily_repeating_schedule_t& )>;
    using TimeZoneCallback = std::function<void( const znet_cmd_schedule_entry_lock_time_offset_t& )>;

public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_SCHEDULE_ENTRY_LOCK>;
    explicit ZCc0x4EScheduleEntryLock( ZChannel& parent, znet_command_class_version_t version = 1 );

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool requestValue( const RequestValue& request ) override;

    bool storeData( IFile& file ) const override;

    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT()

public:
    bool requestWeekDaySchedule( const znet_schedule_id_t& scheduleId,
                                 WeekDayCallback onSuccess = nullptr,
                                 OnFailCallback onFail = nullptr,
                                 WeakJobContext context = {} ) const;

    bool requestYearDaySchedule( const znet_schedule_id_t& scheduleId,
                                 YearDayCallback onSuccess = nullptr,
                                 OnFailCallback onFail = nullptr,
                                 WeakJobContext context = {} ) const;

    bool requestDailyRepeatingSchedule( const znet_schedule_id_t& scheduleId,
                                        DailyRepeatingCallback onSuccess = nullptr,
                                        OnFailCallback onFail = nullptr,
                                        WeakJobContext context = {} ) const;

    bool requestTimeZone( TimeZoneCallback onSuccess, OnFailCallback onFail, WeakJobContext context ) const;

public:
    void setTimeOffset( const znet_cmd_schedule_entry_lock_time_offset_t& timeOffset );

    bool setWeekDaySchedule( const znet_week_day_schedule_t& schedule );

    bool removeWeekDaySchedule( const znet_schedule_id_t& scheduleId );

    bool setYearDaySchedule( const znet_year_day_schedule_t& schedule );

    bool removeYearDaySchedule( const znet_schedule_id_t& scheduleId );

    bool setDailyRepeatingSchedule( const znet_daily_repeating_schedule_t& schedule );

    bool removeDailyRepeatingSchedule( const znet_schedule_id_t& scheduleId );

    bool isPolling() const;

    bool isTimeZoneSupported() const;

    uint8_t getWeekDaySlots() const;

    uint8_t getYearDaySlots() const;

    uint8_t getDailyRepeatingSlots() const;

private:
    using ScheduleAction = uint8_t;

private:
    bool requestAllSlots( RequestValue::CallbackPtr callbackPtr, WeakJobContext context ) const;

    bool applySchedule( ScheduleAction action, const znet_week_day_schedule_t& schedule );

    bool applySchedule( ScheduleAction action, const znet_year_day_schedule_t& schedule );

    bool applySchedule( ScheduleAction action, const znet_daily_repeating_schedule_t& schedule );

private:
    static constexpr ScheduleAction EraseSchedule{ 0 };
    static constexpr ScheduleAction ModifySchedule{ 1 };

private:
    mutable SchedulePollingHandler m_pollingHandler;

    uint8_t m_weekDaySlots{ 0 };
    uint8_t m_yearDaySlots{ 0 };
    uint8_t m_dailyRepeatingSlots{ 0 };
};

}  // namespace znet
