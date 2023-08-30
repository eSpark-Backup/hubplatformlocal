#include <iostream>
#include "cc/TransportService.h"
#include "fc/ApplicationCommandHandler.h"
#include "fc/SendDataMultiBridge.h"
#include "Defs.h"
#include "Msg.h"
#include "utils/plog.h"

#define META_DATA_MAX_DATA_SIZE 48

static const char* logtag = "SendDataMulti";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

SendDataMultiBridge::SendDataMultiBridge( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::SendDataMultiBridge, hub )
, m_pack( pack ){};

void SendDataMultiBridge::execute() {
    P_LOGI( logtag, "SendDataMultiBridge::execute" );
    std::cout << "SendDataMultiBridge\n";
    getCommandInvoker().setAck();

    // HOST->ZW: REQ |  0xAB | srcNodeID | numberNodes | pNodeIDList[ ] | dataLength | pData[ ] | txOptions | funcID
    uint8_t srcNodeID = m_pack.at( 0 );
    uint8_t numberNodes = m_pack.at( 1 );
    std::vector<uint8_t> pNodeIDList( m_pack.begin() + 2, m_pack.begin() + 2 + numberNodes );
    uint8_t dataLength = m_pack.at( numberNodes + 2 );
    uint8_t commandClassId = m_pack.at( numberNodes + 3 );
    std::vector<uint8_t> data( m_pack.begin() + numberNodes + 3, m_pack.end() - 2 );
    uint8_t txOptions = m_pack.at( numberNodes + dataLength + 3 );
    uint8_t funcID = m_pack.at( numberNodes + dataLength + 4 );

    // Call CommandClass
    bool result = true;
    for( uint8_t i = 0; i < numberNodes; ++i ) {
        Device& device = getCommandInvoker().deviceGet( pNodeIDList[i] );
        std::shared_ptr<CommandClass> commandClass = device.getCC( commandClassId );
        std::vector<uint8_t> retDataDommandClass;
        std::vector<std::vector<uint8_t>> rev;
        result &= commandClass->run( rev, retDataDommandClass, data );
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
