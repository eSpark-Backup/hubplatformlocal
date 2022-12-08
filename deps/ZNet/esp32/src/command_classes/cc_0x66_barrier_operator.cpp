#include "znet/cc_0x66_barrier_operator.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_utils.hpp"
#include "znet/converters/barrier_operator_state_converter.hpp"
#include "znet/enums/barrier_operator_state.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x66BarrierOperator::setValue( const uint8_t value ) {
    znet_error_t result = znet_node_cmd_barrier_operator_set( getNodeId(), getChannelId(), value );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: set: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

bool ZCc0x66BarrierOperator::requestValue( const RequestValue& request ) {
    auto jobContext = request.jobContext;
    auto callbackPtr =
        request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr;

    return getParentNode().getRequestsManager().doRequest(
        BarrierOperatorRequest( getNodeId(), getChannelId() ),
        jobContext,
        [jobContext, callbackPtr, this]( const znet_cmd_barrier_operator_value_t& result ) {
            requestNextSignal( callbackPtr, jobContext );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

bool ZCc0x66BarrierOperator::interview( InterviewCallback callback, WeakJobContext context ) {
    auto callbackPtr = std::make_shared<InterviewCallback>( std::move( callback ) );

    return getParentNode().getRequestsManager().doRequest(
        BarrierOperatorSupportedSignalsRequest( getNodeId(), getChannelId() ),
        context,
        [=]( const uint8_t* bitmask, uint8_t count ) {
            m_supportedSignals.clear();
            m_supportedSignals.reserve( utils::getBitsCount( bitmask, count ) );
            utils::forEachBit(
                bitmask, count, [this]( uint8_t bitIndex ) { m_supportedSignals.push_back( bitIndex + 1 ); } );

            requestNextSignal( callbackPtr, context );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ) );
}

void ZCc0x66BarrierOperator::setSignalValue( uint8_t signalId, uint8_t value ) {
    znet_node_cmd_barrier_operator_signal_set( getNodeId(), getChannelId(), signalId, value ? 0xFF : 0 );
}

bool ZCc0x66BarrierOperator::requestSignalValue( uint8_t signalId,
                                                 SignalCallback onSuccess,
                                                 OnFailCallback onFail,
                                                 WeakJobContext context ) {
    return getParentNode().getRequestsManager().doRequest(
        BarrierOperatorSignalRequest( getNodeId(), getChannelId(), signalId ),
        context,
        std::move( onSuccess ),
        std::move( onFail ) );
}

bool ZCc0x66BarrierOperator::setToken( const std::string& value ) {
    BarrierOperatorState state = barrier_operator_state_converter::stringToType( value.c_str() );
    if( state == BarrierOperatorState::Unknown ) {
        ZNET_LOGE( "cc_0x%x: set token: error unsupported token value: %s\n\n", getId(), value.c_str() );
        return false;
    }
    else {
        return setValue( static_cast<uint8_t>( state ) );
    }
}

bool ZCc0x66BarrierOperator::setTokenMulticast( const uint8_t nodeQuantity,
                                                const znet_node_id_t* listNodeId,
                                                const std::string& value,
                                                const bool sendSingleCasts ) {
    ZNET_LOGI( "set Barrier Operator Multi: value(%s)\n", value.c_str() );

    BarrierOperatorState state = barrier_operator_state_converter::stringToType( value.c_str() );
    if( state == BarrierOperatorState::Unknown ) {
        ZNET_LOGE( "cc_0x%x: set token multicast: error unsupported token value: %s\n\n", getId(), value.c_str() );
        return false;
    }
    else {
        znet_error_t result = znet_node_cmd_barrier_operator_set_multi(
            nodeQuantity, listNodeId, getParentChannel().getId(), static_cast<uint8_t>( state ), sendSingleCasts );
        if( ZNET_ERR_OK != result ) {
            ZNET_LOGE( "error: %i\n\n", result );
            return false;
        }
        return true;
    }
}

bool ZCc0x66BarrierOperator::storeData( IFile& file ) const {
    return utils::writeVectorToFile( file, m_supportedSignals );
}

bool ZCc0x66BarrierOperator::restoreData( IFile& file, int version ) {
    bool result = false;
    if( version >= majorFormatVersion_v11 ) {
        result = utils::readVectorFromFile( file, m_supportedSignals );
    }
    else {
        result = true;
    }

    return result;
}

void ZCc0x66BarrierOperator::requestNextSignal( InterviewCallbackPtr callbackPtr,
                                                WeakJobContext jobContext,
                                                uint8_t index ) {
    if( index >= m_supportedSignals.size() ) {
        setInterviewDone( true );
        onRequestsFinished( callbackPtr, {} );
        return;
    }

    getParentNode().getRequestsManager().doRequest(
        BarrierOperatorSignalRequest( getNodeId(), getChannelId(), m_supportedSignals[index] ),
        jobContext,
        [=]( uint8_t /*state*/ ) {
            // TODO: handle state
            requestNextSignal( callbackPtr, jobContext, index + 1 );
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
        DefaultPostponedRequestDelay );
}

const std::vector<uint8_t>& ZCc0x66BarrierOperator::getSupportedSignals() const {
    return m_supportedSignals;
}

}  // namespace znet
