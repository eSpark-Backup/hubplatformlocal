#include "znet_state_accessor.h"

extern "C" {
extern uint8_t MY_NODE_ID;
extern uint8_t SIS_id;

extern uint8_t Z_WAVE_LIBRARY_TYPE;
extern uint8_t Z_WAVE_PROTOCOL_VERSION;
extern uint8_t Z_WAVE_PROTOCOL_SUB_VERSION;
}

namespace znet {
namespace ut {

ZNetStateAccessor::ZNetStateAccessor()
: m_selfNodeId( MY_NODE_ID )
, m_sisNodeId( SIS_id )
, m_zwaveLibraryType( Z_WAVE_LIBRARY_TYPE )
, m_zwaveProtocolVersion( Z_WAVE_PROTOCOL_VERSION )
, m_zwaveProtocolSubVersion( Z_WAVE_PROTOCOL_SUB_VERSION ) {}

void ZNetStateAccessor::setSelfNodeId( uint8_t nodeId ) {
    m_selfNodeId = nodeId;
}

void ZNetStateAccessor::setSisNodeId( uint8_t nodeId ) {
    m_sisNodeId = nodeId;
}

void ZNetStateAccessor::setZwaveLibraryType( uint8_t type ) {
    m_zwaveLibraryType = type;
}

void ZNetStateAccessor::setZwaveProtocolVersion( uint8_t version, uint8_t subversion ) {
    m_zwaveProtocolVersion = version;
    m_zwaveProtocolSubVersion = subversion;
}

}  // namespace ut
}  // namespace znet
