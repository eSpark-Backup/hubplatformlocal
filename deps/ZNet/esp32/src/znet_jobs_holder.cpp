#include <znet/znet_jobs_holder.hpp>

#include <algorithm>
#include <functional>
#include <cassert>

namespace znet {

class JobsHolder::CommonJobContext : public JobContext {
public:
    CommonJobContext( JobsHolder& jobsHolder, JobWPtr jobWPtr )
    : m_jobsHolder( jobsHolder )
    , m_jobWPtr( jobWPtr ) {}

    bool isExpired() const override {
        return m_isExpired;
    }

    JobWPtr getJob() {
        return m_jobWPtr;
    }

    void setExpired() {
        if( !m_isExpired ) {
            m_isExpired = true;
            if( auto job = getJob().lock() ) {
                job->onContextExpired( m_jobsHolder.getID() );
            }
        }
    }

protected:
    JobsHolder& m_jobsHolder;
    JobWPtr m_jobWPtr;
    bool m_isExpired = false;
};

class JobsHolder::SpecificJobContext : public JobsHolder::CommonJobContext {
public:
    SpecificJobContext( JobsHolder& jobsHolder, const QueueItem& item )
    : CommonJobContext( jobsHolder, item.second )
    , m_jobPriority( item.first ) {}

    JobPriority getPriority() const {
        return m_jobPriority;
    }

    ~SpecificJobContext() override {
        if( !m_isExpired ) {
            assert( !m_jobsHolder.getJobContext() );
            m_jobsHolder.processQueue();
        }
    }

private:
    JobPriority m_jobPriority;
};

JobsHolder::~JobsHolder() {
    if( auto context = m_jobContext.lock() ) {
        context->setExpired();
    }

    for( auto contextWPtr : m_commonJobContexts ) {
        if( auto context = contextWPtr.lock() ) {
            context->setExpired();
        }
    }
}

void JobsHolder::setID( JobsHolderID id ) {
    m_id = id;
}

JobsHolderID JobsHolder::getID() const {
    return m_id;
}

JobPriorityOpt JobsHolder::getCurrentJobPriority() const {
    if( auto context = m_jobContext.lock() ) {
        return { context->getPriority(), true };
    }

    return { 0, false };
}

void JobsHolder::lockJobContext( JobPriorityOpt priorityOpt, JobWPtr job ) {
    assert( !job.expired() );

    if( priorityOpt.second ) {
        m_jobs.emplace( priorityOpt.first, job );
        processQueue();
    }
    else {
        auto context = std::make_shared<CommonJobContext>( *this, job );
        m_commonJobContexts.push_back( context );
        job.lock()->onContextLocked( getID(), context );

        context.reset();

        auto isExpired = []( std::weak_ptr<CommonJobContext> context ) { return context.expired(); };

        m_commonJobContexts.erase( std::remove_if( m_commonJobContexts.begin(), m_commonJobContexts.end(), isExpired ),
                                   m_commonJobContexts.end() );
    }
}

WeakJobContext JobsHolder::getJobContext() const {
    return WeakJobContext{ m_jobContext };
}

void JobsHolder::processQueue() {
    if( m_jobs.empty() ) {
        return;
    }

    if( auto context = m_jobContext.lock() ) {
        if( context->isExpired() ) {
            return;  // recursive call from onContextExpired
        }

        if( context->getPriority() >= m_jobs.top().first ) {
            return;
        }

        m_jobs.emplace( context->getPriority(), context->getJob() );
        context->setExpired();
        m_jobContext.reset();
    }

    while( !m_jobs.empty() && m_jobs.top().second.expired() ) {
        m_jobs.pop();
    }

    if( m_jobs.empty() ) {
        return;
    }

    auto context = std::make_shared<SpecificJobContext>( *this, m_jobs.top() );

    m_jobContext = context;
    m_jobs.pop();

    auto job = context->getJob().lock();
    job->onContextLocked( getID(), std::move( context ) );
}

}  // namespace znet
