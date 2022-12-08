#include "../cc/BinarySwitch.h"

#include <vector>
#include <assert.h>

BinarySwitch::BinarySwitch()
: CommandClass( CommandClass::CommandClassId::binarySwitch, 1 )
, m_val( false ) {}

bool BinarySwitch::run( std::vector<std::vector<uint8_t>>& rev,
                        std::vector<uint8_t>& ret,
                        std::vector<uint8_t> const& data ) {
    switch( data.at( 1 ) ) {
    case Command::get: {
        uint8_t val = m_val ? 0xFF : 0x00;
        ret = { get_id(), Command::report, val };
        break;
    }
    case Command::set: {
        uint8_t val = data.at( 2 );
        if( val == 0 ) {
            m_val = false;
        }
        else if( val >= 1 && val <= 99 || val == 0xFF ) {
            m_val = true;
        }
        else {
            return false;
        }
        break;
    }
    case Command::report:
        break;

    default:
        assert( !"Error Command" );
    }

    return true;
}
