#include "znet_handler_exclude.hpp"

#include "znet_impl.hpp"
extern "C" {
#include "znet_log.h"
}

namespace znet {

void ExcludeHandler::startExclude() {
    m_nodeId = 0;
    auto errCode = ::znet_node_rem();
    if( ZNET_ERR_OK == errCode ) {
        m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::InProgress );
    }
    else {
        m_parentMesh.setState(
            ::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::Failed, err::message( errCode ) );
        m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
    }
}

void ExcludeHandler::startExclude( const ::znet::ZNode& node ) {
    m_nodeId = node.getId();
    m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::Invoked );

    ::znet_error_t result;
    if( node.isWakeup() ) {
        result = ( node.isReachable() == ::znet::ReachableState::Reachable ) ?
                     ::znet_node_rem() :
                     ::znet_node_cmd_no_operation( node.getId() );
    }
    else {
        result = ::znet_node_cmd_no_operation( node.getId() );
    }
    if( result == ZNET_ERR_OK ) {
        m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::InProgress );
    }
    else {
        m_parentMesh.setState(
            ::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::Failed, err::message( result ) );
        m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
    }
}

void ExcludeHandler::stopExclude() {
    auto errCode = ::znet_node_rem_break();
    if( ZNET_ERR_OK == errCode ) {
        m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStopping, ::znet::ModeStatus::InProgress );
    }
    else {
        m_parentMesh.setState(
            ::znet::IZNet::Mode::ExcludeStopping, ::znet::ModeStatus::Failed, err::message( errCode ) );
        m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
    }
}

void ExcludeHandler::onNodeRemInvoked() {
    m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::Invoked );
}

void ExcludeHandler::onNodeRemBreak(::znet_error_t result, const ::znet_nodeinfo_t* nodeInfo ) {
    if( ZNET_ERR_TIME == result ) {
        m_parentMesh.setState(
            ::znet::IZNet::Mode::ExcludeStopping, ::znet::ModeStatus::Timeout, err::message( result ) );
    }
    else if( result != ZNET_ERR_OK ) {
        m_parentMesh.setState(
            ::znet::IZNet::Mode::ExcludeStopping, ::znet::ModeStatus::Failed, err::message( result ) );
    }
    else {
        m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStopping, ::znet::ModeStatus::Finished );
    }
    m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
}

void ExcludeHandler::onNodeNoOperation( znet_node_id_t nodeId, znet_error_t error ) {
    if( error == ZNET_ERR_REQUEST_SENT_OK ) {
        m_parentMesh.setReachableState( nodeId, true );
        m_parentMesh.setState(::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::Failed, err::message( error ) );
        m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
    }
    else {
        m_parentMesh.setReachableState( nodeId, false );
        const auto result = znet_node_rem_failed_node( nodeId );
        if( result != ZNET_ERR_OK ) {
            m_parentMesh.setState(
                ::znet::IZNet::Mode::ExcludeStarting, ::znet::ModeStatus::Failed, err::message( result ) );
            m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
        }
    }
}

void ExcludeHandler::onNodeRem(::znet_error_t err, const ::znet_nodeinfo_t* nodeInfo ) {
    if( err == ZNET_ERR_OK || nodeInfo ) {
        if( nodeInfo->node_id == 0 && m_nodeId > 0 ) {
            znet_nodeinfo_t nodeInfoStart;
            nodeInfoStart.node_id = m_nodeId;  // it needs only node_id then
            MonitoringBaseHandler::onNodeRem( err, &nodeInfoStart );
            return;
        }
    }
    MonitoringBaseHandler::onNodeRem( err, nodeInfo );
}

}  // namespace znet
