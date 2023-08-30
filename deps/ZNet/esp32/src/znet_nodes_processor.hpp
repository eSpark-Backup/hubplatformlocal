#pragma once

#include <znet/znet.h>
#include <znet/iznet_nodes_provider.hpp>
#include <znet/iznet_timer_provider.hpp>
#include <znet/znet_job.hpp>
#include <znet/znet_error_message.hpp>

#include <vector>
#include <chrono>
#include <utility>
#include <memory>

namespace znet {

class INodesProvider;
class ZNode;

namespace ut {
class FakeNodesProcessor;
}

static constexpr std::chrono::seconds DefaultTimeout( 15 );

class NodesProcessor {
    enum class NodeProcessingState : uint8_t { Default, Started, Interrupted };

public:
    using NodeIdsSet = INodesProvider::NodeIdsSet;

    explicit NodesProcessor( INodesProvider& nodesProvider,
                             ITimerProvider& timerProvider,
                             std::chrono::milliseconds waitingInterval = DefaultTimeout,
                             JobPriorityOpt priorityOpt = JobPriorityOpt( DefaultJobPriority, true ) );

    virtual ~NodesProcessor();

    void addNodeToProcess( znet_node_id_t nodeId, bool force = false );

    void addNodesToProcess( const NodeIdsSet& nodeIds );

    void removeNodesFromProcess( const NodeIdsSet& nodeIds );

    void addAllNodesToProcess();

    void startNodesProcessing();

    void stopNodesProcessing();

    bool isInProgress() const;

    bool isDirty() const;

    const NodeIdsSet& getProcessingNodeIds() const;

    bool isNodeAddedToProcess( znet_node_id_t nodeId ) const;

    znet_node_id_t getCurrentNodeId() const;

    WeakJobContext getJobContext() const;

protected:
    ZNode* getCurrentNode();

    const ZNode* getCurrentNode() const;

    void updateTimePoint();

    INodesProvider& getNodesProvider();

protected:
    virtual void onReportWaitingExpired();

    virtual bool processNode( ZNode& currentNode ) = 0;

    virtual void onNodeProcessed( ErrorOpt );

private:
    bool trySetupCurrentNode( znet_node_id_t nodeId );

    void processNextNode();

    void onContextLocked( znet_node_id_t nodeId, JobContextPtr context );

    void onContextExpired( znet_node_id_t nodeId );

    void onNodesAddedToProcess( bool isCurrentNodeReseted );

private:
    class JobImpl;
    static constexpr znet_node_id_t InvalidNodeId = 0;
    static constexpr uint16_t NodeIdsBitsCount = NodeIdsSet::Size * 8;

    INodesProvider& m_nodesProvider;
    ITimerProvider& m_timerProvider;

    NodeIdsSet m_nodeIds;
    znet_node_id_t m_forceNodeId = InvalidNodeId;

    uint16_t m_maskStartBit = NodeIdsBitsCount;
    znet_node_id_t m_currentNodeId = InvalidNodeId;

    ITimerProvider::TimerUPtr m_timer;
    std::chrono::milliseconds m_waitingInterval;

    NodeProcessingState m_nodeProcessingState = NodeProcessingState::Default;

    JobPriorityOpt m_priorityOpt;

    JobPtr m_job;
    JobContextPtr m_jobContext;

    friend class ut::FakeNodesProcessor;
};

}  // namespace znet
