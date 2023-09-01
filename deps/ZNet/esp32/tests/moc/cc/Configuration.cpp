#include "../cc/Configuration.h"

#include <assert.h>

Configuration::Configuration()
: CommandClass( CommandClass::CommandClassId::configuration, 4 ) {}

void Configuration::parameterAdd( uint8_t parameterNumber, Configuration::Type type, std::uint32_t value ) {
    assert( !m_parameters.count( parameterNumber ) );
    Parameter p;
    p.type = type;
    p.value = value;
    p.defvalue = p.value;
    m_parameters[parameterNumber] = p;
}

void Configuration::parameterSet( uint8_t parameterNumber, std::uint32_t value ) {
    assert( m_parameters.count( parameterNumber ) );
    m_parameters.at( parameterNumber ).value = value;
}

std::uint32_t Configuration::parameterGet( uint8_t parameterNumber ) {
    assert( m_parameters.count( parameterNumber ) );
    return m_parameters.at( parameterNumber ).value;
}

uint8_t Configuration::parameterGetSize( uint8_t parameterNumber ) {
    assert( m_parameters.count( parameterNumber ) );
    return m_parameters.at( parameterNumber ).type;
}

void Configuration::v3ParameterAdd( uint16_t id,
                                    uint8_t format,
                                    Type size,
                                    uint32_t minValue,
                                    uint32_t maxValue,
                                    uint32_t defaultValue,
                                    uint32_t value,
                                    const std::string& name,
                                    const std::string& info ) {
    m_v3Parameters.emplace( id, V3Parameter{ id, format, size, minValue, maxValue, defaultValue, value, name, info } );
}

void Configuration::forEachV3ParameterName( std::function<void( uint16_t, const std::string& )> callback ) const {
    for( auto v3data : m_v3Parameters ) {
        callback( v3data.first, v3data.second.m_name );
    }
}

void Configuration::forEachV3ParameterInfo( std::function<void( uint16_t, const std::string& )> callback ) const {
    for( auto v3data : m_v3Parameters ) {
        callback( v3data.first, v3data.second.m_info );
    }
}

void Configuration::forEachV3ParameterProperties(
    std::function<void( uint16_t, uint8_t, Type, uint32_t, uint32_t, uint32_t )> callback ) const {
    for( auto v3data : m_v3Parameters ) {
        callback( v3data.first,
                  v3data.second.m_format,
                  v3data.second.m_size,
                  v3data.second.m_minValue,
                  v3data.second.m_maxValue,
                  v3data.second.m_defaultValue );
    }
}

void Configuration::parameterReset( uint8_t parameterNumber ) {
    assert( m_parameters.count( parameterNumber ) );
    m_parameters.at( parameterNumber ).value = m_parameters.at( parameterNumber ).defvalue;
}

bool Configuration::run( std::vector<std::vector<uint8_t>>& rev,
                         std::vector<uint8_t>& ret,
                         std::vector<uint8_t> const& data ) {
    uint8_t command = data.at( 1 );

    switch( command ) {
    /*case Command::version: {
        break;
    }*/
    case Command::Set:
        return setValue( data );
        break;

    case Command::Get:
        return getValue( ret, data );
        break;

    case Command::NameGet:
        return getName( ret, data );
        break;

    case Command::InfoGet:
        return getInfo( ret, data );
        break;

    case Command::PropertiesGet:
        return getProperties( ret, data );
        break;

    /*case Command::report:
        break;*/
    default:
        assert( !"Error Command" );
    }

    return true;
}

bool Configuration::setValue( std::vector<uint8_t> const& data ) {
    uint8_t parameterNumber = data.at( 2 );
    uint8_t size = data.at( 3 ) & 0x07;
    uint8_t def = data.at( 3 ) & 0x80;

    uint8_t sizeParameter = parameterGetSize( parameterNumber );
    assert( size == sizeParameter );

    if( def != 0 ) {
        parameterReset( parameterNumber );
    }
    else {
        switch( size ) {
        case Type::u8: {
            assert( data.size() == 5 );
            uint8_t value = data.at( 4 );
            parameterSet( parameterNumber, value );
            return true;
        }
        case Type::u16: {
            assert( data.size() == 6 );
            uint16_t value = data.at( 4 ) << 8 | data.at( 5 );
            parameterSet( parameterNumber, value );
            return true;
        }
        case Type::u32: {
            assert( data.size() == 8 );
            uint32_t value = data.at( 4 ) << 24 | data.at( 5 ) << 16 | data.at( 6 ) << 8 | data.at( 7 );
            parameterSet( parameterNumber, value );
            return true;
        }
        default: { assert( !"Error type!" ); }
        }
    }

    return false;
}

bool Configuration::getValue( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data ) {
    uint8_t parameterNumber = data.at( 2 );
    uint8_t sizeParameter = parameterGetSize( parameterNumber );

    switch( sizeParameter ) {
    case Type::u8: {
        uint8_t value = parameterGet( parameterNumber );
        ret = { get_id(), Command::Report, parameterNumber, sizeParameter, value };
        return true;
    }
    case Type::u16: {
        std::uint16_t value = parameterGet( parameterNumber );
        uint8_t value1 = value >> 8 & 0xFF;
        uint8_t value2 = value & 0xFF;
        ret = { get_id(), Command::Report, parameterNumber, sizeParameter, value1, value2 };
        return true;
    }
    case Type::u32: {
        std::uint32_t value = parameterGet( parameterNumber );
        uint8_t value1 = value >> 24 & 0xFF;
        uint8_t value2 = value >> 16 & 0xFF;
        uint8_t value3 = value >> 8 & 0xFF;
        uint8_t value4 = value & 0xFF;
        ret = { get_id(), Command::Report, parameterNumber, sizeParameter, value1, value2, value3, value4 };
        return true;
    }
    default: { assert( !"Error type!" ); }
    }

    return false;
}

bool Configuration::getName( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data ) {
    auto parameterNumber = ( (uint16_t)data.at( 2 ) << 8 ) | data.at( 3 );
    uint8_t reportsToFollow = 0;
    ret = { get_id(), Command::NameReport, data.at( 2 ), data.at( 3 ), reportsToFollow };

    auto name = getV3ParameterName( parameterNumber );
    for( auto character : name ) {
        ret.push_back( static_cast<uint8_t>( character ) );
    }

    return true;
}

bool Configuration::getInfo( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data ) {
    auto parameterNumber = ( (uint16_t)data.at( 2 ) << 8 ) | data.at( 3 );
    uint8_t reportsToFollow = 0;
    ret = { get_id(), Command::InfoReport, data.at( 2 ), data.at( 3 ), reportsToFollow };

    auto info = getV3ParameterInfo( parameterNumber );
    for( auto character : info ) {
        ret.push_back( static_cast<uint8_t>( character ) );
    }

    return true;
}

bool Configuration::getProperties( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data ) {
    auto parameterNumber = ( (uint16_t)data.at( 2 ) << 8 ) | data.at( 3 );
    const auto& param = m_v3Parameters.at( parameterNumber );

    ret = { get_id(), Command::PropertiesReport, data.at( 2 ), data.at( 3 ) };

    uint8_t sizeFormat = ( param.m_format << 3 ) | static_cast<uint8_t>( param.m_size );
    ret.push_back( sizeFormat );

    auto valueAdder = [&ret]( uint32_t value, uint8_t size ) {
        for( int i = size - 1; i >= 0; --i ) {
            ret.push_back( ( value >> 8 * i ) & 0xFF );
        }
    };

    valueAdder( param.m_minValue, param.m_size );
    valueAdder( param.m_maxValue, param.m_size );
    valueAdder( param.m_defaultValue, param.m_size );

    // Next parameter
    ret.push_back( 0 );
    ret.push_back( 0 );

    return true;
}

std::string Configuration::getV3ParameterName( uint16_t id ) const {
    return m_v3Parameters.at( id ).m_name;
}

std::string Configuration::getV3ParameterInfo( uint16_t id ) const {
    return m_v3Parameters.at( id ).m_info;
}
