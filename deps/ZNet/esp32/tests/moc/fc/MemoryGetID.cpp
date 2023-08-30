#include <cstdint>
#include <vector>
#include <iostream>
#include "MemoryGetID.h"
#include "Defs.h"
#include "Msg.h"

MemoryGetID::MemoryGetID( ZwaveHub& hub )
: Command( zdef::FunctionClassId::ApplicationNodeInformationCommandClasses, hub ){};

void MemoryGetID::execute() {
    std::cout << "MemoryGetID\n";
    getCommandInvoker().setAck();

    std::uint32_t hid = getCommandInvoker().getHomeId();
    uint8_t nid = getCommandInvoker().getNodeId();

    std::vector<uint8_t> parameterPack{ ( uint8_t )( hid >> 24 & 0xFF ),
                                        ( uint8_t )( hid >> 16 & 0xFF ),
                                        ( uint8_t )( hid >> 8 & 0xFF ),
                                        ( uint8_t )( hid >> 0 & 0xFF ),
                                        nid };

    Msg msg( zdef::Type::Respons, get_id() );
    msg.append( std::move( parameterPack ) );
    getCommandInvoker().setTxdata( std::move( msg ) );
}
