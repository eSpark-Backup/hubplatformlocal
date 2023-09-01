#include "fc/GetSUCNodeID.h"
#include <iostream>
#include "Defs.h"

GetSUCNodeID::GetSUCNodeID( ZwaveHub& hub )
: Command( zdef::FunctionClassId::GetSUCNodeID, hub ){};

void GetSUCNodeID::execute() {
    std::cout << "GetSUCNodeID" << std::endl;
    getCommandInvoker().setAck();

    // ZW->HOST: RES | 0x56 | SUCNodeID
    std::vector<uint8_t> pack = { getCommandInvoker().getSUCNodeId() };
    Msg msg( zdef::Type::Respons, get_id() );
    msg.append( std::move( pack ) );
    getCommandInvoker().setTxdata( std::move( msg ) );
}
