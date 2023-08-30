#include <znet/znet_default_timer_provider.hpp>

extern "C" {
#include <znet_timer.h>
}

#include <cassert>
#include <algorithm>

namespace znet {

namespace {
class DefaultTimer : public ITimerProvider::Timer {
public:
    using Interval = ITimerProvider::TimerInterval;

    DefaultTimer( ITimerProvider::TimerCallback&& cb )
    : m_callback( std::move( cb ) ) {
        assert( m_callback );
    }

    void runPeriodically( Interval first, Interval interval ) override {
        m_interval = fixInterval( interval );
        runOnceImpl( fixInterval( first ) );
    }

    void runOnce( Interval first ) override {
        m_interval = Interval( 0 );
        runOnceImpl( fixInterval( first ) );
    }

private:
    static Interval fixInterval( const Interval& interval ) {
        return std::max( interval, Interval( 1 ) );
    }

    void runOnceImpl( Interval first ) {
        assert( first.count() );
        m_znetTimer.reset( znet_timer_shoot( static_cast<uint64_t>( first.count() ), &onTimerEvent, this ) );
    }

    void operator()() {
        m_znetTimer.release();

        if( m_interval.count() ) {
            runOnceImpl( m_interval );
        }

        m_callback();
    }

    static void onTimerEvent( znet_timer_t* znetTimer, void* data, int isDestroy ) {
        if( !isDestroy ) {
            assert( data );
            auto& timer = *static_cast<DefaultTimer*>( data );
            assert( znetTimer == timer.m_znetTimer.get() );
            timer();
        }
    }

private:
    std::unique_ptr<znet_timer_t, void ( * )( znet_timer_t* )> m_znetTimer{ nullptr, &znet_timer_kill };
    ITimerProvider::TimerCallback m_callback;
    Interval m_interval;
};
}  // namespace

ITimerProvider::TimerUPtr DefaultTimerProvider::createTimer( TimerCallback cb ) {
    return ITimerProvider::TimerUPtr( new DefaultTimer( std::move( cb ) ) );
}

}  // namespace znet
