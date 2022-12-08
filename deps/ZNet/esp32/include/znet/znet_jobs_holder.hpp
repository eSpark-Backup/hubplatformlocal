#pragma once

#include <znet/znet_job.hpp>

#include <queue>

namespace znet {

class JobsHolder {
public:
    JobsHolder() = default;

    JobsHolder( const JobsHolder& ) = delete;
    JobsHolder( JobsHolder&& ) = delete;

    ~JobsHolder();

    void setID( JobsHolderID id );

    JobsHolderID getID() const;

    /* @brief gets priority for current job context
     */
    JobPriorityOpt getCurrentJobPriority() const;

    /* @brief Requests job context lock with priority.
     *        Calls Job::onContextLocked immediatelly if possible or later ( after end of more urgent contextes ).
     */
    void lockJobContext( JobPriorityOpt priorityOpt, JobWPtr job );

    /* @brief Get weak job context. Check it as bool to know is current context still valid
     */
    WeakJobContext getJobContext() const;

private:
    void processQueue();

private:
    class SpecificJobContext;
    class CommonJobContext;
    using QueueItem = std::pair<JobPriority, JobWPtr>;
    struct ItemComparator {
        bool operator()( const QueueItem& a, const QueueItem& b ) const {
            return a.first < b.first || ( a.first == b.first && std::owner_less<JobWPtr>()( a.second, b.second ) );
        }
    };

    JobsHolderID m_id = 0;
    std::weak_ptr<SpecificJobContext> m_jobContext;
    std::vector<std::weak_ptr<CommonJobContext> > m_commonJobContexts;

    std::priority_queue<QueueItem, std::vector<QueueItem>, ItemComparator> m_jobs;
};

}  // namespace znet
