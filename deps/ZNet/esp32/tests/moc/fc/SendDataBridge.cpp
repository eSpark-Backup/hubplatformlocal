#include <iostream>
#include <iostream>
#include "cc/TransportService.h"
#include "fc/ApplicationCommandHandler.h"
#include "fc/SendDataBridge.h"
#include "Defs.h"
#include "Msg.h"

#define META_DATA_MAX_DATA_SIZE 48

SendDataBridge::SendDataBridge( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::SendDataBridge, hub )
, m_pack( pack ){};

void SendDataBridge::execute() {
    std::cout << "SendDataBridge\n";
    getCommandInvoker().setAck();

    // HOST->ZW: REQ | 0xA9 | srcNodeID | destNodeID | dataLength | pData[ ] | txOptions | pRoute[4] | funcID
    uint8_t nodeId = m_pack.at( 1 );
    uint8_t dataLength = m_pack.at( 2 );
    uint8_t commandClassId = m_pack.at( 3 );
    std::vector<uint8_t> data( m_pack.begin() + 3, m_pack.end() - 6 );
    uint8_t txOptions = m_pack.at( dataLength + 3 );
    uint8_t funcID = m_pack.at( dataLength + 8 );

    // Call CommandClass
    Device& device = getCommandInvoker().deviceGet( nodeId );
    std::shared_ptr<CommandClass> commandClass = device.getCC( commandClassId );
    std::vector<uint8_t> retDataDommandClass;
    std::vector<std::vector<uint8_t>> rev;
    bool resust = commandClass->run( rev, retDataDommandClass, data );

    // Set RetVal
    {
        Msg msg( zdef::Type::Respons, get_id() );
        msg.append( resust ? zdef::RetVal::True : zdef::RetVal::False );
        getCommandInvoker().setTxdata( std::move( msg ) );
    }

    // ZW->HOST: REQ | 0x13 | funcID | txStatus | wTransmitTicksMSB | wTransmitTicksLSB | bRepeaters |
    // rssi_values.incoming[0] | rssi_values.incoming[1] | rssi_values.incoming[2] | rssi_values.incoming[3] |
    // rssi_values.incoming[4] | bACKChannelNo | bLastTxChannelNo | bRouteSchemeState | repeater0 |
    // repeater1 | repeater2 | repeater3 | routespeed | bRouteTries | bLastFailedLink.from | bLastFailedLink.to
    {
        std::vector<uint8_t> sendDataResp = { funcID, 0x00, 0x00, 0x03, 0x00, 0xb9, 0x7f, 0x7f, 0x7f, 0x7f, 0x00,
                                              0x01,   0x04, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00 };
        Msg msg( zdef::Type::Request, get_id() );
        msg.append( std::move( sendDataResp ) );
        getCommandInvoker().setTxdata( std::move( msg ) );
    }

    // ApplicationCommandHandler
    if( resust && retDataDommandClass.size() > 0 ) {
        ApplicationCommandHandler ach{ nodeId };
        std::vector<Msg> ret;
        ach.get( ret, std::move( retDataDommandClass ) );
        for( auto msg : ret ) {
            getCommandInvoker().setTxdata( std::move( msg ) );
        }
    }

    for( auto lpack : rev ) {
        // HOST->ZW: REQ | 0x13 | nodeID | dataLength | pData[ ] | txOptions | funcID
        uint8_t dataLength = lpack.size();
        uint8_t commandClassId = lpack.at( 0 );
        std::vector<uint8_t> data( lpack.begin(), lpack.end() );

        // Call CommandClass
        Device& dev = getCommandInvoker().deviceGet( nodeId );
        std::shared_ptr<CommandClass> commandClass = dev.getCC( commandClassId );
        std::vector<uint8_t> retDataDommandClass;
        std::vector<std::vector<uint8_t>> rev;
        bool res = commandClass->run( rev, retDataDommandClass, data );

        // ApplicationCommandHandler
        if( res && retDataDommandClass.size() > 0 ) {
            ApplicationCommandHandler ach{ nodeId };
            std::vector<Msg> ret;
            ach.get( ret, std::move( retDataDommandClass ) );
            for( auto msg : ret ) {
                getCommandInvoker().setTxdata( std::move( msg ) );
            }
        }
    }
}
