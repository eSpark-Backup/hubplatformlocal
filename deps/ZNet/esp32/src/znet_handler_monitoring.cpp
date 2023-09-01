#include "znet_handler_monitoring.hpp"
#include "znet_impl.hpp"
extern "C" {
#include "znet_log.h"
}

namespace znet {

void MonitoringHandler::initialize() {
    if( hasSisRole() ) {
        ::znet_smart_start_listen_mode_enable();
    }
}

void MonitoringHandler::onDeactivated() {
    if( hasSisRole() ) {
        ::znet_smart_start_listen_mode_disable();
    }
}

void MonitoringHandler::onNodeAdd( const znet_nodeinfo_t* node_info ) {
    m_parentMesh.activateMode( IZNet::Mode::InclusionControllerStarting );
    m_parentMesh.getHandler().onNodeAdd( node_info );
}

void MonitoringHandler::onNodeInclusionControllerInitiate( znet_node_id_t nodeId,
                                                           znet_node_id_t childNodeId,
                                                           znet_inclution_controller_step_id_t stepId ) {
    m_parentMesh.activateMode( IZNet::Mode::InclusionControllerStarting );
    m_parentMesh.getHandler().onNodeInclusionControllerInitiate( nodeId, childNodeId, stepId );
}

bool MonitoringHandler::hasSisRole() {
    return ::znet_get_self_node_id() == ::znet_get_sis_node_id();
}

}  // namespace znet
