#include <iostream>
#include "fc/LibraryVersion.h"

LibraryVersion::LibraryVersion( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::LibraryVersion, hub )
, m_pack( pack ){};

void LibraryVersion::execute() {
    std::cout << "LibraryVersion\n";
    getCommandInvoker().setAck();

    // ZW->HOST: RES | 0x15 | buffer (12 bytes) | library type

    std::vector<uint8_t> pack = { getCommandInvoker().getLibraryVersion() };
    pack.emplace_back( getCommandInvoker().getLibraryType() );
    Msg msg( zdef::Type::Respons, get_id() );
    msg.append( std::move( pack ) );
    getCommandInvoker().setTxdata( std::move( msg ) );
}
