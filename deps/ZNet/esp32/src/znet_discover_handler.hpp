#pragma once

#include "znet_handler_interview_base.hpp"
#include "znet_nodes_processor.hpp"

namespace znet {

class DiscoverHandler : public InterviewHandlerBase, public NodesProcessor {
public:
    DiscoverHandler( ZNetImpl& parent );

    void initialize() override;

    void finalize( ErrorOpt ) override;

    void onDeactivated() override;

    bool onLoop() override;

    void setupNodeSetting( znet_node_id_t nodeId ) override;

    bool canHandleNode( znet_node_id_t nodeId ) const override;

protected:
    void sendInclusionProgress( const Step interviewStep ) override;

    bool processNode( ZNode& currentNode ) override;

    void onNodeProcessed( ErrorOpt ) override;

    WeakJobContext getJobContext() const override;

    bool requestNodeInfo( ZNode& currentNode );
};

}  // namespace znet
