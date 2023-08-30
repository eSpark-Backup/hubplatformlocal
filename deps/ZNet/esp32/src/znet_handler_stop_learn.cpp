#include "znet_impl.hpp"

#include "znet_handler_stop_learn.hpp"

namespace znet {

void StopLearnHandler::stopLearn() {
    auto errCode = ::znet_node_learn_break();
    if( ZNET_ERR_OK == errCode ) {
        m_parentMesh.setState(::znet::IZNet::Mode::LearnStopping, ::znet::ModeStatus::Invoked );
    }
    else {
        m_parentMesh.setState(::znet::IZNet::Mode::LearnStopping, ::znet::ModeStatus::Failed, err::message( errCode ) );
        m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
    }
}

void StopLearnHandler::onNodeLearnBreak(::znet_error_t result, ::znet_node_id_t nodeId ) {
    if( ZNET_ERR_TIME == result ) {
        m_parentMesh.setState(::znet::IZNet::Mode::LearnStopping, ::znet::ModeStatus::Timeout, err::message( result ) );
    }
    else if( result != ZNET_ERR_OK ) {
        m_parentMesh.setState(::znet::IZNet::Mode::LearnStopping, ::znet::ModeStatus::Failed, err::message( result ) );
    }
    else {
        m_parentMesh.setState(::znet::IZNet::Mode::LearnStopping, ::znet::ModeStatus::Finished );
    }
    m_parentMesh.activateMode(::znet::IZNet::Mode::Monitoring );
}

}  // namespace znet
