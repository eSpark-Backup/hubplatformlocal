#include <znet/znet_command_class.hpp>
#include <znet/znet_defs.h>
#include <znet/znet_node.hpp>
#include <znet/znet_command_class_utils.hpp>
#include <znet/cc_0x2b_scene_activation.hpp>

extern "C" {
#include <znet_log.h>
}

#include <algorithm>

namespace znet {

ZClass::ZClass( ZChannel& parentChannel, const znet_command_class_version_t version )
: m_parentChannel( parentChannel )
, m_version( version ) {}

znet_command_class_t ZClass::getId() const {
    return getCommandClassId( *this );
}

ZChannel& ZClass::getParentChannel() const {
    return m_parentChannel;
}

ZNode& ZClass::getParentNode() const {
    return getParentChannel().getParentNode();
}

void ZClass::setParentChannel( ZChannel& parentChannel ) {
    m_parentChannel = parentChannel;
}

void ZClass::setVersion( const znet_command_class_version_t value ) {
    m_version = value;
}

znet_command_class_version_t ZClass::getVersion() const {
    return m_version;
}

bool ZClass::setToken( const std::string& value ) {
    ZNET_LOGE( "ZClass::setToken: no support, CC = 0x%x\n", getId() );
    return false;
}

bool ZClass::canRequest() const {
    return !getParentNode().isWakeup() || getParentNode().isWokenUp();
}

bool ZClass::setValue( const uint8_t value ) {
    return false;
}

bool ZClass::setValueMulticast( const uint8_t nodeQuantity,
                                const znet_node_id_t* listNodeId,
                                const uint8_t value,
                                bool sendSingleCasts ) {
    return false;
}

bool ZClass::setTokenMulticast( const uint8_t nodeQuantity,
                                const znet_node_id_t* listNodeId,
                                const std::string& value,
                                bool sendSingleCasts ) {
    return false;
}

bool ZClass::requestValue( const RequestValue& request ) {
    return false;
}

bool ZClass::setRgb( ColorComponents& ) {
    return false;
}

bool ZClass::setRgbMulticast( const uint8_t nodeQuantity,
                              const znet_node_id_t* listNodeId,
                              ColorComponents& components,
                              bool sendSingleCasts ) {
    return false;
}

bool ZClass::interview( InterviewCallback callback, WeakJobContext context ) {
    return false;
}

bool ZClass::hasInterviewDone() const {
    return m_interviewDone;
}

void ZClass::setInterviewDone( const bool value ) {
    m_interviewDone = value;
}

bool ZClass::storeData( IFile& file ) const {
    return true;
}

bool ZClass::restoreData( IFile& file, int version ) {
    return true;
}

bool ZClass::isRediscoverRequired() const {
    return false;
}

znet_node_id_t ZClass::getNodeId() const {
    return getParentNode().getId();
}

znet_node_channel_id_t ZClass::getChannelId() const {
    return getParentChannel().getId();
}

}  // namespace znet
