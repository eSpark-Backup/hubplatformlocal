#include "../cc/Version.h"

#include <assert.h>

Version::Version( uint8_t ZWaveLibraryType,
                  uint8_t ZWaveProtocolVersion,
                  uint8_t ZWaveProtocolSubVersion,
                  uint8_t ApplicationVersion,
                  uint8_t ApplicationSubVersion )
: CommandClass( CommandClass::CommandClassId::version, 1 )
, m_ZWaveLibraryType( ZWaveLibraryType )
, m_ZWaveProtocolVersion( ZWaveProtocolVersion )
, m_ZWaveProtocolSubVersion( ZWaveProtocolSubVersion )
, m_ApplicationVersion( ApplicationVersion )
, m_ApplicationSubVersion( ApplicationSubVersion ) {}

bool Version::run( std::vector<std::vector<uint8_t>>& rev,
                   std::vector<uint8_t>& ret,
                   std::vector<uint8_t> const& data ) {
    switch( data.at( 1 ) ) {
    case Command::get: {
        ret = { get_id(),
                Command::report,
                m_ZWaveLibraryType,
                m_ZWaveProtocolVersion,
                m_ZWaveProtocolSubVersion,
                m_ApplicationVersion,
                m_ApplicationSubVersion };
        break;
    }
    case Command::report:
        break;

    default:
        assert( !"Error Command" );
    }

    return true;
}
