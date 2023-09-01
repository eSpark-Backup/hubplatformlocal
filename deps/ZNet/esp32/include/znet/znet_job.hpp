#pragma once

#include <memory>
#include <utility>

namespace znet {

using JobsHolderID = uint8_t;
using JobPriority = uint8_t;
using JobPriorityOpt = std::pair<JobPriority, bool>;

constexpr const JobPriority DefaultJobPriority = 1;
constexpr const JobPriority MaxJobPriority = 255;

class JobContext {
public:
    virtual ~JobContext() = default;
    virtual bool isExpired() const = 0;
};

using JobContextPtr = std::shared_ptr<JobContext>;
using JobContextWPtr = std::weak_ptr<JobContext>;

class WeakJobContext {
public:
    WeakJobContext( JobContextWPtr context )
    : m_context( context )
    , m_useContext( true ) {}

    WeakJobContext()
    : m_useContext( false ) {}

    operator bool() const {
        auto context = m_context.lock();
        return !m_useContext || ( context && !context->isExpired() );
    }

private:
    JobContextWPtr m_context;
    bool m_useContext;
};

class Job {
public:
    virtual ~Job() = default;

    virtual void onContextLocked( JobsHolderID id, JobContextPtr context ) = 0;
    virtual void onContextExpired( JobsHolderID id ) = 0;
};

using JobPtr = std::shared_ptr<Job>;
using JobWPtr = std::weak_ptr<Job>;

}  // namespace znet
