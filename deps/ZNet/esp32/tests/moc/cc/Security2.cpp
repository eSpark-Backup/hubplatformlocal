#include "Security2.h"

Security2::Security2()
: CommandClass( CommandClassId::security2, 0x1 ) {}

bool Security2::run( std::vector<std::vector<uint8_t>>& rev,
                     std::vector<uint8_t>& ret,
                     const std::vector<uint8_t>& data ) {
    if( data.size() != 2 ) {
        assert( !"KEX_GET command of length 2 is expected in Security 2 command class mock." );
        return false;
    }
    if( data[1] != 0x4 ) {
        assert( !"KEX_GET command is expected in Security 2 command class mock." );
        return false;
    }
    return true;
}
