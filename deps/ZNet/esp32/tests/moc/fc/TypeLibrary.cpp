#include <iostream>
#include "fc/TypeLibrary.h"

TypeLibrary::TypeLibrary( ZwaveHub& hub, std::vector<uint8_t> const& pack )
: Command( zdef::FunctionClassId::TypeLibrary, hub )
, m_pack( pack ){};

void TypeLibrary::execute() {
    std::cout << "TypeLibrary\n";
    getCommandInvoker().setAck();

    // ZW->HOST: RES | 0xBD | TypeLibrary
    std::vector<uint8_t> pack = { getCommandInvoker().getLibraryType() };
    Msg msg( zdef::Type::Respons, get_id() );
    msg.append( std::move( pack ) );
    getCommandInvoker().setTxdata( std::move( msg ) );
}
