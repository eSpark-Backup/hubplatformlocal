#include <iostream>
#include "cc/TransportService.h"
#include "fc/ApplicationCommandHandler.h"
#include "fc/SendDataMulti.h"
#include "Defs.h"
#include "Msg.h"
#include "utils/plog.h"

#define META_DATA_MAX_DATA_SIZE 48

static const char* logtag = "SendDataMulti";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG
#define TRANSMIT_OPTION_ACK 0x01        /* request acknowledge from destination node */
#define TRANSMIT_OPTION_AUTO_ROUTE 0x04 /* request retransmission via repeater nodes */

SendDataMulti::SendDataMulti( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::SendDataMulti, hub )
, m_pack( pack ){};

void SendDataMulti::execute() {
    P_LOGI( logtag, "SendDataMulti::execute" );
    std::cout << "SendDataMulti\n";
    getCommandInvoker().setAck();

    // HOST->ZW: REQ | 0x13 | numberNodes | pNodeIDList[ ] | dataLength | pData[ ] | txOptions | funcID
    uint8_t numberNodes = m_pack.at( 0 );
    std::vector<uint8_t> pNodeIDList( m_pack.begin() + 1, m_pack.begin() + 1 + numberNodes );
    uint8_t dataLength = m_pack.at( numberNodes + 1 );
    uint8_t commandClassId = m_pack.at( numberNodes + 2 );
    std::vector<uint8_t> data( m_pack.begin() + numberNodes + 2, m_pack.end() - 2 );
    uint8_t txOptions = m_pack.at( numberNodes + dataLength + 2 );
    uint8_t funcID = m_pack.at( numberNodes + dataLength + 3 );

    bool result = true;

    // checking the options with which the multicast was sent
    if( txOptions != ( TRANSMIT_OPTION_AUTO_ROUTE || TRANSMIT_OPTION_ACK ) ) {
        // Call CommandClass
        for( uint8_t i = 0; i < numberNodes; ++i ) {
            Device& device = getCommandInvoker().deviceGet( pNodeIDList[i] );
            std::shared_ptr<CommandClass> commandClass = device.getCC( commandClassId );
            std::vector<uint8_t> retDataDommandClass;
            std::vector<std::vector<uint8_t>> rev;
            result &= commandClass->run( rev, retDataDommandClass, data );
        }
    }
    else {
        result = false;
    }

    // Set RetVal
    {
        Msg msg( zdef::Type::Respons, get_id() );
        msg.append( result ? zdef::RetVal::True : zdef::RetVal::False );
        getCommandInvoker().setTxdata( std::move( msg ) );
    }

    // ZW->HOST: REQ | 0x14 | funcID | txStatus
    {
        std::vector<uint8_t> sendDataResp = { funcID, 0x00 };
        Msg msg( zdef::Type::Request, get_id() );
        msg.append( std::move( sendDataResp ) );
        getCommandInvoker().setTxdata( std::move( msg ) );
    }
}
