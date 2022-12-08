#include "znet_time_synchronization_handler.hpp"
#include "znet_impl.hpp"

namespace znet {

void TimeSynchronizationHandler::initialize() {
    m_parentMesh.getTimeManager().startNodesProcessing();
    m_parentMesh.setState( IZNet::Mode::TimeSynchronization, ModeStatus::Started );
}

void TimeSynchronizationHandler::onDeactivated() {
    m_parentMesh.getTimeManager().stopNodesProcessing();
}

bool TimeSynchronizationHandler::onLoop() {
    if( !m_parentMesh.getTimeManager().isInProgress() ) {
        m_parentMesh.activateMode( IZNet::Mode::Monitoring );
        m_parentMesh.setState( IZNet::Mode::TimeSynchronization, ModeStatus::Finished );
    }

    return ZNetHandler::onLoop();
}

bool TimeSynchronizationHandler::canHandleNode( znet_node_id_t nodeId ) const {
    return m_parentMesh.getTimeManager().getCurrentNodeId() == nodeId && nodeId != 0;
}

}  // namespace znet
