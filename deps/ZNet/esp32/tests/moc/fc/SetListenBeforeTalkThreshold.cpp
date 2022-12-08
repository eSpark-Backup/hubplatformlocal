#include <iostream>
#include "fc/SetListenBeforeTalkThreshold.h"

SetThreshold::SetThreshold( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::SetThreshold, hub )
, m_pack( pack ){};

void SetThreshold::execute() {
    std::cout << "SetThreshold\n";
    getCommandInvoker().setAck();

    // HOST->ZW: REQ | 0x3C | bChannZW->HOST: RES | 0x3C | TRUE

    // ZW->HOST: RES | 0x54 | RetVal
    Msg msg( zdef::Type::Respons, get_id() );
    msg.append( zdef::RetVal::True );
    getCommandInvoker().setTxdata( std::move( msg ) );
}
