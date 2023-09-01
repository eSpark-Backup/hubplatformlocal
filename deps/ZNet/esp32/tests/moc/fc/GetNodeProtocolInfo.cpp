#include <iostream>
#include <iostream>
#include <vector>
#include "fc/SendData.h"
#include "fc/GetNodeProtocolInfo.h"
#include "Msg.h"
#include "Defs.h"

GetNodeProtocolInfo::GetNodeProtocolInfo( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::GetNodeProtocolInfo, hub )
, m_pack( pack ){};

void GetNodeProtocolInfo::execute() {
    std::cout << "GetNodeProtocolInfo\n";
    getCommandInvoker().setAck();

    // TODO
    Msg msg( zdef::Type::Respons, get_id() );
    std::vector<uint8_t> nif = { 0xd3, 0x9c, 0x00, 0x04, 0x10, 0x01 };
    msg.append( std::move( nif ) );
    getCommandInvoker().setTxdata( std::move( msg ) );
}
