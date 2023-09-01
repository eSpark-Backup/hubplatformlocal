#include <znet/znet_node.hpp>
#include <znet/cc_0x2b_scene_activation.hpp>

namespace znet {

bool ZCc0x2BSceneActivation::setActivation( uint8_t channelId, uint8_t sceneId, uint8_t value ) {
    znet_error_t res = znet_node_cmd_scene_activation_set( getParentNode().getId(), channelId, sceneId, value );
    if( ZNET_ERR_OK != res ) {
        return false;
    }
    return true;
}

}  // namespace znet
