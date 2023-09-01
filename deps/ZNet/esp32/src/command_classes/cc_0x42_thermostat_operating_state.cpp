#include "znet/cc_0x42_thermostat_operating_state.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/enums/thermostat_operating_state.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cassert>

namespace znet {

bool ZCc0x42ThermostatOpState::requestValue( const RequestValue& request ) {
    const auto result =
        requestValueImpl( *this, ThermostatOperatingStateRequest( getNodeId(), getChannelId() ), request );
    if( !result ) {
        ZNET_LOGE( "cc_0x30: requestValue failed" );
    }

    return result;
}

bool ZCc0x42ThermostatOpState::interview( InterviewCallback callback, WeakJobContext context ) {
    if( getVersion() <= 1 ) {
        return false;
    }

    return singleInterviewRequestImpl(
        *this,
        ThermostatSuppertedOperatingStatesRequest( getNodeId(), getChannelId() ),
        context,
        [this]( const znet_thermostat_operating_state_logging_supported_report_t& report ) {
            m_opstates.clear();
            m_opstates.reserve( utils::getBitsCount( report.bit_mask, report.opscount ) );
            utils::forEachBit( report.bit_mask, report.opscount, [this]( uint8_t mode ) {
                m_opstates.push_back( static_cast<ThermostatOperatingState>( mode ) );
            } );
        },
        std::move( callback ) );
}

bool ZCc0x42ThermostatOpState::restoreData( IFile& file, int version ) {
    size_t count = 0;
    ThermostatOperatingState param;

    if( file.read( &count, sizeof( count ) ) != true ) {
        return false;
    }
    for( int i = 0; i < count; ++i ) {
        if( file.read( &param, sizeof( param ) ) != true ) {
            return false;
        }
        m_opstates.push_back( param );
    }

    return true;
}

bool ZCc0x42ThermostatOpState::storeData( IFile& file ) const {
    bool result = true;

    size_t count = getOpStatesLoggingCount();
    if( file.write( &count, sizeof( count ) ) != true ) {
        return false;
    }
    for( int i = 0; i < count; ++i ) {
        ThermostatOperatingState param = getOpStateLogging( i );
        if( result && file.write( &param, sizeof( param ) ) != true ) {
            return false;
        }
    }

    return true;
}

ThermostatOperatingState ZCc0x42ThermostatOpState::getOpStateLogging( uint8_t index ) const {
    if( index < m_opstates.size() ) {
        return m_opstates[index];
    }

    assert( !"Out of range" );

    return ThermostatOperatingState::Idle;
}

size_t ZCc0x42ThermostatOpState::getOpStatesLoggingCount() const {
    return m_opstates.size();
}

}  // namespace znet
