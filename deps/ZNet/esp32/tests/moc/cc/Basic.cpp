#include "../cc/Basic.h"

#include <vector>
#include <assert.h>

Basic::Basic()
: CommandClass( CommandClass::CommandClassId::basic, 1 )
, m_val( 0x00 ) {}

bool Basic::run( std::vector<std::vector<uint8_t>>& rev, std::vector<uint8_t>& ret, std::vector<uint8_t> const& data ) {
    switch( data.at( 1 ) ) {
    case Command::get:
        ret = { get_id(), Command::report, m_val };
        break;

    case Command::set:
        m_val = data.at( 2 );
        break;

        /*case Command::report:
            break;*/

    default:
        assert( !"Error Command" );
    }

    return true;
}
