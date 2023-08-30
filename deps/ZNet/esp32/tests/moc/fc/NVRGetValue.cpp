#include "fc/NVRGetValue.h"
#include "Defs.h"
#include <iostream>

NVRGetValue::NVRGetValue( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::NVRGetValue, hub )
, m_pack( pack ) {}

void NVRGetValue::execute() {
    std::cout << "NVRGetValue" << std::endl;
    getCommandInvoker().setAck();

    uint8_t dataLength = m_pack.at( 1 );

    Msg msg( zdef::Type::Respons, get_id() );
    msg.append( std::vector<uint8_t>( dataLength, 0x00 ) );

    getCommandInvoker().setTxdata( std::move( msg ) );
}
