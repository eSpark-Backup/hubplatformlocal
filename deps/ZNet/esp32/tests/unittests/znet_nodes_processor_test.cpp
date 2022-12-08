#include <znet_nodes_processor.hpp>
#include <znet/znet_utils.hpp>
#include <znet/znet_node.hpp>
#include <znet/znet_jobs_holder.hpp>

#include "fake_timer_provider.h"
#include "fake_nodes_provider.h"

#include <gtest/gtest.h>

#include <unordered_map>
#include <unordered_set>

namespace znet {
namespace ut {

class FakeNodesProcessor : public NodesProcessor {
public:
    using NodesProcessor::NodesProcessor;

    bool processNode( ZNode& node ) override {
        EXPECT_EQ( &node, getCurrentNode() );

        m_startedNodeIds.insert( node.getId() );

        auto customHandlerIt = m_customStartHandlers.find( node.getId() );

        return customHandlerIt == m_customStartHandlers.end() || customHandlerIt->second( node );
    }

    bool checkInitialState() const {
        return !isDirty() && !isInProgress() && getCurrentNodeId() == 0 && getCurrentNode() == nullptr &&
               checkNodes( {} );
    }

    bool checkNodes( const std::unordered_set<znet_node_id_t>& nodeIds ) const {
        for( auto id : nodeIds ) {
            if( !utils::checkBit( getProcessingNodeIds().mask, id ) ) {
                return false;
            }
        }

        return utils::getBitsCount( getProcessingNodeIds().mask, NodesProcessor::NodeIdsSet::Size ) == nodeIds.size();
    }

    void onNodeProcessed(::znet::ErrorOpt err ) override {
        ASSERT_NE( getCurrentNodeId(), 0 );

        m_processedNodeIds.insert( getCurrentNodeId() );

        NodesProcessor::onNodeProcessed( std::move( err ) );
    }

    bool checkProcessedNodes( const std::unordered_multiset<znet_node_id_t>& expected ) const {
        return m_processedNodeIds == expected;
    }

    void setCustomHandler( znet_node_id_t nodeId, std::function<bool( ZNode& )> handler ) {
        m_customStartHandlers.emplace( nodeId, std::move( handler ) );
    }

    void setNodeSkipped( znet_node_id_t nodeId ) {
        setCustomHandler( nodeId, []( ZNode& ) { return false; } );
    }

    bool checkStartedNodes( const std::unordered_multiset<znet_node_id_t>& expected ) const {
        return m_startedNodeIds == expected;
    }

    using NodesProcessor::getCurrentNode;
    using NodesProcessor::onNodeProcessed;

    bool isNodeIdInvalid() const {
        return getCurrentNodeId() == NodesProcessor::InvalidNodeId;
    }

private:
    std::unordered_map<znet_node_id_t, std::function<bool( ZNode& )>> m_customStartHandlers;
    std::unordered_multiset<znet_node_id_t> m_startedNodeIds;
    std::unordered_multiset<znet_node_id_t> m_processedNodeIds;
};

class NodesProcessorTestSuite : public ::testing::Test, public FakeNodesProvider, public FakeTimerProvider {
protected:
    std::unique_ptr<FakeNodesProcessor> createNodesProcessor(
        JobPriorityOpt priorityOpt = JobPriorityOpt( DefaultJobPriority, true ) ) {
        return std::unique_ptr<FakeNodesProcessor>(
            new FakeNodesProcessor( *this, *this, DefaultTimeout, priorityOpt ) );
    }
};

TEST_F( NodesProcessorTestSuite, emptyNodesProcessor ) {
    auto nodesProcessor = createNodesProcessor();
    EXPECT_TRUE( nodesProcessor->checkInitialState() );

    nodesProcessor->startNodesProcessing();
    EXPECT_TRUE( nodesProcessor->checkInitialState() );

    nodesProcessor->stopNodesProcessing();
    EXPECT_TRUE( nodesProcessor->checkInitialState() );
}

TEST_F( NodesProcessorTestSuite, addOneNodeToProcess ) {
    auto nodesProcessor = createNodesProcessor();

    nodeCreate( 111 );
    auto node = onFindNode( 111 );

    {
        nodesProcessor->addNodeToProcess( 111 );
        EXPECT_TRUE( nodesProcessor->isDirty() );
        EXPECT_FALSE( nodesProcessor->isInProgress() );
        EXPECT_EQ( nodesProcessor->getCurrentNodeId(), 0 );
        EXPECT_EQ( nodesProcessor->getCurrentNode(), nullptr );
        EXPECT_FALSE( node->getJobs().getJobContext() );
    }

    {
        nodesProcessor->startNodesProcessing();
        EXPECT_TRUE( nodesProcessor->isDirty() );
        EXPECT_TRUE( nodesProcessor->isInProgress() );
        EXPECT_EQ( nodesProcessor->getCurrentNodeId(), 111 );
        EXPECT_EQ( nodesProcessor->getCurrentNode(), node );
        EXPECT_TRUE( node->getJobs().getJobContext() );
    }

    {
        nodesProcessor->stopNodesProcessing();
        EXPECT_TRUE( nodesProcessor->isDirty() );
        EXPECT_FALSE( nodesProcessor->isInProgress() );
        EXPECT_EQ( nodesProcessor->getCurrentNodeId(), 0 );
        EXPECT_FALSE( node->getJobs().getJobContext() );
    }

    {
        nodesProcessor->startNodesProcessing();
        EXPECT_TRUE( nodesProcessor->isDirty() );
        EXPECT_TRUE( nodesProcessor->isInProgress() );
        EXPECT_EQ( nodesProcessor->getCurrentNodeId(), 111 );
        EXPECT_EQ( nodesProcessor->getCurrentNode(), node );
    }

    EXPECT_TRUE( checkAskedNullNodes( {} ) );
}

TEST_F( NodesProcessorTestSuite, severalNodesAndTwoProcessorsWithTheSamePriority ) {
    auto nodesProcessor1 = createNodesProcessor();
    auto nodesProcessor2 = createNodesProcessor();

    nodeCreate( 1 );
    nodeCreate( 2 );
    nodeCreate( 3 );

    nodesProcessor1->addAllNodesToProcess();
    nodesProcessor2->addAllNodesToProcess();

    // start 1st processor. It must work with 1st node
    {
        nodesProcessor1->startNodesProcessing();
        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 1 );
        EXPECT_TRUE( nodesProcessor1->isDirty() );
        EXPECT_TRUE( nodesProcessor1->isInProgress() );

        EXPECT_TRUE( checkJobContexts( { 1 } ) );
        EXPECT_TRUE( nodesProcessor1->checkNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor1->checkProcessedNodes( {} ) );
    }

    // start 2nd processor. Next node is 2nd (1st is busy)
    {
        nodesProcessor2->startNodesProcessing();
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 2 );
        EXPECT_TRUE( nodesProcessor2->isDirty() );
        EXPECT_TRUE( nodesProcessor2->isInProgress() );

        EXPECT_TRUE( checkJobContexts( { 1, 2 } ) );
        EXPECT_TRUE( nodesProcessor2->checkNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor2->checkProcessedNodes( {} ) );
    }

    // 1st node is processed by 1st. Next node is 3rd (2nd is busy)
    {
        nodesProcessor1->onNodeProcessed( {} );

        EXPECT_TRUE( nodesProcessor1->isDirty() );
        EXPECT_TRUE( nodesProcessor1->isInProgress() );
        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 3 );

        EXPECT_TRUE( checkJobContexts( { 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor1->checkNodes( { 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor2->checkNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor1->checkProcessedNodes( { 1 } ) );
    }

    // 2nd node is processed by 2nd. Next node is 1st.
    {
        nodesProcessor2->onNodeProcessed( {} );

        EXPECT_TRUE( nodesProcessor2->isDirty() );
        EXPECT_TRUE( nodesProcessor2->isInProgress() );

        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 1 );

        EXPECT_TRUE( checkJobContexts( { 1, 3 } ) );
        EXPECT_TRUE( nodesProcessor1->checkNodes( { 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor2->checkNodes( { 1, 3 } ) );

        EXPECT_TRUE( nodesProcessor2->checkProcessedNodes( { 2 } ) );
    }

    // 3rd node is processed by 1st. Next node is 2nd.
    {
        nodesProcessor1->onNodeProcessed( {} );
        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 2 );
        EXPECT_TRUE( nodesProcessor1->isDirty() );
        EXPECT_TRUE( nodesProcessor1->isInProgress() );

        EXPECT_TRUE( checkJobContexts( { 1, 2 } ) );
        EXPECT_TRUE( nodesProcessor1->checkNodes( { 2 } ) );
        EXPECT_TRUE( nodesProcessor2->checkNodes( { 1, 3 } ) );

        EXPECT_TRUE( nodesProcessor1->checkProcessedNodes( { 1, 3 } ) );
    }

    // all nodes are processed by 1st.
    {
        nodesProcessor1->onNodeProcessed( {} );
        EXPECT_TRUE( nodesProcessor1->checkInitialState() );

        EXPECT_TRUE( nodesProcessor1->checkProcessedNodes( { 1, 2, 3 } ) );
    }

    //
    {
        nodesProcessor2->onNodeProcessed( {} );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 3 );

        EXPECT_TRUE( nodesProcessor2->isDirty() );
        EXPECT_TRUE( nodesProcessor2->isInProgress() );

        EXPECT_TRUE( checkJobContexts( { 3 } ) );
        EXPECT_TRUE( nodesProcessor1->checkNodes( {} ) );
        EXPECT_TRUE( nodesProcessor2->checkNodes( { 3 } ) );

        EXPECT_TRUE( nodesProcessor2->checkProcessedNodes( { 1, 2 } ) );
    }

    // all nodes are processed by 2nd
    {
        nodesProcessor2->onNodeProcessed( {} );

        EXPECT_TRUE( nodesProcessor1->checkInitialState() );
        EXPECT_TRUE( nodesProcessor2->checkInitialState() );

        EXPECT_TRUE( nodesProcessor1->checkProcessedNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor2->checkProcessedNodes( { 1, 2, 3 } ) );
    }

    EXPECT_TRUE( checkAskedNullNodes( {} ) );
}

TEST_F( NodesProcessorTestSuite, severalNodesAndTwoProcessorsWithDifferentPriority ) {
    auto nodesProcessor1 = createNodesProcessor( JobPriorityOpt( 1, true ) );
    auto nodesProcessor2 = createNodesProcessor( JobPriorityOpt( 2, true ) );
    auto nodesProcessor3 = createNodesProcessor( JobPriorityOpt( 3, true ) );
    auto nodesProcessor4 = createNodesProcessor( JobPriorityOpt( 4, true ) );

    auto onLoop = [&]() {
        nodesProcessor4->startNodesProcessing();
        nodesProcessor3->startNodesProcessing();
        nodesProcessor2->startNodesProcessing();
        nodesProcessor1->startNodesProcessing();
    };

    nodeCreate( 1 );
    nodeCreate( 2 );
    nodeCreate( 3 );

    nodesProcessor1->addAllNodesToProcess();
    nodesProcessor2->addAllNodesToProcess();
    nodesProcessor3->addAllNodesToProcess();
    nodesProcessor4->addAllNodesToProcess();

    // there are enough nodes for 3 processors
    {
        nodesProcessor2->startNodesProcessing();
        nodesProcessor1->startNodesProcessing();
        nodesProcessor3->startNodesProcessing();

        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 2 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 1 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 3 );

        EXPECT_TRUE( checkJobContexts( { 1, 2, 3 } ) );

        EXPECT_TRUE( nodesProcessor1->checkNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor2->checkNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor3->checkNodes( { 1, 2, 3 } ) );
    }

    // there are not enough nodes for 3 processors. 4th processor should kick 1st.
    {
        nodesProcessor4->startNodesProcessing();

        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 0 );
        EXPECT_EQ( nodesProcessor4->getCurrentNodeId(), 2 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 1 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 3 );

        EXPECT_TRUE( checkJobContexts( { 1, 2, 3 } ) );
        EXPECT_FALSE( nodesProcessor1->isDirty() );

        EXPECT_TRUE( nodesProcessor1->checkNodes( {} ) );
        EXPECT_TRUE( nodesProcessor2->checkNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor3->checkNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor4->checkNodes( { 1, 2, 3 } ) );
    }

    {
        nodesProcessor2->onNodeProcessed( {} );

        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 0 );
        EXPECT_EQ( nodesProcessor4->getCurrentNodeId(), 2 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 0 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 3 );

        EXPECT_TRUE( checkJobContexts( { 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor1->isDirty() );
        EXPECT_FALSE( nodesProcessor1->isInProgress() );  // Auto start is not enabled ( handlers mode )

        EXPECT_TRUE( nodesProcessor1->checkNodes( { 1 } ) );
        EXPECT_TRUE( nodesProcessor2->checkNodes( {} ) );
        EXPECT_TRUE( nodesProcessor3->checkNodes( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor4->checkNodes( { 1, 2, 3 } ) );

        onLoop();  // start. Use autostart to avoid it

        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 1 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 0 );

        EXPECT_TRUE( checkJobContexts( { 1, 2, 3 } ) );
        EXPECT_TRUE( nodesProcessor1->isDirty() );

        nodesProcessor4->checkProcessedNodes( { 1 } );
    }

    {
        nodesProcessor4->onNodeProcessed( {} );
        onLoop();

        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 0 );
        EXPECT_EQ( nodesProcessor4->getCurrentNodeId(), 1 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 2 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 3 );

        EXPECT_TRUE( checkJobContexts( { 1, 2, 3 } ) );

        nodesProcessor4->checkProcessedNodes( { 2 } );
    }

    {
        nodesProcessor4->onNodeProcessed( {} );
        onLoop();

        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 0 );
        EXPECT_EQ( nodesProcessor4->getCurrentNodeId(), 3 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 2 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 1 );

        EXPECT_TRUE( checkJobContexts( { 1, 2, 3 } ) );

        nodesProcessor4->checkProcessedNodes( { 2, 1 } );
    }

    {
        nodesProcessor4->onNodeProcessed( {} );
        onLoop();

        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 3 );
        EXPECT_EQ( nodesProcessor4->getCurrentNodeId(), 0 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 2 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 1 );

        EXPECT_TRUE( checkJobContexts( { 1, 2, 3 } ) );

        nodesProcessor4->checkProcessedNodes( { 2, 1, 3 } );
    }
}

TEST_F( NodesProcessorTestSuite, nodeRemoved ) {
    auto nodesProcessor = createNodesProcessor();

    nodeCreate( 1 );
    nodeCreate( 11 );
    nodeCreate( 111 );

    nodesProcessor->addAllNodesToProcess();
    nodesProcessor->startNodesProcessing();

    nodeRemove( 11 );

    EXPECT_TRUE( nodesProcessor->checkNodes( { 1, 11, 111 } ) );
    EXPECT_TRUE( checkJobContexts( { 1 } ) );

    nodeRemove( 1 );

    EXPECT_TRUE( nodesProcessor->checkNodes( { 111 } ) );
    EXPECT_TRUE( checkJobContexts( { 111 } ) );
    EXPECT_TRUE( nodesProcessor->checkProcessedNodes( {} ) );

    EXPECT_TRUE( checkAskedNullNodes( { 11 } ) );
}

TEST_F( NodesProcessorTestSuite, skipNodes ) {
    auto nodesProcessor = createNodesProcessor();

    nodeCreate( 1 );
    nodeCreate( 11 );
    nodeCreate( 111 );

    nodesProcessor->setNodeSkipped( 1 );
    nodesProcessor->setNodeSkipped( 11 );

    nodesProcessor->addAllNodesToProcess();
    nodesProcessor->startNodesProcessing();

    EXPECT_TRUE( nodesProcessor->checkNodes( { 111 } ) );
    EXPECT_TRUE( checkJobContexts( { 111 } ) );

    EXPECT_TRUE( checkAskedNullNodes( {} ) );
}

TEST_F( NodesProcessorTestSuite, finishNodeProcessingInProcessNode ) {
    auto nodesProcessor = createNodesProcessor();

    nodeCreate( 9 );
    nodeCreate( 10 );
    nodeCreate( 42 );
    nodeCreate( 142 );

    nodesProcessor->setCustomHandler( 9, [&nodesProcessor]( ZNode& ) {
        nodesProcessor->onNodeProcessed( {} );
        return false;
    } );

    nodesProcessor->setCustomHandler( 42, [&nodesProcessor]( ZNode& ) {
        nodesProcessor->onNodeProcessed( {} );
        return true;
    } );

    nodesProcessor->addAllNodesToProcess();
    nodesProcessor->startNodesProcessing();

    EXPECT_EQ( nodesProcessor->getCurrentNodeId(), 10 );
    EXPECT_TRUE( nodesProcessor->checkProcessedNodes( { 9 } ) );
    EXPECT_TRUE( nodesProcessor->checkStartedNodes( { 9, 10 } ) );

    nodesProcessor->onNodeProcessed( {} );
    EXPECT_EQ( nodesProcessor->getCurrentNodeId(), 142 );
    EXPECT_TRUE( nodesProcessor->checkProcessedNodes( { 9, 10, 42 } ) );
    EXPECT_TRUE( nodesProcessor->checkStartedNodes( { 9, 10, 42, 142 } ) );
}

TEST_F( NodesProcessorTestSuite, withoutPriority ) {
    auto nodesProcessor1 = createNodesProcessor( JobPriorityOpt( JobPriority(), false ) );
    auto nodesProcessor2 = createNodesProcessor( JobPriorityOpt( JobPriority(), false ) );

    auto nodesProcessor3 = createNodesProcessor( JobPriorityOpt( 1, true ) );

    nodeCreate( 1 );
    nodeCreate( 2 );
    nodeCreate( 5 );
    nodeCreate( 11 );

    NodeIdsSet nodeIds;
    utils::setBit( nodeIds.mask, 2 );
    utils::setBit( nodeIds.mask, 11 );

    nodesProcessor1->addNodesToProcess( nodeIds );
    nodesProcessor2->addNodesToProcess( nodeIds );

    nodesProcessor1->startNodesProcessing();
    nodesProcessor2->startNodesProcessing();

    EXPECT_TRUE( checkJobContexts( {} ) );

    nodesProcessor3->addNodesToProcess( nodeIds );
    nodesProcessor3->startNodesProcessing();

    EXPECT_TRUE( nodesProcessor1->checkNodes( { 2, 11 } ) );
    EXPECT_TRUE( nodesProcessor2->checkNodes( { 2, 11 } ) );
    EXPECT_TRUE( nodesProcessor3->checkNodes( { 2, 11 } ) );

    EXPECT_TRUE( checkJobContexts( { 2 } ) );

    EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 2 );
    EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 2 );
    EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 2 );

    {
        nodesProcessor1->onNodeProcessed( {} );
        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 11 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 2 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 2 );
        EXPECT_TRUE( checkJobContexts( { 2 } ) );
    }

    {
        nodesProcessor2->onNodeProcessed( {} );
        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 11 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 11 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 2 );
        EXPECT_TRUE( checkJobContexts( { 2 } ) );
    }

    {
        nodesProcessor3->onNodeProcessed( {} );
        EXPECT_EQ( nodesProcessor1->getCurrentNodeId(), 11 );
        EXPECT_EQ( nodesProcessor2->getCurrentNodeId(), 11 );
        EXPECT_EQ( nodesProcessor3->getCurrentNodeId(), 11 );
        EXPECT_TRUE( checkJobContexts( { 11 } ) );
    }

    {
        nodeRemove( 11 );

        EXPECT_TRUE( nodesProcessor1->isNodeIdInvalid() );
        EXPECT_TRUE( nodesProcessor2->isNodeIdInvalid() );
        EXPECT_TRUE( nodesProcessor3->isNodeIdInvalid() );
    }
}

}  // namespace ut
}  // namespace znet
