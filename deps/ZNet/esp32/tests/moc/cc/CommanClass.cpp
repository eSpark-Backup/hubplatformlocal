#include "../cc/CommanClass.h"

CommandClass::CommandClass( uint8_t id, uint8_t v )
: m_id( id )
, m_version( v ){};

uint8_t CommandClass::get_id() {
    return m_id;
}

uint8_t CommandClass::get_version() {
    return m_version;
}
