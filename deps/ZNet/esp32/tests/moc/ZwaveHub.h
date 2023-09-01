#ifndef ZWAVEHUB_H_
#define ZWAVEHUB_H_

#include <map>
#include <vector>
#include <queue>
#include <memory>
#include <cstdint>
#include <cinttypes>
#include "Device.h"
#include "Msg.h"
#include "gmock/gmock.h"
#include <znet/znet_provisioning_types.hpp>

class ZwaveHub : public Zwavenode {
public:
    ZwaveHub( std::uint32_t homeId, uint8_t nodeId );
    ZwaveHub( std::uint32_t homeId, uint8_t nodeId, uint8_t sisId );
    virtual ~ZwaveHub(){};

    void deviceAdd( std::shared_ptr<Device> device );
    Device& deviceGet( uint8_t node );
    bool diviceInInclusionExist();
    Device& deviceInInclusionGet();
    void setSUCNodeId( uint8_t SUCNodeId );
    uint8_t getSUCNodeId();
    void setLibraryType( uint8_t libraryType );
    uint8_t getLibraryType();
    std::vector<uint8_t> getLibraryVersion();
    void setDskDerivedHomeId( const ::znet::DskDerivedHomeId& dsk );
    ::znet::DskDerivedHomeId getDskDerivedHomeId() const;

    void rxData( const void* data, uint8_t length );
    uint8_t txData( void* data, uint8_t length );
    void setAck();
    void setTxdata( Msg&& msg );
    uint8_t getTxDataLength();
    virtual void SendDataMultiWasReceived(){};
    virtual void SendDataMultiBridgeWasReceived(){};

private:
    std::map<uint8_t, std::shared_ptr<Device>> m_deviceList;
    std::queue<uint8_t> m_txqueue;
    std::vector<uint8_t> m_rxData;
    bool m_canRead = false;
    uint8_t m_SUCNodeId;
    uint8_t m_LibraryType = 1;
    ::znet::DskDerivedHomeId m_dskDerivedHomeId;
    const std::vector<uint8_t> m_LibraryVersion = {
        0x5a,  // Z
        0x2d,  // -
        0x57,  // W
        0x61,  // a
        0x76,  // v
        0x65,  // e
        0x20,  //

        0x36,  // 6
        0x2e,  // .
        0x30,  // 0
        0x34,  // 4
        0x00   // '\0'
    };

    Device m_defaultDevice{ 0, 0 };
};

using namespace testing;
class MockHub : public ZwaveHub {
public:
    MockHub( std::uint32_t homeId, uint8_t nodeId )
    : ZwaveHub( homeId, nodeId ){};
    MOCK_METHOD0( SendDataMultiWasReceived, void() );
    MOCK_METHOD0( SendDataMultiBridgeWasReceived, void() );
};

#endif /* ZWAVEHUB_H_ */
