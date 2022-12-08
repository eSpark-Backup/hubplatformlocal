#include "znet/znet_schedule_polling_handler.hpp"

#include "znet/cc_0x4e_schedule_entry_lock.hpp"
#include "znet/znet_node.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

SchedulePollingHandler::SchedulePollingHandler( ZCc0x4EScheduleEntryLock& scheduleEntryLock )
: m_scheduleEntryLock( scheduleEntryLock ) {}

bool SchedulePollingHandler::start( const std::unordered_set<znet_supported_users_t>& userIds,
                                    RequestValue::CallbackPtr callback,
                                    WeakJobContext context ) {
    if( inProgress() ) {
        ZNET_LOGW( "Scedule polling for node %u is in progress: new polling cannot be started\n",
                   m_scheduleEntryLock.getParentNode().getId() );
        return false;
    }

    if( m_scheduleEntryLock.getWeekDaySlots() == 0 && m_scheduleEntryLock.getYearDaySlots() == 0 &&
        m_scheduleEntryLock.getDailyRepeatingSlots() == 0 ) {
        return false;
    }

    auto ids = std::make_shared<std::vector<uint8_t> >();
    ids->reserve( userIds.size() );

    for( auto userId : userIds ) {
        if( userId <= 0xff ) {
            ids->push_back( static_cast<uint8_t>( userId ) );
        }
        else {
            ZNET_LOGW( "Schedule entry lock cc doesn't support 16bit user id" );
        }
    }

    m_userIds = ids;

    return !ids->empty() && requestNextComponent( std::move( callback ), context, std::move( ids ) );
}

bool SchedulePollingHandler::inProgress() const {
    return !m_userIds.expired();
}

bool SchedulePollingHandler::requestNextComponent( RequestValue::CallbackPtr callback,
                                                   WeakJobContext context,
                                                   std::shared_ptr<std::vector<uint8_t> > userIds,
                                                   ScheduleEntryLock::Type type,
                                                   uint8_t slotId ) {
    auto onFinished = [=]( RequestFailTypeOpt failType ) {
        if( callback && *callback ) {
            ( *callback )( failType );
        }
    };

    if( !userIds || userIds->empty() ) {
        onFinished( {} );
        return true;
    }

    const auto userId = userIds->back();

    auto getNextSlotRequestor = [&]() {
        return std::bind( [=]() {
            if( !requestNextComponent( callback, context, userIds, type, slotId + 1 ) ) {
                onFinished( RequestFailType::SendingError );
            }
        } );
    };

    auto setNextType = [&]( ScheduleEntryLock::Type nextType ) {
        type = nextType;
        slotId = 1;
    };

    if( type == ScheduleEntryLock::Type::WeekDay ) {
        if( slotId <= m_scheduleEntryLock.getWeekDaySlots() ) {
            return m_scheduleEntryLock.requestWeekDaySchedule(
                znet_schedule_id_t{ userId, slotId }, getNextSlotRequestor(), onFinished, context );
        }
        else {
            setNextType( ScheduleEntryLock::Type::YearDay );
        }
    }

    if( type == ScheduleEntryLock::Type::YearDay ) {
        if( slotId <= m_scheduleEntryLock.getYearDaySlots() ) {
            return m_scheduleEntryLock.requestYearDaySchedule(
                znet_schedule_id_t{ userId, slotId }, getNextSlotRequestor(), onFinished, context );
        }
        else {
            setNextType( ScheduleEntryLock::Type::DailyRepeating );
        }
    }

    if( type == ScheduleEntryLock::Type::DailyRepeating ) {
        if( slotId <= m_scheduleEntryLock.getDailyRepeatingSlots() ) {
            return m_scheduleEntryLock.requestDailyRepeatingSchedule(
                znet_schedule_id_t{ userId, slotId }, getNextSlotRequestor(), onFinished, context );
        }
        else {
            setNextType( ScheduleEntryLock::Type::WeekDay );
        }
    }

    userIds->pop_back();
    return requestNextComponent( callback, context, userIds );
}

}  // namespace znet
