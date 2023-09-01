#include "znet/cc_0x43_thermostat_setpoint.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cmath>
#include <unordered_set>

namespace znet {

bool ZCc0x43ThermostatSetpoint::interview( InterviewCallback callback, WeakJobContext context ) {
    ZNET_LOGD( "0x43 ThermostatSetpoint interview start\n" );

    auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

    return getParentNode().getRequestsManager().doRequest(
        ThermostatSetpointSupportedRequest( getNodeId(), getChannelId() ),
        context,
        [=]( const znet_thermostat_setpoint_supported_report_t& value ) {
            setSetpointTypes( value.bit_mask, value.stcount );

            if( getVersion() < 3 ) {
                requestNextSetpointInterview( callbackPtr, context );
            }
            else {
                requestNextSetpointCapability( callbackPtr, context );
            }
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

bool ZCc0x43ThermostatSetpoint::requestValue( const RequestValue& request ) {
    return !m_description.empty() &&
           requestNextSetpoint(
               request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr,
               request.jobContext );
}

bool ZCc0x43ThermostatSetpoint::requestSetpoint(
    const uint8_t type,
    std::function<void( const znet_thermostat_setpoint_report_t&, bool& requested )> onSuccess,
    std::function<void( RequestFailType )> onFail ) {
    return getParentNode().getRequestsManager().doRequest(
        ThermostatSetpointRequest( getNodeId(), getChannelId(), type ),
        WeakJobContext(),
        std::move( onSuccess ),
        std::move( onFail ) );
}

void ZCc0x43ThermostatSetpoint::requestNextSetpointCapability( InterviewCallbackPtr callbackPtr,
                                                               WeakJobContext context,
                                                               uint8_t index ) {
    if( index >= m_description.size() ) {
        setInterviewDone( true );
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    getParentNode().getRequestsManager().doRequest(
        ThermostatSetpointCapabilitiesRequest( getNodeId(), getChannelId(), m_description[index].type ),
        context,
        [=]( const znet_thermostat_setpoint_capabilities_report_t& value ) {
            auto& desc = m_description[index];
            desc.minValue = getConvertedValue( value.min_params, value.min_value );
            desc.minScale = getScale( value.min_params );
            desc.maxValue = getConvertedValue( value.max_params, value.max_value );
            desc.maxScale = getScale( value.max_params );

            requestNextSetpointInterview( callbackPtr, context, index );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

void ZCc0x43ThermostatSetpoint::requestNextSetpointInterview( RequestValue::CallbackPtr callbackPtr,
                                                              WeakJobContext context,
                                                              uint8_t index ) {
    if( index >= m_description.size() ) {
        setInterviewDone( true );
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    auto processReport = [this, index]( const znet_thermostat_setpoint_report_t& value ) {
        auto& desc = m_description[index];
        desc.size = value.size;
        desc.precision = value.precision;
    };

    if( getVersion() < 3 ) {
        getParentNode().getRequestsManager().doRequest(
            ThermostatAmbiguousSetpointRequest( getNodeId(), getChannelId(), m_description[index].type ),
            context,
            [=]( const znet_thermostat_setpoint_report_t& value, bool& requested ) {
                requested = true;
                if( value.type == 0x00 ) {
                    // Setpoint not supported
                    m_description.erase( m_description.begin() + index );
                    requestNextSetpointInterview( callbackPtr, context, index );
                }
                else {
                    processReport( value );
                    requestNextSetpointInterview( callbackPtr, context, index + 1 );
                }
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    }
    else {
        getParentNode().getRequestsManager().doRequest(
            ThermostatSetpointRequest( getNodeId(), getChannelId(), m_description[index].type ),
            context,
            [=]( const znet_thermostat_setpoint_report_t& value, bool& requested ) {
                requested = true;
                processReport( value );
                requestNextSetpointCapability( callbackPtr, context, index + 1 );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    }
}

bool ZCc0x43ThermostatSetpoint::requestNextSetpoint( RequestValue::CallbackPtr callbackPtr,
                                                     WeakJobContext context,
                                                     uint8_t index ) {
    auto onFinished = [this, callbackPtr]( RequestFailTypeOpt failTypeOpt ) {
        onRequestsFinished( callbackPtr, failTypeOpt );
    };

    if( index >= m_description.size() ) {
        onFinished( {} );
        return true;
    }

    return getParentNode().getRequestsManager().doRequest(
        ThermostatSetpointRequest( getNodeId(), getChannelId(), m_description[index].type ),
        context,
        [=]( const znet_thermostat_setpoint_report_t& value, bool& requested ) {
            requested = true;
            if( !requestNextSetpoint( callbackPtr, context, index + 1 ) ) {
                onFinished( RequestFailType::SendingError );
            }
        },
        onFinished );
}

bool ZCc0x43ThermostatSetpoint::setValue( uint8_t setpointType, uint8_t scale, float value ) {
    uint8_t size = getSize( setpointType );
    uint8_t precision = getPrecision( setpointType );
    if( size == 0 ) {
        size = 4;
        precision = 2;
    }
    return ZNET_ERR_OK == znet_node_cmd_thermostat_setpoint_set( getParentNode().getId(),
                                                                 getParentChannel().getId(),
                                                                 setpointType,
                                                                 size,
                                                                 scale,
                                                                 precision,
                                                                 utils::valueFromFloat( value, precision ) );
}

/**
 * @brief Store class data
 * @param file  Device file
 * @return status operation
 */
bool ZCc0x43ThermostatSetpoint::storeData( IFile& file ) const {
    uint8_t numSetpoints = m_description.size();
    if( !file.write( &numSetpoints, sizeof( numSetpoints ) ) ) {
        return false;
    }

    for( size_t n = 0; n < m_description.size(); n++ ) {
        if( !file.write( &m_description[n], sizeof( m_description[0] ) ) ) {
            ZNET_LOGE( "     store FAILED\n" );
            return false;
        }
    }

    return true;
}

/**
 * @brief Restore class data
 * @param file  Device file
 * @return status operation
 */
bool ZCc0x43ThermostatSetpoint::restoreData( IFile& file, int version ) {
    uint8_t numSetpoints = 0;
    if( file.read( &numSetpoints, sizeof( numSetpoints ) ) != true ) {
        return false;
    }

    m_description.reserve( numSetpoints );
    for( size_t n = 0; n < numSetpoints; n++ ) {
        if( version >= majorFormatVersion_v5 ) {
            setpointsDescription_t_v5 setpointsDescription;
            if( !file.read( &setpointsDescription, sizeof( setpointsDescription ) ) ) {
                return false;
            }
            m_description.push_back( setpointsDescription );
        }
        else {
            setpointsDescription_t setpointsDescription;
            if( !file.read( &setpointsDescription, sizeof( setpointsDescription ) ) ) {
                return false;
            }
            m_description.push_back( setpointsDescription_t_v5( setpointsDescription ) );
        }
    }
    return true;
}

void ZCc0x43ThermostatSetpoint::setSetpointTypes( const uint8_t* data, uint8_t len ) {
    m_description.clear();
    m_description.reserve( utils::getBitsCount( data, len ) * 2 );

    std::unordered_set<uint8_t> supportedTypes;

    auto addSetpointType = [this, &supportedTypes]( uint8_t type ) {
        if( isValidSetpointType( type ) && supportedTypes.insert( type ).second ) {
            ZNET_LOGD( "thermostat setpoint type added: type = 0x%X\n", type );
            setpointsDescription_t setpointsDescription;
            setpointsDescription.type = type;
            m_description.emplace_back( setpointsDescription );
        }
    };

    utils::forEachBit( data, len, [this, &addSetpointType]( uint8_t type ) {
        if( type <= ZNET_THERMOSTAT_SETPOINT_VER3_INC_VALUE ) {
            addSetpointType( type );
            return;
        }

        if( getVersion() < 3 ) {
            addSetpointType( type );
        }

        addSetpointType( type + ZNET_THERMOSTAT_SETPOINT_VER3_CORRECTOR );
    } );
}

const std::vector<ZCc0x43ThermostatSetpoint::setpointsDescription_t_v5>& ZCc0x43ThermostatSetpoint::getDescription()
    const {
    return m_description;
}

uint8_t ZCc0x43ThermostatSetpoint::getScale( uint8_t params ) const {
    return static_cast<uint8_t>( params & 0b00011000 ) >> 3;
}

float ZCc0x43ThermostatSetpoint::getConvertedValue( uint8_t params, int32_t value ) const {
    const uint8_t precision = static_cast<uint8_t>( params & 0b11100000 ) >> 5;
    return utils::valueToFloat( value, precision );
}

uint8_t ZCc0x43ThermostatSetpoint::getSize( uint8_t type ) {
    for( auto& desc : m_description ) {
        if( desc.type == type ) {
            return desc.size;
        }
    }
    return 0;
}

uint8_t ZCc0x43ThermostatSetpoint::getPrecision( uint8_t type ) {
    for( auto& desc : m_description ) {
        if( desc.type == type ) {
            return desc.precision;
        }
    }
    return 0;
}

bool ZCc0x43ThermostatSetpoint::isValidSetpointType( uint8_t type ) const {
    switch( type ) {
    case 0x01:
    case 0x02:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
        return true;

    case 0x0B:
    case 0x0C:
    case 0x0D:
        return getVersion() >= 2;

    case 0x0E:
    case 0x0F:
        return getVersion() >= 3;

    default:
        return false;
    }
}

}  // namespace znet
