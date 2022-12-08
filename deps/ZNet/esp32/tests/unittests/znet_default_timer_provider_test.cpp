#include <znet/znet_default_timer_provider.hpp>
#include <znet/znet.h>

#include "MainFixture.h"

#include <gtest/gtest.h>

namespace znet {
namespace ut {

class DefaultTimerProviderTestSuite : public MainFixture, public DefaultTimerProvider {
protected:
    void SetUp() override {
        m_callbacks.clock = []( void* arg ) -> uint64_t {
            return static_cast<DefaultTimerProviderTestSuite*>( arg )->m_timePoint.count();
        };

        MainFixture::SetUp();
    }

    void sleep( std::chrono::milliseconds duration ) {
        m_timePoint += duration;
        znet_proc();
    }

private:
    std::chrono::milliseconds m_timePoint{ 1 };
};

TEST_F( DefaultTimerProviderTestSuite, runOnce ) {
    uint8_t counter = 0;
    auto timer = createTimer( [&counter]() { ++counter; } );

    ASSERT_NE( timer, nullptr );

    timer->runOnce( std::chrono::milliseconds( 10 ) );
    sleep( std::chrono::milliseconds( 9 ) );
    EXPECT_EQ( counter, 0 );

    sleep( std::chrono::milliseconds( 1 ) );
    EXPECT_EQ( counter, 1 );

    sleep( std::chrono::milliseconds( 100 ) );
    EXPECT_EQ( counter, 1 );
}

TEST_F( DefaultTimerProviderTestSuite, runPeriodically ) {
    uint8_t counter = 0;
    auto timer = createTimer( [&counter]() { ++counter; } );

    ASSERT_NE( timer, nullptr );

    timer->runPeriodically( std::chrono::milliseconds( 5 ), std::chrono::milliseconds( 20 ) );
    sleep( std::chrono::milliseconds( 4 ) );
    EXPECT_EQ( counter, 0 );

    sleep( std::chrono::milliseconds( 1 ) );
    EXPECT_EQ( counter, 1 );

    for( int i = 0; i < 3; ++i ) {
        sleep( std::chrono::milliseconds( 19 ) );
        EXPECT_EQ( counter, 1 + i );

        sleep( std::chrono::milliseconds( 1 ) );
        EXPECT_EQ( counter, 2 + i );
    }
}

TEST_F( DefaultTimerProviderTestSuite, restartInsideCallback ) {
    uint8_t counter = 0;
    TimerUPtr timer = createTimer( [&counter, &timer]() {
        if( ++counter == 1 ) {
            timer->runOnce( std::chrono::milliseconds( 20 ) );
        }
    } );

    ASSERT_NE( timer, nullptr );

    timer->runOnce( std::chrono::milliseconds( 10 ) );

    sleep( std::chrono::milliseconds( 10 ) );
    EXPECT_EQ( counter, 1 );

    sleep( std::chrono::milliseconds( 20 ) );
    EXPECT_EQ( counter, 2 );

    sleep( std::chrono::milliseconds( 20 ) );
    EXPECT_EQ( counter, 2 );
}

TEST_F( DefaultTimerProviderTestSuite, restartPeriodicInsideCallback ) {
    uint8_t counter = 0;
    TimerUPtr timer = createTimer( [&counter, &timer]() {
        if( ++counter == 1 ) {
            timer->runPeriodically( std::chrono::milliseconds( 100 ), std::chrono::milliseconds( 200 ) );
        }
    } );

    ASSERT_NE( timer, nullptr );

    timer->runPeriodically( std::chrono::milliseconds( 10 ), std::chrono::milliseconds( 20 ) );

    sleep( std::chrono::milliseconds( 10 ) );
    EXPECT_EQ( counter, 1 );

    sleep( std::chrono::milliseconds( 99 ) );
    EXPECT_EQ( counter, 1 );

    sleep( std::chrono::milliseconds( 1 ) );
    EXPECT_EQ( counter, 2 );

    sleep( std::chrono::milliseconds( 200 ) );
    EXPECT_EQ( counter, 3 );
}

TEST_F( DefaultTimerProviderTestSuite, zeroInterval ) {
    uint8_t counter = 0;
    auto timer = createTimer( [&counter]() { ++counter; } );

    ASSERT_NE( timer, nullptr );

    timer->runPeriodically( std::chrono::milliseconds( 0 ), std::chrono::milliseconds( 0 ) );
    EXPECT_EQ( counter, 0 );

    sleep( std::chrono::milliseconds( 1 ) );
    EXPECT_EQ( counter, 1 );

    sleep( std::chrono::milliseconds( 1 ) );
    EXPECT_EQ( counter, 2 );

    timer->runOnce( std::chrono::milliseconds( 0 ) );

    sleep( std::chrono::milliseconds( 1 ) );
    EXPECT_EQ( counter, 3 );

    sleep( std::chrono::milliseconds( 1 ) );
    EXPECT_EQ( counter, 3 );
}

TEST_F( DefaultTimerProviderTestSuite, destruction ) {
    auto counter = std::make_shared<uint8_t>( 0 );
    auto timer = createTimer( [counter]() { ++*counter; } );

    ASSERT_NE( timer, nullptr );

    timer->runPeriodically( std::chrono::milliseconds( 10 ), std::chrono::milliseconds( 20 ) );

    sleep( std::chrono::milliseconds( 5 ) );

    timer.reset();

    EXPECT_EQ( counter.use_count(), 1 );
    EXPECT_EQ( *counter, 0 );

    sleep( std::chrono::milliseconds( 100 ) );

    EXPECT_EQ( counter.use_count(), 1 );
    EXPECT_EQ( *counter, 0 );
}

}  // namespace ut
}  // namespace znet
