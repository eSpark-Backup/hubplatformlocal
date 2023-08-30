#include "fake_timer_provider.h"
#include <vector>

namespace znet {
namespace ut {

class FakeTimerProvider::FakeTimer : public ITimerProvider::Timer {
public:
    struct Data {
        Data( FakeTimerProvider& owner, uint32_t index, ITimerProvider::TimerCallback cb )
        : m_owner( owner )
        , m_index( index )
        , m_callback( std::move( cb ) ) {}

        FakeTimerProvider& m_owner;
        uint32_t m_index;
        std::chrono::system_clock::time_point m_nextTimePoint;
        TimerInterval m_interval;
        bool m_isRun = false;
        ITimerProvider::TimerCallback m_callback;
    };

    using TimerInterval = ITimerProvider::TimerInterval;
    FakeTimer( FakeTimerProvider& owner, uint32_t index, ITimerProvider::TimerCallback cb )
    : m_data( std::make_shared<Data>( owner, index, std::move( cb ) ) ) {
        m_data->m_owner.m_timers.emplace( index, *this );
    }

    ~FakeTimer() override {
        auto data = std::move( m_data );
        data->m_owner.m_timers.erase( data->m_index );
    }

    void runPeriodically( TimerInterval first, TimerInterval interval ) override {
        m_data->m_nextTimePoint = m_data->m_owner.m_timePoint + first;
        m_data->m_interval = std::max( TimerInterval( 1 ), interval );
        m_data->m_isRun = true;
    }
    void runOnce( TimerInterval first ) override {
        m_data->m_nextTimePoint = m_data->m_owner.m_timePoint + first;
        m_data->m_interval = {};
        m_data->m_isRun = true;
    }

    void onTimePointUpdated() {
        auto data = m_data;
        while( data.use_count() > 1 && data->m_nextTimePoint <= data->m_owner.m_timePoint && data->m_isRun ) {
            data->m_isRun = data->m_interval.count() != 0;
            data->m_callback();
            data->m_nextTimePoint += data->m_interval;
        }
    }

private:
    std::shared_ptr<Data> m_data;
};

FakeTimerProvider::TimerUPtr FakeTimerProvider::createTimer( TimerCallback cb ) {
    return TimerUPtr( new FakeTimer( *this, m_index++, std::move( cb ) ) );
}

void FakeTimerProvider::addTime( TimerInterval interval ) {
    const auto nextPoint = m_timePoint + interval;
    do {
        m_timePoint = std::min( m_timePoint + std::chrono::milliseconds( 10 ), nextPoint );
        std::vector<uint32_t> indexes;
        indexes.reserve( m_timers.size() );

        for( auto& indexToTimer : m_timers ) {
            indexes.push_back( indexToTimer.first );
        }

        for( auto index : indexes ) {
            auto it = m_timers.find( index );
            if( it != m_timers.end() ) {
                it->second.onTimePointUpdated();
            }
        }
    } while( m_timePoint != nextPoint );
}

}  // namespace ut
}  // namespace znet
