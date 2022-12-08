#pragma once

#include <znet/enums/schedule_entry_lock.hpp>
#include <znet/znet.h>
#include <znet/znet_job.hpp>
#include <znet/znet_request_value.hpp>

#include <vector>
#include <unordered_set>

namespace znet {

class ZCc0x4EScheduleEntryLock;

class SchedulePollingHandler {
public:
    explicit SchedulePollingHandler( ZCc0x4EScheduleEntryLock& scheduleEntryLock );

    bool start( const std::unordered_set<znet_supported_users_t>& userIds,
                RequestValue::CallbackPtr callback,
                WeakJobContext context /* TODO: add RequestCallbackPtr here*/ );

    bool inProgress() const;

private:
    bool requestNextComponent( RequestValue::CallbackPtr callback,
                               WeakJobContext context,
                               std::shared_ptr<std::vector<uint8_t> > userIds,
                               ScheduleEntryLock::Type type = ScheduleEntryLock::Type::WeekDay,
                               uint8_t slot = 1 );

private:
    ZCc0x4EScheduleEntryLock& m_scheduleEntryLock;
    std::weak_ptr<std::vector<uint8_t> > m_userIds;
};

}  // namespace znet
