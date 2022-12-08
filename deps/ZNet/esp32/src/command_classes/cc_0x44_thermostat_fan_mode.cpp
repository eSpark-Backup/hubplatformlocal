#include "znet/cc_0x44_thermostat_fan_mode.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x44ThermostatFanMode::setValue( const uint8_t value ) {
    return ZNET_ERR_OK ==
           znet_node_cmd_thermostat_fan_mode_set( getParentNode().getId(), getParentChannel().getId(), value );
}

bool ZCc0x44ThermostatFanMode::requestValue( const RequestValue& request ) {
    const bool result =
        requestValueImpl( *this, ThermostatFanModeRequest( getNodeId(), getChannelId() ), *request.toRequestValue() );

    if( !result ) {
        ZNET_LOGE( "cannot request thermostat fan mode for node: %i\n", getNodeId() );
    }
    return result;
}

bool ZCc0x44ThermostatFanMode::interview( InterviewCallback callback, WeakJobContext context ) {
    return singleInterviewRequestImpl( *this,
                                       ThermostatSuppertedFanModesRequest( getNodeId(), getChannelId() ),
                                       context,
                                       [this]( const znet_thermostat_fan_mode_supported_report_t& report ) {
                                           m_fanmodes.clear();
                                           m_fanmodes.reserve( utils::getBitsCount( report.bit_mask, report.fmcount ) );
                                           utils::forEachBit( report.bit_mask, report.fmcount, [this]( uint8_t mode ) {
                                               m_fanmodes.push_back( mode );
                                           } );
                                       },
                                       std::move( callback ) );
}

bool ZCc0x44ThermostatFanMode::restoreData( IFile& file, int version ) {
    size_t count = 0;
    uint8_t param = 0;

    if( file.read( &count, sizeof( count ) ) != true ) {
        return false;
    }
    for( int i = 0; i < count; ++i ) {
        if( file.read( &param, sizeof( param ) ) != true ) {
            return false;
        }
        restoreFanMode( param );
    }

    return true;
}

bool ZCc0x44ThermostatFanMode::storeData( IFile& file ) const {
    size_t count = getFanModesCount();
    if( file.write( &count, sizeof( count ) ) != true ) {
        return false;
    }
    for( int i = 0; i < count; ++i ) {
        uint8_t param = getFanMode( i );
        if( file.write( &param, sizeof( param ) ) != true ) {
            return false;
        }
    }

    return true;
}

void ZCc0x44ThermostatFanMode::restoreFanMode( uint8_t mode ) {
    m_fanmodes.push_back( mode );
}

uint8_t ZCc0x44ThermostatFanMode::getFanMode( uint8_t index ) const {
    if( index < m_fanmodes.size() ) {
        return m_fanmodes[index];
    }
    return 0;
}

size_t ZCc0x44ThermostatFanMode::getFanModesCount() const {
    return m_fanmodes.size();
}

}  // namespace znet
