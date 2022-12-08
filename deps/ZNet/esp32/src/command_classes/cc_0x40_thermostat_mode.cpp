#include "znet/cc_0x40_thermostat_mode.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x40ThermostatMode::setValue( const uint8_t value ) {
    znet_error_t result =
        znet_node_cmd_thermostat_mode_set( getParentNode().getId(), getParentChannel().getId(), value, nullptr, 0 );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: set: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

bool ZCc0x40ThermostatMode::requestValue( const RequestValue& request ) {
    const bool result =
        requestValueImpl( *this, ThermostatModeRequest( getNodeId(), getChannelId() ), *request.toRequestValue() );

    if( !result ) {
        ZNET_LOGE( "cannot request thermostat mode for node: %i\n", getNodeId() );
    }
    return result;
}

bool ZCc0x40ThermostatMode::interview( InterviewCallback callback, WeakJobContext context ) {
    return singleInterviewRequestImpl(
        *this,
        ThermostatSupportedModesRequest( getNodeId(), getChannelId() ),
        context,
        [this]( const znet_thermostat_mode_supported_report_t& report ) {
            m_modes.clear();
            m_modes.reserve( utils::getBitsCount( report.bit_mask, report.mcount ) );
            utils::forEachBit( report.bit_mask, report.mcount, [this]( uint8_t mode ) { m_modes.push_back( mode ); } );
        },
        std::move( callback ) );
}

bool ZCc0x40ThermostatMode::restoreData( IFile& file, int version ) {
    size_t count = 0;
    uint8_t param = 0;

    if( file.read( &count, sizeof( count ) ) != true ) {
        return false;
    }
    for( int i = 0; i < count; ++i ) {
        if( file.read( &param, sizeof( param ) ) != true ) {
            return false;
        }
        restoreMode( param );
    }

    return true;
}

bool ZCc0x40ThermostatMode::storeData( IFile& file ) const {
    size_t count = getModesCount();
    if( file.write( &count, sizeof( count ) ) != true ) {
        return false;
    }
    for( int i = 0; i < count; ++i ) {
        uint8_t param = getMode( i );
        if( file.write( &param, sizeof( param ) ) != true ) {
            return false;
        }
    }
    return true;
}

void ZCc0x40ThermostatMode::restoreMode( uint8_t mode ) {
    m_modes.push_back( mode );
}

uint8_t ZCc0x40ThermostatMode::getMode( uint8_t index ) const {
    if( index < m_modes.size() ) {
        return m_modes[index];
    }
    return 0;
}

size_t ZCc0x40ThermostatMode::getModesCount() const {
    return m_modes.size();
}

bool ZCc0x40ThermostatMode::setManufacturerSpecificMode( const uint8_t* data, uint8_t size ) {
    znet_error_t result =
        znet_node_cmd_thermostat_mode_set( getParentNode().getId(), getParentChannel().getId(), 0x1F, data, size );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: set: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

}  // namespace znet
