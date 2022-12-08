#include "znet/cc_0x31_sensor_multilevel.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <string.h>
#include <cassert>
#include <algorithm>

namespace znet {

bool ZCc0x31SensorMultilevel::addSensorDescriptionImpl( MultilevelSensor::Type sensorType, uint8_t sensorMask ) {
    if( !isValidSensorType( sensorType ) ) {
        return false;
    }

    for( auto& desc : m_description ) {
        if( desc.type == sensorType ) {
            desc.scalemask = sensorMask;
            return true;
        }
    }

    SensorDescription sensorsDescription = { .type = sensorType, .scalemask = sensorMask };

    m_description.push_back( sensorsDescription );

    return true;
}

bool ZCc0x31SensorMultilevel::addSensorDescription( MultilevelSensor::Type sensorType, uint8_t sensorScale ) {
    return sensorScale <= MultilevelSensor::MAX_SCALES &&
           addSensorDescriptionImpl( sensorType, static_cast<uint8_t>( 1u << sensorScale ) );
}

bool ZCc0x31SensorMultilevel::addSensorTypes( const uint8_t* data, uint8_t len ) {
    const uint8_t countTypes = utils::getBitsCount( data, len );

    m_description.reserve( countTypes );

    utils::forEachBit( data, len, [this]( uint8_t bitIndex ) {
        addSensorDescriptionImpl( static_cast<MultilevelSensor::Type>( bitIndex + 1 ), ms_invalidScaleMask );
    } );

    return true;
}

bool ZCc0x31SensorMultilevel::interview( InterviewCallback callback, WeakJobContext context ) {
    ZNET_LOGD( "multilevel sensor interview started, ver = %d\n", getVersion() );

    m_description.clear();

    if( getVersion() <= 4 ) {
        return singleInterviewRequestImpl( *this,
                                           MultiLevelSensorUnknownTypeRequest( getNodeId(), getChannelId() ),
                                           context,
                                           [this]( const znet_multilevel_sensor_report_t& report ) {
                                               addSensorDescription( static_cast<MultilevelSensor::Type>( report.type ),
                                                                     report.scale );
                                           },
                                           std::move( callback ) );
    }
    else {
        auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

        return getParentNode().getRequestsManager().doRequest(
            MultiLevelSensorSupportedTypesRequest( getNodeId(), getChannelId() ),
            context,
            [=]( const uint8_t* bitmask, uint8_t count ) {
                addSensorTypes( bitmask, count );
                requestNextScale( callbackPtr, context );
            },
            std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
    }
}

void ZCc0x31SensorMultilevel::requestNextScale( InterviewCallbackPtr callbackPtr,
                                                WeakJobContext context,
                                                uint8_t index ) {
    if( index >= m_description.size() ) {
        setInterviewDone( true );
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    getParentNode().getRequestsManager().doRequest(
        MultiLevelSensorSupportedScalesRequest( getNodeId(), getChannelId(), m_description[index].type ),
        context,
        [=]( uint8_t scalemask ) {
            m_description[index].scalemask = scalemask;
            requestNextComponentInterview( callbackPtr, context, index );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

void ZCc0x31SensorMultilevel::requestNextComponentInterview( InterviewCallbackPtr callback,
                                                             WeakJobContext context,
                                                             uint8_t index ) {
    const auto firstScale =
        utils::findFirstBit( &m_description[index].scalemask, sizeof( m_description[index].scalemask ) );

    if( !firstScale ) {
        ZNET_LOGW( "cc_0x31: interview: error: Supported scales is empty!\n\n" );
        requestNextScale( callback, context, index + 1 );
        return;
    }

    getParentNode().getRequestsManager().doRequest(
        MultiLevelSensorRequest( getNodeId(), getChannelId(), m_description[index].type, *firstScale, getVersion() ),
        context,
        [=]( const znet_multilevel_sensor_report_t& value ) { requestNextScale( callback, context, index + 1 ); },
        std::bind( onRequestsFinished, callback, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

bool ZCc0x31SensorMultilevel::requestValue( const RequestValue& request ) {
    if( m_status == Status::Polling ) {
        ZNET_LOGW( "cc_0x31: Cannot start requestValue, actual status is polling\n" );
        return false;
    }

    const auto firstIt = std::find_if( m_description.begin(), m_description.end(), []( const SensorDescription& desc ) {
        return desc.scalemask != 0;
    } );

    m_status = Status::Polling;

    const auto result =
        firstIt != m_description.end() &&
        requestNextComponent(
            request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr,
            request.jobContext,
            static_cast<uint8_t>( firstIt - m_description.begin() ) );

    if( !result ) {
        m_status = Status::Idle;
    }

    return result;
}

bool ZCc0x31SensorMultilevel::requestNextComponent( InterviewCallbackPtr callbackPtr,
                                                    WeakJobContext context,
                                                    uint8_t index ) {
    assert( m_status == Status::Polling );

    auto onFinished = [this, callbackPtr]( RequestFailTypeOpt failTypeOpt ) {
        m_status = Status::Idle;
        onRequestsFinished( callbackPtr, failTypeOpt );
    };

    if( index >= m_description.size() ) {
        onFinished( {} );
        return true;
    }

    const auto firstScale =
        utils::findFirstBit( &m_description[index].scalemask, sizeof( m_description[index].scalemask ) );
    if( !firstScale ) {
        ZNET_LOGW( "cc_0x31: requestValue: error: Supported scales is empty!\n\n" );
        return requestNextComponent( callbackPtr, context, index + 1 );
    }

    ZNET_LOGD( "cc_0x31: get: nodeId: 0x%02X, channelId: 0x%02x, type: %u\n",
               getParentNode().getId(),
               getParentChannel().getId(),
               m_description[index].type );

    return getParentNode().getRequestsManager().doRequest(
        MultiLevelSensorRequest( getNodeId(), getChannelId(), m_description[index].type, *firstScale, getVersion() ),
        context,
        [=]( const znet_multilevel_sensor_report_t& value ) {
            if( !requestNextComponent( callbackPtr, context, index + 1 ) ) {
                onFinished( RequestFailType::SendingError );
            }
        },
        onFinished );
}

bool ZCc0x31SensorMultilevel::storeData( IFile& file ) const {
    bool result = true;

    const uint8_t numSensor = static_cast<uint8_t>( m_description.size() );
    result = result && file.write( &numSensor, sizeof( numSensor ) );

    for( auto desc : m_description ) {
        result = result && file.write( &desc, sizeof( desc ) );
    }

    assert( result );

    return result;
}

bool ZCc0x31SensorMultilevel::restoreData( IFile& file, int version ) {
    bool result = true;

    uint8_t numSensor = 0;
    result = result && file.read( &numSensor, sizeof( numSensor ) );

    m_description.reserve( numSensor );
    for( size_t n = 0; n < numSensor; n++ ) {
        SensorDescription desc;
        result = result && file.read( &desc, sizeof( desc ) );
        desc.scalemask &= ( ( 1 << MultilevelSensor::MAX_SCALES ) - 1 );
        m_description.push_back( desc );
    }

    assert( result );

    return result;
}

std::vector<uint8_t> ZCc0x31SensorMultilevel::getSupportedScales( const SensorDescription& desc ) {
    std::vector<uint8_t> supportedScales;
    supportedScales.reserve( utils::getBitsCount( &desc.scalemask, sizeof( desc.scalemask ) ) );
    utils::forEachBit(
        &desc.scalemask, sizeof( desc.scalemask ), [&]( uint8_t index ) { supportedScales.push_back( index ); } );

    return supportedScales;
}

bool ZCc0x31SensorMultilevel::isValidSensorType( MultilevelSensor::Type type ) {
    return MultilevelSensor::Reserved < type && type < MultilevelSensor::Count;
}

std::vector<ZCc0x31SensorMultilevel::SensorInfo> ZCc0x31SensorMultilevel::getSupportedSensorsInfo() const {
    std::vector<ZCc0x31SensorMultilevel::SensorInfo> supportedSensors;
    supportedSensors.reserve( m_description.size() );
    for( const auto& desc : m_description ) {
        supportedSensors.push_back( { static_cast<MultilevelSensor::Type>( desc.type ), getSupportedScales( desc ) } );
    }

    return supportedSensors;
}

}  // namespace znet
