#include "znet/cc_0x26_switch_multi_level.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {
const int ZCc0x26SwitchMultiLevel::DurationMsToSec = 127000;
const int ZCc0x26SwitchMultiLevel::DurationMsToMin = 7560000;

bool ZCc0x26SwitchMultiLevel::setValue( const uint8_t value ) {
    ZNET_LOGI( "set MLSwitch: value(%d)\n", value );

    if( value != ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX_ABSOLUTE ) {
        m_target_level = value;
    }
    const znet_error_t result = znet_node_cmd_multilevel_switch_set(
        getNodeId(), getChannelId(), value, getVersion() > 1, ZNET_CMD_MULTILEVEL_SWITCH_DEFAULT_DURATION );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: set: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

bool ZCc0x26SwitchMultiLevel::setValueMulticast( const uint8_t nodeQuantity,
                                                 const znet_node_id_t* listNodeId,
                                                 const uint8_t value,
                                                 const bool sendSingleCasts ) {
    ZNET_LOGI( "set MLSwitch Multi: value(%d)\n", value );

    znet_error_t result = znet_node_cmd_multilevel_switch_set_multi( nodeQuantity,
                                                                     listNodeId,
                                                                     getChannelId(),
                                                                     value,
                                                                     getVersion() > 1,
                                                                     ZNET_CMD_MULTILEVEL_SWITCH_DEFAULT_DURATION,
                                                                     sendSingleCasts );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "error: %i\n\n", result );
        return false;
    }
    return true;
}

bool ZCc0x26SwitchMultiLevel::setLevelChange( ValueChangeCommand command, const SecondsOpt& duration ) {
    if( command == ValueChangeCommand::Stop ) {
        ZNET_LOGI( "cc_0x%x: try to stop SM_LevelChange\n\n", getId() );
        return stopLevelChange();
    }
    else {
        ZNET_LOGI( "cc_0x%x: try to start SM_LevelChange, param(0x%x)\n\n", getId(), ValueChangeCommand::Down );
        return startLevelChange(
            command == ValueChangeCommand::Down, ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN, duration );
    }
}

bool ZCc0x26SwitchMultiLevel::startLevelChange( bool downDirection,
                                                const uint8_t startValue,
                                                const SecondsOpt& seconds ) {
    if( ( downDirection && startValue == ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MIN ) ||
        ( !downDirection && startValue == ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX ) ) {
        return false;
    }

    m_target_level = downDirection ? ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MIN : ZNET_CMD_MULTILEVEL_SWITCH_VALUE_MAX;

    const znet_error_t result =
        znet_node_cmd_multilevel_switch_start_change( getNodeId(),
                                                      getChannelId(),
                                                      downDirection ? ZNET_CMD_MULTILEVEL_SWITCH_CHANGE_DIRECTION_DEC :
                                                                      ZNET_CMD_MULTILEVEL_SWITCH_CHANGE_DIRECTION_INC,
                                                      startValue,
                                                      utils::actuatorSetTimeFromSeconds( seconds ),
                                                      getVersion() );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: start_change: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

bool ZCc0x26SwitchMultiLevel::stopLevelChange() {
    const znet_error_t result = znet_node_cmd_multilevel_switch_stop_change( getNodeId(), getChannelId() );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: stop_change: error: %i\n\n", getId(), result );
        return false;
    }
    m_target_level = ZNET_CMD_MULTILEVEL_SWITCH_VALUE_UNKNOWN;
    return true;
}

bool ZCc0x26SwitchMultiLevel::interview( InterviewCallback callback, WeakJobContext context ) {
    if( getVersion() < 3 ) {
        return false;
    }

    return singleInterviewRequestImpl( *this,
                                       SwitchMultilevelSupportedTypesRequest( getNodeId(), getChannelId() ),
                                       context,
                                       [this]( const znet_multilevel_switch_supported_report_t& report ) {
                                           m_primaryType = report.primary_type;
                                           m_secondaryType = report.secondary_type;
                                       },
                                       std::move( callback ) );
}

bool ZCc0x26SwitchMultiLevel::storeData( IFile& file ) const {
    bool result = true;
    result = result && file.write( &m_primaryType, sizeof( m_primaryType ) );
    result = result && file.write( &m_secondaryType, sizeof( m_secondaryType ) );
    return result;
}

bool ZCc0x26SwitchMultiLevel::restoreData( IFile& file, int version ) {
    bool result = true;
    uint8_t param1 = 0;
    uint8_t param2 = 0;
    result = result && file.read( &param1, sizeof( param1 ) );
    result = result && file.read( &param2, sizeof( param2 ) );
    if( result ) {
        m_primaryType = param1;
        m_secondaryType = param2;
    }
    return result;
}

bool ZCc0x26SwitchMultiLevel::requestValue( const RequestValue& request ) {
    const bool result = requestValueImpl( *this, MultilevelSwitchValueRequest( getNodeId(), getChannelId() ), request );

    if( !result ) {
        ZNET_LOGE( "multilevel switch get error, node %u\n", getNodeId() );
    }
    return result;
}

uint8_t ZCc0x26SwitchMultiLevel::getPrimaryType() const {
    return m_primaryType;
}

uint8_t ZCc0x26SwitchMultiLevel::getSecondaryType() const {
    return m_secondaryType;
}

uint8_t ZCc0x26SwitchMultiLevel::getTargetLevel() {
    return m_target_level;
}

}  // namespace znet
