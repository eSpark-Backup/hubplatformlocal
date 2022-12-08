#include "znet_nodes_processor.hpp"

#include "znet/iznet_date_time_provider.hpp"
#include "znet/znet_node.hpp"
#include "znet/cc_0x8a_time.hpp"
#include "znet/cc_0x8b_time_parameters.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <algorithm>

namespace znet {

class NodesProcessor::JobImpl : public Job {
public:
    JobImpl( NodesProcessor& owner )
    : m_owner( owner ) {}

    void onContextLocked( JobsHolderID id, JobContextPtr context ) override {
        m_owner.onContextLocked( id, context );
    }
    void onContextExpired( JobsHolderID id ) override {
        m_owner.onContextExpired( id );
    }

private:
    NodesProcessor& m_owner;
};

NodesProcessor::NodesProcessor( INodesProvider& nodesProvider,
                                ITimerProvider& timerProvider,
                                std::chrono::milliseconds waitingInterval,
                                JobPriorityOpt priorityOpt )
: m_nodesProvider( nodesProvider )
, m_timerProvider( timerProvider )
, m_waitingInterval( waitingInterval )
, m_priorityOpt( priorityOpt )
, m_job( std::make_shared<JobImpl>( *this ) ) {}

NodesProcessor::~NodesProcessor() {
    stopNodesProcessing();
}

void NodesProcessor::addNodeToProcess( znet_node_id_t nodeId, bool force ) {
    assert( nodeId != InvalidNodeId );

    const bool isCurrentNode = nodeId == m_currentNodeId;

    utils::setBit( m_nodeIds.mask, nodeId );
    m_maskStartBit = std::min( m_maskStartBit, uint16_t( nodeId ) );
    if( force ) {
        m_forceNodeId = nodeId;
    }

    onNodesAddedToProcess( isCurrentNode );
}

void NodesProcessor::addNodesToProcess( const NodeIdsSet& nodeIds ) {
    size_t index = 0;
    for( auto& mask : m_nodeIds.mask ) {
        mask |= nodeIds.mask[index++];
    }
    m_maskStartBit = 0;

    assert( !utils::checkBit( m_nodeIds.mask, InvalidNodeId ) );

    onNodesAddedToProcess( utils::checkBit( nodeIds.mask, m_currentNodeId ) );
}

void NodesProcessor::removeNodesFromProcess( const NodeIdsSet& nodeIds ) {
    size_t index = 0;
    for( auto& mask : m_nodeIds.mask ) {
        mask &= ~nodeIds.mask[index++];
    }

    if( isInProgress() && utils::checkBit( nodeIds.mask, m_currentNodeId ) ) {
        // TODO: investigate: is it proper implementation or do we need to call onNodeProcessed here?

        while( auto temp = std::move( m_jobContext ) )
            ;  //  skip all for current node

        processNextNode();
    }
}

void NodesProcessor::addAllNodesToProcess() {
    m_nodeIds = m_nodesProvider.getNodeIds();
    m_maskStartBit = 0;
    assert( !utils::checkBit( m_nodeIds.mask, InvalidNodeId ) );

    onNodesAddedToProcess( utils::checkBit( m_nodeIds.mask, m_currentNodeId ) );
}

bool NodesProcessor::isInProgress() const {
    return m_currentNodeId != InvalidNodeId;
}

void NodesProcessor::startNodesProcessing() {
    if( isDirty() && !isInProgress() ) {
        processNextNode();
    }
}

bool NodesProcessor::isDirty() const {
    return m_maskStartBit != NodeIdsBitsCount;
}

const NodesProcessor::NodeIdsSet& NodesProcessor::getProcessingNodeIds() const {
    return m_nodeIds;
}

bool NodesProcessor::isNodeAddedToProcess( znet_node_id_t nodeId ) const {
    return utils::checkBit( m_nodeIds.mask, nodeId );
}

bool NodesProcessor::trySetupCurrentNode( znet_node_id_t nodeId ) {
    assert( !m_jobContext );
    m_currentNodeId = nodeId;
    if( auto node = getCurrentNode() ) {
        node->getJobs().lockJobContext( m_priorityOpt, m_job );

        if( auto jobSecondHolder = m_jobContext ) {
            assert( !jobSecondHolder->isExpired() );

            updateTimePoint();

            assert( m_nodeProcessingState == NodeProcessingState::Default );

            m_nodeProcessingState = NodeProcessingState::Started;

            auto resetNodeProcessingState = []( NodeProcessingState* state ) { *state = NodeProcessingState::Default; };

            std::unique_ptr<NodeProcessingState, decltype( resetNodeProcessingState )> nodeProcessingStateGuard(
                &m_nodeProcessingState, resetNodeProcessingState );

            if( processNode( *node ) && m_nodeProcessingState != NodeProcessingState::Interrupted ) {
                return true;
            }

            nodeProcessingStateGuard.reset();
            m_jobContext.reset();
        }
    }

    utils::unsetBit( m_nodeIds.mask, m_currentNodeId );
    return false;
}

void NodesProcessor::processNextNode() {
    assert( !m_jobContext );

    ZNode* acceptableNode = nullptr;
    if( m_forceNodeId != InvalidNodeId && utils::checkBit( m_nodeIds.mask, m_forceNodeId ) ) {
        auto node = m_nodesProvider.onFindNode( m_forceNodeId );
        if( node ) {
            acceptableNode = node;
            m_forceNodeId = InvalidNodeId;
        }
    }

    if( !acceptableNode ) {
        ZNode* firstNode = nullptr;

        utils::findBit(
            m_nodeIds.mask,
            NodeIdsSet::Size,
            [&]( znet_node_id_t nodeId ) {
                auto node = m_nodesProvider.onFindNode( nodeId );
                if( !node ) {
                    utils::unsetBit( m_nodeIds.mask, nodeId );
                    return false;
                }

                if( !firstNode ) {
                    firstNode = node;
                }

                const auto priorityOpt = node->getJobs().getCurrentJobPriority();
                if( !priorityOpt.second ) {
                    acceptableNode = node;
                    return true;
                }

                if( !acceptableNode || priorityOpt.first < acceptableNode->getJobs().getCurrentJobPriority().first ) {
                    acceptableNode = node;
                }
                return false;
            },
            static_cast<uint8_t>( m_maskStartBit ) );

        m_maskStartBit = firstNode ? firstNode->getId() : InvalidNodeId;
    }

    znet_node_id_t nodeId = acceptableNode ? acceptableNode->getId() : InvalidNodeId;
    for( ; !trySetupCurrentNode( nodeId ); nodeId = static_cast<znet_node_id_t>( m_maskStartBit ) ) {
        const auto found =
            utils::findFirstBit( m_nodeIds.mask, NodeIdsSet::Size, static_cast<uint8_t>( m_maskStartBit ) );

        if( !found ) {
            m_maskStartBit = NodeIdsBitsCount;
            stopNodesProcessing();
            return;
        }

        m_maskStartBit = *found;
    }
}

void NodesProcessor::onReportWaitingExpired() {
    onNodeProcessed( err::message( ZNET_ERR_REPORT_EXPIRED ) );
}

void NodesProcessor::onNodeProcessed( ErrorOpt err ) {
    if( err ) {
        ZNET_LOGE( "%s\n", utils::stringify( *err ).c_str() );
    }
    // trySetupCurrentNode holds job context
    if( m_nodeProcessingState == NodeProcessingState::Started ) {
        m_nodeProcessingState = NodeProcessingState::Interrupted;
        return;
    }

    while( auto temp = std::move( m_jobContext ) )
        ;  //  skip all for current node

    if( isInProgress() ) {
        utils::unsetBit( m_nodeIds.mask, m_currentNodeId );
        processNextNode();
    }
}

ZNode* NodesProcessor::getCurrentNode() {
    return isInProgress() ? m_nodesProvider.onFindNode( m_currentNodeId ) : nullptr;
}

const ZNode* NodesProcessor::getCurrentNode() const {
    return isInProgress() ? m_nodesProvider.onFindNode( m_currentNodeId ) : nullptr;
}

INodesProvider& NodesProcessor::getNodesProvider() {
    return m_nodesProvider;
}

znet_node_id_t NodesProcessor::getCurrentNodeId() const {
    return m_currentNodeId;
}

WeakJobContext NodesProcessor::getJobContext() const {
    return { m_jobContext };
}

void NodesProcessor::updateTimePoint() {
    if( !m_timer ) {
        m_timer = m_timerProvider.createTimer( std::bind( &NodesProcessor::onReportWaitingExpired, this ) );
    }

    m_timer->runOnce( m_waitingInterval );
}

void NodesProcessor::onContextLocked( znet_node_id_t nodeId, JobContextPtr context ) {
    assert( WeakJobContext( context ) );
    if( nodeId == m_currentNodeId ) {
        assert( !WeakJobContext( m_jobContext ) );
        m_jobContext = std::move( context );
    }
    else {
        context.reset();  // reset before for autostart case in addNodeToProcess
        addNodeToProcess( nodeId );
    }
}

void NodesProcessor::onContextExpired( znet_node_id_t nodeId ) {
    assert( nodeId == m_currentNodeId );
    assert( !WeakJobContext( m_jobContext ) );

    m_jobContext.reset();

    if( isInProgress() ) {
        utils::unsetBit( m_nodeIds.mask, m_currentNodeId );
        processNextNode();
    }
}

void NodesProcessor::onNodesAddedToProcess( bool isCurrentNodeReseted ) {
    if( isCurrentNodeReseted && isInProgress() ) {
        // TODO: prepare user codes request and uncomment this code

        //        assert( m_jobContext );
        //        if( auto node = getCurrentNode() ) {
        //            node->getJobs().lockJobContext( m_priority, m_job );
        //            auto temp = std::move( m_jobContext );
        //        }
    }

    // TODO: add autostart here
}

void NodesProcessor::stopNodesProcessing() {
    while( auto temp = std::move( m_jobContext ) )
        ;  //  skip all for current node

    m_timer.reset();

    m_currentNodeId = InvalidNodeId;
}

}  // namespace znet
