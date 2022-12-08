#include "znet/cc_0x62_door_lock.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cassert>

namespace znet {

namespace {
template <typename Capabilities, typename Handler>
void forEachFeature( Capabilities& capabilities, Handler&& handler ) {
    handler( capabilities.isDoorSupported, 0 );
    handler( capabilities.isBoltSupported, 1 );
    handler( capabilities.isLatchSupported, 2 );
    handler( capabilities.isBlockToBlockSupported, 3 );
    handler( capabilities.isTwistAssistSupported, 4 );
    handler( capabilities.isHoldAndReleaseSupported, 5 );
    handler( capabilities.areTimeOperationsSupported, 6 );
    handler( capabilities.isAutoRelockSupported, 7 );
}
}  // namespace

bool ZCc0x62DoorLock::requestValue( const RequestValue& request ) {
    auto jobContext = request.jobContext;
    auto callbackPtr =
        request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr;

    return requestConfiguration(
        [jobContext, callbackPtr, this]( const znet_door_lock_configuration_t&,
                                         const znet_door_lock_extended_configuration_t* ) {
            if( !requestValueImpl(
                    *this, DoorLockOperationRequest( getNodeId(), getChannelId() ), callbackPtr, jobContext ) ) {
                onRequestsFinished( callbackPtr, RequestFailType::SendingError );
            }
        },
        std::bind( onRequestsFinished, callbackPtr, std::placeholders::_1 ),
        jobContext );
}

bool ZCc0x62DoorLock::interview( InterviewCallback callback, WeakJobContext context ) {
    if( getVersion() < 4 ) {
        return false;
    }

    return singleInterviewRequestImpl(
        *this,
        DoorLockCapabilitiesRequest( getNodeId(), getChannelId() ),
        context,
        [this]( const znet_door_lock_capabilities_t& report ) {
            m_capabilities.isBoltSupported = report.is_bolt_supported;
            m_capabilities.isDoorSupported = report.is_door_supported;
            m_capabilities.isLatchSupported = report.is_latch_supported;
            m_capabilities.isTwistAssistSupported = report.is_twist_assist_supported;
            m_capabilities.isHoldAndReleaseSupported = report.is_hold_and_release_supported;
            m_capabilities.areTimeOperationsSupported = report.are_timed_operations_supported;
            m_capabilities.isBlockToBlockSupported = report.is_block_to_block_supported;
            m_capabilities.isAutoRelockSupported = report.is_auto_relock_supported;
            m_capabilities.supportedInsideDoorHandlesBitmask = report.supported_inside_handle_ids;
            m_capabilities.supportedOutsideDoorHandlesBitmask = report.supported_outside_handle_ids;
            m_capabilities.supportedModes.reserve( report.supported_door_lock_modes_count );
            for( size_t i = 0; i < report.supported_door_lock_modes_count; ++i ) {
                m_capabilities.supportedModes.push_back( DoorLock::Mode( report.supported_door_lock_modes[i] ) );
            }
        },
        std::move( callback ) );
}

DoorCondition ZCc0x62DoorLock::decodeDoorCondition( uint8_t doorConditionMask ) {
    using namespace DoorLock;

    constexpr int LATCH_STATE_BIT = 2;
    constexpr int BOLT_STATE_BIT = 1;
    constexpr int DOOR_STATE_BIT = 0;

    return { ( ( doorConditionMask >> LATCH_STATE_BIT ) & 1 ) ? LatchState::Closed : LatchState::Open,
             ( ( doorConditionMask >> BOLT_STATE_BIT ) & 1 ) ? BoltState::Unlocked : BoltState::Locked,
             ( ( doorConditionMask >> DOOR_STATE_BIT ) & 1 ) ? DoorState::Closed : DoorState::Open };
}

const ZCc0x62DoorLock::Capabilities* ZCc0x62DoorLock::getCapabilities() const {
    return getVersion() >= 4 ? &m_capabilities : nullptr;
}

bool ZCc0x62DoorLock::storeData( IFile& file ) const {
    const uint8_t doorHandlesBitmask = m_capabilities.supportedInsideDoorHandlesBitmask |
                                       static_cast<uint8_t>( m_capabilities.supportedOutsideDoorHandlesBitmask << 4 );
    bool result = file.write( &doorHandlesBitmask, 1 );
    uint8_t flags = 0;
    forEachFeature( m_capabilities, [&flags]( bool feature, uint8_t index ) {
        if( feature ) {
            flags |= 1 << index;
        }
    } );
    result &= file.write( &flags, 1 );
    result &= utils::writeVectorToFile( file, m_capabilities.supportedModes );
    return result;
}

bool ZCc0x62DoorLock::restoreData( IFile& file, int version ) {
    bool result = true;
    if( version >= majorFormatVersion_v7 ) {
        uint8_t doorHandlesBitmask = 0;
        result &= file.read( &doorHandlesBitmask, 1 );
        m_capabilities.supportedInsideDoorHandlesBitmask = doorHandlesBitmask & 0xF;
        m_capabilities.supportedOutsideDoorHandlesBitmask = doorHandlesBitmask >> 4;

        uint8_t flags = 0;
        result &= file.read( &flags, 1 );
        forEachFeature( m_capabilities,
                        [flags]( bool& feature, uint8_t index ) { feature = ( flags & ( 1 << index ) ) != 0; } );

        result &= utils::readVectorFromFile( file, m_capabilities.supportedModes );
    }

    return result;
}

bool ZCc0x62DoorLock::requestConfiguration( ConfigurationCallback onSuccess,
                                            OnFailCallback onFail,
                                            WeakJobContext context ) {
    return getParentNode().getRequestsManager().doRequest( DoorLockConfigurationRequest( getNodeId(), getChannelId() ),
                                                           context,
                                                           std::move( onSuccess ),
                                                           std::move( onFail ) );
}

bool ZCc0x62DoorLock::requestOperation( OperationCallback onSuccess, OnFailCallback onFail, WeakJobContext context ) {
    return getParentNode().getRequestsManager().doRequest(
        DoorLockOperationRequest( getNodeId(), getChannelId() ), context, std::move( onSuccess ), std::move( onFail ) );
}

bool ZCc0x62DoorLock::setConfiguration( const znet_door_lock_configuration_t& configuration,
                                        const znet_door_lock_extended_configuration_t* extendedConfiguration ) {
    return ZNET_ERR_OK ==
           znet_node_cmd_door_lock_configuration_set(
               getNodeId(), getChannelId(), &configuration, getVersion() >= 4 ? extendedConfiguration : nullptr );
}

bool ZCc0x62DoorLock::setOperation( DoorLock::Mode mode ) {
    return ZNET_ERR_OK ==
           znet_node_cmd_door_lock_operation_set( getNodeId(), getChannelId(), static_cast<uint8_t>( mode ) );
}

}  // namespace znet
