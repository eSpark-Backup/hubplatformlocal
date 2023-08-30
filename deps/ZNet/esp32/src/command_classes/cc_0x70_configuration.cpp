#include "znet/cc_0x70_configuration.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include <algorithm>
#include "znet/enums/configuration_format.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cstring>

namespace znet {

bool ZCc0x70Configuration::interview( InterviewCallback callback, WeakJobContext context ) {
    if( getVersion() <= 2 || isLowMemoryDevice() ) {
        // Nothing to do
        return ZClass::interview( callback, context );
    }
    else {
        m_v3Parameters.clear();

        auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

        return getParentNode().getRequestsManager().doRequest(
            ConfigurationUnknownPropertiesRequest( getNodeId(), getChannelId(), 0 ),
            context,
            [=]( const znet_configuration_properties_report_t& report ) {
                interviewNextProperty( context, callbackPtr, report.next_param_number );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
    }
}

bool ZCc0x70Configuration::storeData( IFile& file ) const {
    // Save V1 settings
    const uint8_t parametersCount = static_cast<uint8_t>( m_v1Parameters.size() );
    auto result = file.write( &parametersCount, sizeof( uint8_t ) );

    for( auto& parameterInfo : m_v1Parameters ) {
        result = result && file.write( &parameterInfo.first, sizeof( uint8_t ) ) &&
                 file.write( &parameterInfo.second, sizeof( uint8_t ) );
    }

    // Save V3 settings
    const uint16_t v3ParametersCount = static_cast<uint16_t>( m_v3Parameters.size() );
    result = result && file.write( &v3ParametersCount, sizeof( v3ParametersCount ) );
    for( const auto& parameter : m_v3Parameters ) {
        result = result && file.write( &parameter.m_id, sizeof( parameter.m_id ) );
        result = result && file.write( &parameter.m_format, sizeof( parameter.m_format ) );
        result = result && file.write( &parameter.m_size, sizeof( parameter.m_size ) );
        result = result && file.write( &parameter.m_readOnly, sizeof( parameter.m_readOnly ) );
        result = result && file.write( &parameter.m_minValue, sizeof( parameter.m_minValue ) );
        result = result && file.write( &parameter.m_maxValue, sizeof( parameter.m_maxValue ) );
        result = result && file.write( &parameter.m_defaultValue, sizeof( parameter.m_defaultValue ) );

        size_t nameLength = parameter.m_name.size();
        result = result && file.write( &nameLength, sizeof( nameLength ) );
        for( size_t i = 0; i < nameLength; ++i ) {
            uint8_t character = static_cast<uint8_t>( parameter.m_name[i] );
            result = result && file.write( &character, sizeof( character ) );
        }

        size_t infoLength = parameter.m_info.size();
        result = result && file.write( &infoLength, sizeof( infoLength ) );
        for( size_t i = 0; i < infoLength; ++i ) {
            uint8_t character = static_cast<uint8_t>( parameter.m_info[i] );
            result = result && file.write( &character, sizeof( character ) );
        }
    }

    return result;
}

bool ZCc0x70Configuration::restoreData( IFile& file, int version ) {
    if( version < majorFormatVersion_v6 )
        return true;

    bool result = true;

    uint8_t parametersCount;
    result &= file.read( &parametersCount, sizeof( parametersCount ) );
    m_v1Parameters.clear();

    uint8_t parameterNumber;
    uint8_t parameterSize;
    for( uint8_t i = 0; i != parametersCount; ++i ) {
        result &= file.read( &parameterNumber, sizeof( parameterNumber ) ) &&
                  file.read( &parameterSize, sizeof( parameterSize ) );

        m_v1Parameters.emplace( parameterNumber, parameterSize );
    }

    if( version >= majorFormatVersion_v13 ) {
        m_v3Parameters.clear();

        uint16_t v3ParametersCount;
        result &= file.read( &v3ParametersCount, sizeof( v3ParametersCount ) );
        m_v3Parameters.reserve( v3ParametersCount );
        for( int i = 0; i < v3ParametersCount; ++i ) {
            uint16_t id;
            result &= file.read( &id, sizeof( id ) );
            Parameter parameter = createDefaultParameter( id );
            result &= file.read( &parameter.m_format, sizeof( parameter.m_format ) );
            result &= file.read( &parameter.m_size, sizeof( parameter.m_size ) );
            if( version >= majorFormatVersion_v17 ) {
                result &= file.read( &parameter.m_readOnly, sizeof( parameter.m_readOnly ) );
            }
            result &= file.read( &parameter.m_minValue, sizeof( parameter.m_minValue ) );
            result &= file.read( &parameter.m_maxValue, sizeof( parameter.m_maxValue ) );
            result &= file.read( &parameter.m_defaultValue, sizeof( parameter.m_defaultValue ) );

            size_t nameLength;
            result &= file.read( &nameLength, sizeof( nameLength ) );
            parameter.m_name.reserve( nameLength );
            for( size_t j = 0; j < nameLength; ++j ) {
                uint8_t character;
                result &= file.read( &character, sizeof( character ) );
                parameter.m_name.push_back( static_cast<char>( character ) );
            }

            size_t infoLength;
            result &= file.read( &infoLength, sizeof( infoLength ) );
            parameter.m_info.reserve( infoLength );
            for( size_t j = 0; j < infoLength; ++j ) {
                uint8_t character;
                result &= file.read( &character, sizeof( character ) );
                parameter.m_info.push_back( static_cast<char>( character ) );
            }

            m_v3Parameters.push_back( std::move( parameter ) );
        }
    }

    return result;
}

bool ZCc0x70Configuration::requestValue( const RequestValue& request ) {
    auto jobContext = request.jobContext;
    auto callbackPtr =
        request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr;

    if( getVersion() <= 2 ) {
        return !m_v1Parameters.empty() && requestNextV1Parameter( m_v1Parameters.begin(), jobContext, callbackPtr );
    }
    else {
        return !m_v3Parameters.empty() && requestNextV3Parameter( 0, jobContext, callbackPtr );
    }
}

bool ZCc0x70Configuration::addConfiguration( uint8_t parameterNumber, uint8_t configSize ) {
    return m_v1Parameters.emplace( parameterNumber, configSize ).second;
}

bool ZCc0x70Configuration::removeConfiguration( uint8_t parameterNumber ) {
    return m_v1Parameters.erase( parameterNumber );
}

RequestStatus::Enum ZCc0x70Configuration::setConfigurationValue( uint16_t configParamNum,
                                                                 uint8_t configSize,
                                                                 znet_cmd_configuration_value_t configValue ) {
    if( m_v3Parameters.size() > 0 ) {
        const auto paramItr =
            std::find_if( m_v3Parameters.begin(), m_v3Parameters.end(), [&configParamNum]( const Parameter& param ) {
                return param.m_id == configParamNum;
            } );

        if( paramItr != m_v3Parameters.end() ) {
            if( ( *paramItr ).m_readOnly ) {
                return RequestStatus::Enum::Failed;
            }
        }
    }

    switch( configSize ) {
    case 1:
    case 2:
    case 4:
        break;
    default:
        ZNET_LOGE( "cc_0x%x: wrong configuration size\n\n", getId() );
        return RequestStatus::Failed;
    }

    znet_error_t result;
    if( configParamNum > 255 ) {
        // Use bulk command
        uint8_t buf[4];
        for( int i = 0; i < configSize; ++i ) {
            buf[i] = ( uint8_t )( configValue >> ( 8 * ( configSize - i - 1 ) ) );
        }
        result = znet_node_cmd_configuration_bulk_set(
            getParentNode().getId(), getParentChannel().getId(), configParamNum, 1, configSize, 0, 0, buf );
    }
    else {
        result = znet_node_cmd_configuration_set(
            getParentNode().getId(), getParentChannel().getId(), configParamNum, configSize, 0, configValue );
    }

    return RequestStatus::fromStatus( result );
}

RequestStatus::Enum ZCc0x70Configuration::resetConfigurationValue( uint16_t configParamNum ) {
    znet_error_t result;
    if( configParamNum > 255 ) {
        uint8_t buf[4] = { 0, 0, 0, 0 };
        result = znet_node_cmd_configuration_bulk_set(
            getParentNode().getId(), getParentChannel().getId(), configParamNum, 1, 4, 0, 1, buf );
    }
    else {
        result = znet_node_cmd_configuration_set(
            getParentNode().getId(), getParentChannel().getId(), configParamNum, 4, 1, 0 );
    }

    return RequestStatus::fromStatus( result );
}

RequestStatus::Enum ZCc0x70Configuration::resetAllConfigurationValues() {
    if( getVersion() < 4 ) {
        ZNET_LOGE( "cc_0x%x: unable to reset all configurations\n\n", getId() );
        return RequestStatus::Failed;
    }

    return RequestStatus::fromStatus(
        znet_node_cmd_configuration_default_reset( getParentNode().getId(), getParentChannel().getId() ) );
}

bool ZCc0x70Configuration::requestParameterValue( uint16_t parameterNumber ) const {
    znet_error_t result;
    if( parameterNumber > 255 ) {
        result = znet_node_cmd_configuration_bulk_get(
            getParentNode().getId(), getParentChannel().getId(), parameterNumber, 1 );
    }
    else {
        result =
            znet_node_cmd_configuration_get( getParentNode().getId(), getParentChannel().getId(), parameterNumber );
    }

    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: get: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

void ZCc0x70Configuration::forEachV3Parameter( std::function<void( const Parameter& )> callback ) const {
    for( const auto& parameter : m_v3Parameters ) {
        callback( parameter );
    }
}

const ZCc0x70Configuration::Parameter* ZCc0x70Configuration::findV3Parameter( uint16_t parameterNumber ) const {
    for( const auto& parameter : m_v3Parameters ) {
        if( parameter.m_id == parameterNumber ) {
            return &parameter;
        }
    }

    return nullptr;
}

bool ZCc0x70Configuration::requestNextV1Parameter( std::map<uint8_t, uint8_t>::const_iterator it,
                                                   WeakJobContext jobContext,
                                                   RequestValue::CallbackPtr cb ) {
    if( it == m_v1Parameters.end() ) {
        onRequestsFinished( cb, {} );
        return true;
    }

    return getParentNode().getRequestsManager().doRequest(
        ConfigurationRequest( getNodeId(), getChannelId(), it->first ),
        jobContext,
        [=]( const znet_configuration_report_t& ) {
            if( !requestNextV1Parameter( std::next( it ), jobContext, cb ) ) {
                onRequestsFinished( cb, RequestFailType::SendingError );
            }
        },
        [=]( RequestFailType failType ) {
            // TBD: may be remove incorrect configurations from settings and use default fail handler
            if( failType != RequestFailType::Timeout ) {
                onRequestsFinished( cb, failType );
            }
            else if( !requestNextV1Parameter( std::next( it ), jobContext, cb ) ) {
                onRequestsFinished( cb, RequestFailType::SendingError );
            }
        } );
}

bool ZCc0x70Configuration::requestNextV3Parameter( size_t index,
                                                   WeakJobContext jobContext,
                                                   RequestValue::CallbackPtr cb ) {
    if( index >= m_v3Parameters.size() ) {
        onRequestsFinished( cb, {} );
        return true;
    }

    const auto& parameter = m_v3Parameters.at( index );

    if( parameter.m_id > 255 ) {
        return getParentNode().getRequestsManager().doRequest(
            ConfigurationBulkRequest( getNodeId(), getChannelId(), parameter.m_id, 1 ),
            jobContext,
            [=]( const znet_configuration_bulk_report_t& ) { requestNextV3Parameter( index + 1, jobContext, cb ); },
            std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    }
    else {
        return getParentNode().getRequestsManager().doRequest(
            ConfigurationRequest( getNodeId(), getChannelId(), static_cast<uint8_t>( parameter.m_id ) ),
            jobContext,
            [=]( const znet_configuration_report_t& ) { requestNextV3Parameter( index + 1, jobContext, cb ); },
            std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
            DefaultPostponedRequestDelay );
    }
}

void ZCc0x70Configuration::interviewNextProperty( WeakJobContext jobContext,
                                                  RequestValue::CallbackPtr cb,
                                                  uint16_t id ) {
    if( id == 0 ) {
        setInterviewDone( true );
        onRequestsFinished( cb, {} );
    }
    else {
        requestParameterName( jobContext, cb, std::make_shared<Parameter>( createDefaultParameter( id ) ) );
    }
}

void ZCc0x70Configuration::requestParameterName( WeakJobContext jobContext,
                                                 RequestValue::CallbackPtr cb,
                                                 ParameterPtr parameter,
                                                 utils::optional<uint8_t> reportsToWait ) {
    getParentNode().getRequestsManager().doRequest(
        ConfigurationNameRequest( getNodeId(), getChannelId(), parameter->m_id, !reportsToWait.has_value() ),
        jobContext,
        [=]( const znet_configuration_name_report_t& report ) {
            if( !reportsToWait || report.rep_to_follows == *reportsToWait - 1 ) {
                parameter->m_name.reserve( parameter->m_name.size() + report.name_length );
                parameter->m_name.insert( parameter->m_name.end(), report.data, report.data + report.name_length );
            }
            else {
                ZNET_LOGW( "Unexpected config v3 param name report, reports to wait = %u\n", report.rep_to_follows );
            }

            if( report.rep_to_follows == 0 ) {
                requestParameterInfo( jobContext, cb, parameter );
            }
            else {
                requestParameterName( jobContext, cb, parameter, report.rep_to_follows );
            }
        },
        [=]( RequestFailType failType ) {
            if( reportsToWait ) {
                ZNET_LOGW( "Config v3 param name waiting failed, reports to wait = %u\n", *reportsToWait );
                requestParameterInfo( jobContext, cb, parameter );
            }
            else {
                onRequestsFinished( cb, failType );
            }
        },
        DefaultPostponedRequestDelay );
}

void ZCc0x70Configuration::requestParameterInfo( WeakJobContext jobContext,
                                                 RequestValue::CallbackPtr cb,
                                                 ParameterPtr parameter,
                                                 utils::optional<uint8_t> reportsToWait ) {
    getParentNode().getRequestsManager().doRequest(
        ConfigurationInfoRequest( getNodeId(), getChannelId(), parameter->m_id, !reportsToWait.has_value() ),
        jobContext,
        [=]( const znet_configuration_info_report_t& report ) {
            if( !reportsToWait || report.rep_to_follows == *reportsToWait - 1 ) {
                parameter->m_info.reserve( parameter->m_info.size() + report.info_length );
                parameter->m_info.insert( parameter->m_info.end(), report.data, report.data + report.info_length );
            }
            else {
                ZNET_LOGW( "Unexpected config v3 param info report, reports to wait = %u\n", report.rep_to_follows );
            }

            if( report.rep_to_follows == 0 ) {
                requestParameterProperties( jobContext, cb, parameter );
            }
            else {
                requestParameterInfo( jobContext, cb, parameter, report.rep_to_follows );
            }
        },
        [=]( RequestFailType failType ) {
            if( reportsToWait ) {
                ZNET_LOGW( "Config v3 param info waiting failed, reports to wait = %u\n", *reportsToWait );
                requestParameterProperties( jobContext, cb, parameter );
            }
            else {
                onRequestsFinished( cb, failType );
            }
        },
        DefaultPostponedRequestDelay );
}

void ZCc0x70Configuration::requestParameterProperties( WeakJobContext jobContext,
                                                       RequestValue::CallbackPtr cb,
                                                       ParameterPtr parameter ) {
    getParentNode().getRequestsManager().doRequest(
        ConfigurationPropertiesRequest( getNodeId(), getChannelId(), parameter->m_id ),
        jobContext,
        [=]( const znet_configuration_properties_report_t& report ) {
            // Save properties
            parameter->m_format = static_cast<ConfigurationFormat>( report.param_format );
            parameter->m_size = report.param_size;
            parameter->m_minValue = report.param_min_value;
            parameter->m_maxValue = report.param_max_value;
            parameter->m_defaultValue = report.param_default_value;
            parameter->m_readOnly = getVersion() >= 4 ? report.read_only : false;

            m_v3Parameters.push_back( std::move( *parameter ) );

            interviewNextProperty( jobContext, cb, report.next_param_number );
        },
        std::bind( onRequestsFinished, cb, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

bool ZCc0x70Configuration::isLowMemoryDevice() const {
#ifdef IS_LOW_MEMORY_DEVICE
    return true;
#else
    return false;
#endif
}

ZCc0x70Configuration::Parameter ZCc0x70Configuration::createDefaultParameter( uint16_t id ) const {
    return { .m_id = id,
             .m_format = ConfigurationFormat::Unknown,
             .m_size = 0,
             .m_readOnly = false,
             .m_minValue = 0,
             .m_maxValue = 0,
             .m_defaultValue = 0,
             .m_name = {},
             .m_info = {} };
}

}  // namespace znet
