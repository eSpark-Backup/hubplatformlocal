#include <iostream>
#include "fc/ApplicationCommandHandler.h"

ApplicationCommandHandler::ApplicationCommandHandler( uint8_t sourceNode )
: m_sourceNode( sourceNode ) {}

bool ApplicationCommandHandler::get( std::vector<Msg>& ret, std::vector<uint8_t>&& cmd ) {
    std::cout << "ApplicationCommandHandler\n";

    Msg msg( zdef::Type::Request, zdef::FunctionClassId::ApplicationCommandHandler );

    // ZW->HOST: REQ | 0x04 | rxStatus | sourceNode | cmdLength | pCmd[] | rxRSSIVal | securityKey
    uint8_t cmdLength = cmd.size();
    std::vector<uint8_t> ret0 = { m_rxStatus, m_sourceNode, cmdLength };

    msg.append( std::move( ret0 ) );
    msg.append( std::move( cmd ) );
    msg.append( m_rxRSSIVal );
    msg.append( m_securityKey );
    ret.push_back( std::move( msg ) );

    return true;
}
