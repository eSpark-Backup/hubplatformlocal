#include "znet/cc_0x30_binary_sensor.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <string.h>
#include <cassert>

namespace znet {

std::vector<ZCc0x30BinarySensor::SensorType> ZCc0x30BinarySensor::getSensorTypes() const {
    std::vector<SensorType> types;
    types.reserve(
        utils::getBitsCount( reinterpret_cast<const uint8_t*>( &m_sensorTypeMask ), sizeof( m_sensorTypeMask ) ) );
    utils::forEachBit( reinterpret_cast<const uint8_t*>( &m_sensorTypeMask ),
                       sizeof( m_sensorTypeMask ),
                       [&]( uint8_t index ) { types.push_back( static_cast<SensorType>( index ) ); } );

    return types;
}

bool ZCc0x30BinarySensor::interview( InterviewCallback callback, WeakJobContext context ) {
    ZNET_LOGD( "binary sensor interview started\n" );

    if( getVersion() <= 1 ) {
        return false;
    }
    else {
        return singleInterviewRequestImpl(
            *this,
            BinarySensorSupportedTypesRequest( getNodeId(), getChannelId(), getVersion() ),
            context,
            [this]( const uint8_t* data, uint8_t len ) {
                memcpy( &m_sensorTypeMask, data, std::min( len, uint8_t( sizeof( m_sensorTypeMask ) ) ) );
            },
            std::move( callback ) );
    }
}

/**
 *
 * @return
 */
bool ZCc0x30BinarySensor::requestValue( const RequestValue& request ) {
    ZNET_LOGD( "binary sensor requestValue started\n" );

    if( getVersion() <= 1 ) {
        const auto result = requestValueImpl( *this, BinarySensorRequestV1( getNodeId(), getChannelId() ), request );

        if( !result ) {
            ZNET_LOGE( "cc_0x30, v1: requestValue failed for node %u\n", getNodeId() );
        }

        return result;
    }
    else {
        return m_sensorTypeMask &&
               requestNextComponent( request.resultCallback ?
                                         std::make_shared<RequestValue::Callback>( request.resultCallback ) :
                                         nullptr,
                                     request.jobContext );
    }
}

bool ZCc0x30BinarySensor::requestNextComponent( RequestValue::CallbackPtr callbackPtr,
                                                WeakJobContext jobContext,
                                                uint8_t index ) {
    const auto firstBit =
        utils::findFirstBit( reinterpret_cast<const uint8_t*>( &m_sensorTypeMask ), sizeof( m_sensorTypeMask ), index );
    if( !firstBit ) {
        onRequestsFinished( callbackPtr, {} );
        return true;
    }

    index = *firstBit;

    return getParentNode().getRequestsManager().doRequest(
        BinarySensorRequest( getNodeId(), getChannelId(), index ),
        jobContext,
        [=]( uint8_t ) {
            if( !requestNextComponent( callbackPtr, jobContext, index + 1 ) ) {
                onRequestsFinished( callbackPtr, RequestFailType::SendingError );
            }
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

/**
 * @brief Store class data
 * @param file  Device file
 * @return status operation
 */
bool ZCc0x30BinarySensor::storeData( IFile& file ) const {
    return file.write( &m_sensorTypeMask, sizeof( m_sensorTypeMask ) );
}

/**
 * @brief Restore class data
 * @param file  Device file
 * @return status operation
 */
bool ZCc0x30BinarySensor::restoreData( IFile& file, int version ) {
    return file.read( &m_sensorTypeMask, sizeof( m_sensorTypeMask ) );
}

}  // namespace znet
