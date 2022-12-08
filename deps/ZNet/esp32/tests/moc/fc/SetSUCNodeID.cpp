#include <iostream>
#include "fc/SetSUCNodeID.h"

SetSUCNodeID::SetSUCNodeID( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::GetSUCNodeID, hub )
, m_pack( pack ){};

void SetSUCNodeID::execute() {
    std::cout << "SetSUCNodeID\n";
    getCommandInvoker().setAck();

    // HOST->ZW: REQ | 0x54 | nodeID | SUCState | bTxOption | capabilities | funcID
    uint8_t nodeID = m_pack.at( 0 );
    uint8_t SUCState = m_pack.at( 1 );
    uint8_t bTxOption = m_pack.at( 2 );
    uint8_t capabilities = m_pack.at( 3 );
    uint8_t funcID = m_pack.at( 4 );

    if( SUCState == 0x00 ) {
        getCommandInvoker().setSUCNodeId( 0 );
    }
    else {
        // TODO
        getCommandInvoker().setSUCNodeId( 0 );
    }

    {
        // ZW->HOST: RES | 0x54 | RetVal
        Msg msg( zdef::Type::Request, get_id() );
        msg.append( zdef::RetVal::True );
        getCommandInvoker().setTxdata( std::move( msg ) );
    }

    {
        // ZW->HOST: REQ | 0x54 | funcID | txStatus
        uint8_t txStatus = 0x00;
        Msg msg( zdef::Type::Request, get_id() );
        msg.append( funcID );
        msg.append( txStatus );
        getCommandInvoker().setTxdata( std::move( msg ) );
    }
}
