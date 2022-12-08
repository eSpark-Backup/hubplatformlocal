#include "DefaultCommandHolder.h"
#include "Defs.h"

DefaultCommandHolder::DefaultCommandHolder( ZwaveHub& hub, uint8_t cmdid )
: Command( zdef::FunctionClassId::DefaultCommand, hub )
, m_commandId( cmdid ){};

void DefaultCommandHolder::execute() {
    std::ios_base::fmtflags f( std::cout.flags() );
    std::cout << std::hex;
    std::cout << "Error serial API command ID: 0x" << (int)m_commandId << '\n';
    std::cout.flags( f );
    std::abort();
}
