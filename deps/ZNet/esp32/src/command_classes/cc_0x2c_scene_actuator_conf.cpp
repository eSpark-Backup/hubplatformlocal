#include "znet/znet_node.hpp"
#include "znet/znet_command_class.hpp"
#include "znet/cc_0x2c_scene_actuator_conf.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x2CSceneActuatorConf::setConfiguration( uint8_t channelId,
                                                 uint8_t sceneId,
                                                 uint8_t dimmingDuration,
                                                 uint8_t isOverride,
                                                 uint8_t level ) {
    m_sceneId = sceneId;
    m_dimmingDuration = dimmingDuration;
    return ZNET_ERR_OK == znet_node_cmd_scene_actuator_configuration_set(
                              getParentNode().getId(), channelId, m_sceneId, m_dimmingDuration, isOverride, level );
}

bool ZCc0x2CSceneActuatorConf::requestValue( const RequestValue& request ) {
    const auto result =
        requestValueImpl( *this, SceneActuatorConfigurationRequest( getNodeId(), getChannelId(), m_sceneId ), request );
    if( !result ) {
        ZNET_LOGE( "cc_0x%x: get error, node %u\n", getId(), getNodeId() );
    }
    return result;
}

}  // namespace znet
