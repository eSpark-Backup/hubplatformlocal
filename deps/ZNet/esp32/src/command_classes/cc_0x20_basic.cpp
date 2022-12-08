#include <znet/cc_0x20_basic.hpp>
#include <znet/znet_node.hpp>
#include <znet/znet_requests_manager.hpp>

extern "C" {
#include <znet_log.h>
}

#include <algorithm>

namespace znet {

namespace {
constexpr size_t DefaultInterviewRequestRetryNumber = 2;
constexpr size_t MinimalInterviewRequestRetryNumber = 0;

const std::initializer_list<znet_command_class_t> ActuatorCcIds = {
    ZNET_COMMAND_CLASS_BARRIER_OPERATOR,   ZNET_COMMAND_CLASS_SWITCH_BINARY,     ZNET_COMMAND_CLASS_SWITCH_COLOR,
    ZNET_COMMAND_CLASS_DOOR_LOCK,          ZNET_COMMAND_CLASS_SWITCH_MULTILEVEL, ZNET_COMMAND_CLASS_SOUND_SWITCH,
    ZNET_COMMAND_CLASS_THERMOSTAT_SETPOINT
};

}  // namespace

ZCc0x20Basic::ZCc0x20Basic( ZChannel& parent, znet_command_class_version_t version )
: ZClass( parent, version )
, m_isSupported( false ) {}

bool ZCc0x20Basic::setValue( const uint8_t value ) {
    const znet_error_t result = znet_node_cmd_basic_set( getNodeId(), getChannelId(), value );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: set: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

bool ZCc0x20Basic::requestValue( const RequestValue& request ) {
    if( !m_isSupported ) {
        return false;
    }

    const bool result = requestValueImpl( *this, BasicValueRequest( getNodeId(), getChannelId() ), request );

    if( !result ) {
        ZNET_LOGE( "basic get error, node %u\n", getNodeId() );
    }
    return result;
}

bool ZCc0x20Basic::interview( InterviewCallback callback, WeakJobContext context ) {
    auto hasSiblingActuatorCc =
        std::any_of( ActuatorCcIds.begin(), ActuatorCcIds.end(), [this]( znet_command_class_t ccId ) {
            return ccId != ID::value && static_cast<bool>( getParentChannel().getCommandClass( ccId ) );
        } );

    auto getMaxRetryNumber = [&]() {
        if( hasSiblingActuatorCc ) {
            return MinimalInterviewRequestRetryNumber;  // send request one, anyway set m_isSupported = false. Request
                                                        // is required by certification
        }

        if( getChannelId() == ZNET_CHANNEL_ID_ROOT ) {
            return DefaultInterviewRequestRetryNumber;
        }

        auto rootCc = getParentNode().getCommandClass<ZCc0x20Basic>( ZNET_CHANNEL_ID_ROOT );

        if( !rootCc || !rootCc->hasInterviewDone() ) {
            return MinimalInterviewRequestRetryNumber;  // request once ony if root intervew hasn't been done
        }

        return DefaultInterviewRequestRetryNumber;
    };

    return singleInterviewRequestImpl(
        *this,
        BasicValueRequest( getNodeId(), getChannelId() ),
        context,
        [this, hasSiblingActuatorCc]( znet_cmd_basic_value_t, znet_cmd_basic_value_t, uint8_t ) {
            m_isSupported = !hasSiblingActuatorCc;
        },
        std::move( callback ),
        getMaxRetryNumber() );
}

bool ZCc0x20Basic::storeData( IFile& file ) const {
    return file.write( &m_isSupported, sizeof( m_isSupported ) );
}

bool ZCc0x20Basic::restoreData( IFile& file, int version ) {
    bool result = true;

    if( version >= majorFormatVersion_v19 ) {
        result = result && file.read( &m_isSupported, sizeof( m_isSupported ) );
    }
    else {
        m_isSupported = true;
    }

    return result;
}

bool ZCc0x20Basic::isSupported() const {
    return m_isSupported;
}

bool ZCc0x20Basic::setValueMulticast( const uint8_t nodeQuantity,
                                      const znet_node_id_t* listNodeId,
                                      const uint8_t value,
                                      const bool sendSingleCasts ) {
    ZNET_LOGI( "set Basic Multi: value(%d)\n", value );

    znet_error_t result =
        znet_node_cmd_basic_set_multi( nodeQuantity, listNodeId, getChannelId(), value, sendSingleCasts );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "error: %i\n\n", result );
        return false;
    }
    return true;
}

}  // namespace znet
