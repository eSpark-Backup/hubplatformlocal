#include "znet/cc_0x9c_alarm_sensor.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"
#include "znet/IFile.hpp"

#include <assert.h>
#include <string.h>

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x9cAlarmSensor::interview( InterviewCallback callback, WeakJobContext context ) {
    return singleInterviewRequestImpl( *this,
                                       AlarmSensorSupportedTypesRequest( getNodeId(), getChannelId() ),
                                       context,
                                       [this]( const uint8_t* data, uint8_t len ) {
                                           assert( len <= 1u );

                                           m_sensorTypeMask = len ? *data : uint8_t( 0 );
                                       },
                                       std::move( callback ) );
}

bool ZCc0x9cAlarmSensor::requestValue( const RequestValue& request ) {
    if( const auto alarmSensorValue = request.toRequestValueAlarmSensor() ) {
        return utils::checkBit( &m_sensorTypeMask, sizeof( m_sensorTypeMask ), alarmSensorValue->sensorType ) &&
               requestValueImpl(
                   *this, AlarmSensorRequest( getNodeId(), getChannelId(), alarmSensorValue->sensorType ), request );
    }
    else {
        return m_sensorTypeMask != 0 &&
               requestNextComponent( request.resultCallback ?
                                         std::make_shared<RequestValue::Callback>( request.resultCallback ) :
                                         nullptr,
                                     request.jobContext );
    }
}

bool ZCc0x9cAlarmSensor::requestNextComponent( RequestValue::CallbackPtr callbackPtr,
                                               WeakJobContext jobContext,
                                               uint8_t index ) {
    const auto firstBit = utils::findFirstBit( &m_sensorTypeMask, sizeof( m_sensorTypeMask ), index );
    if( !firstBit ) {
        onRequestsFinished( callbackPtr, {} );
        return true;
    }

    index = *firstBit;

    return getParentNode().getRequestsManager().doRequest(
        AlarmSensorRequest( getNodeId(), getChannelId(), index ),
        jobContext,
        [=]( uint8_t, uint16_t ) {
            if( !requestNextComponent( callbackPtr, jobContext, index + 1 ) ) {
                onRequestsFinished( callbackPtr, RequestFailType::SendingError );
            }
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

std::vector<AlarmSensorType> ZCc0x9cAlarmSensor::getSupportedTypes() const {
    std::vector<AlarmSensorType> result;
    result.reserve( utils::getBitsCount( &m_sensorTypeMask, sizeof( m_sensorTypeMask ) ) );
    utils::forEachBit( &m_sensorTypeMask, sizeof( m_sensorTypeMask ), [&result]( uint8_t type ) {
        result.push_back( static_cast<AlarmSensorType>( type ) );
    } );
    return result;
}

bool ZCc0x9cAlarmSensor::storeData( IFile& file ) const {
    return file.write( &m_sensorTypeMask, sizeof( m_sensorTypeMask ) );
}

bool ZCc0x9cAlarmSensor::restoreData( IFile& file, int version ) {
    return file.read( &m_sensorTypeMask, sizeof( m_sensorTypeMask ) );
}

}  // namespace znet
