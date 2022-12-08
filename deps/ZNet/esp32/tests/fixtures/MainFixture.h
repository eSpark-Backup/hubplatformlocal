#ifndef MAINFIXTURE_H_
#define MAINFIXTURE_H_

#include <cstdint>
#include <znet/type_utils.hpp>
#include <znet/znet.h>
#include "../moc/utils/Storage.h"
#include "gtest/gtest.h"
#include "ZwaveHub.h"
#include "Device.h"
#include "utils/plog.h"
#include "utils/StorageInterface.h"

extern "C" {
#include <znet_lib_cc.h>
}

class MainFixture : public StorageInterface, public ::testing::Test {
public:
    MainFixture();
    explicit MainFixture( uint8_t myNodeId );
    ~MainFixture() = default;
    void SetUp();
    void TearDown();
    void loginit();
    std::shared_ptr<Storage> getStorage();
    void printAllocInfo();
    size_t getUnfreeBlocksNum();

    // callbacks
    static void* znetAlloc( void* ptr, size_t size, void* arg );
    static uint64_t znetClock( void* arg );
    static int znetUartWrite( const void* data, size_t size, size_t* ret_size, void* arg );
    static int znetUartRead( void* data, size_t size, size_t* ret_size, void* arg );

    // utils
    static void znPrintNodeInfo( const znet_nodeinfo_t* node_info );
    static void znPrintHex( const void* src, size_t data_size );
    static void nodeAddInvoked( void* arg );

    template <typename TestSuite, typename F, F f, typename R, typename... Args>
    static void propagateCallbackImpl( R ( *&dest )( Args... ) ) {
        dest = []( Args... args ) -> R {
            auto argsTuple = std::make_tuple( args... );
            void*& arg = std::get<sizeof...( Args ) - 1>( argsTuple );
            auto fixture = static_cast<MainFixture*>( arg );
            auto testSuite = static_cast<TestSuite*>( fixture );
            arg = testSuite->getImpl().getCallbacks().arg;

            return znet::utils::applyTuple( testSuite->getImpl().getCallbacks().*f, argsTuple );
        };
    }

    template <typename TestSuite, typename F, F f>
    void propagateCallback() {
        propagateCallbackImpl<TestSuite, F, f>( m_callbacks.*f );
    }

    static void onMessageReceived( uint8_t nodeId,
                                   uint8_t channelId,
                                   const std::vector<uint8_t>& data,
                                   uint8_t security,
                                   uint8_t messageType = MESSAGE_TYPE_SET_GET );

    static bool znetCheckNodeSupportsCommandClass( znet_node_id_t nodeId,
                                                   znet_node_channel_id_t endpoint,
                                                   znet_command_class_t commandClassId,
                                                   void* arg );

public:
    std::shared_ptr<ZwaveHub> m_hub;
    std::shared_ptr<Storage> m_store;
    znet_callbacks_t m_callbacks;

private:
    znet_node_id_t m_myNodeId;
    static const std::size_t m_defaultZnetUartTxBuffSize = 256;
    uint64_t m_znetClockCounter = 1;
    std::map<void*, size_t> m_allocInfo;

protected:
    std::uint32_t m_procIteration = 100000;
};

#endif /* MAINFIXTURE_H_ */
