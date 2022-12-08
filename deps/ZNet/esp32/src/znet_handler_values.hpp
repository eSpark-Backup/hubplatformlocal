#pragma once

#include <znet_handler_monitoring_base.hpp>
#include <znet_nodes_processor.hpp>

namespace znet {

class CurrentValuesHandler : public MonitoringBaseHandler, public NodesProcessor {
    static const int OneStepWaitingTimeoutMs;

public:
    CurrentValuesHandler( ZNetImpl& parent );

    ~CurrentValuesHandler() = default;

    void initialize() override;

    void finalize( ErrorOpt ) override;

    void onDeactivated() override;

    bool onLoop() override;

    bool canHandleNode( znet_node_id_t nodeId ) const override;

    const std::vector<znet_command_class_t>& getRequestsCcOrder() const;

protected:
    bool processNode( ZNode& currentNode ) override;

    void onNodeProcessed( ErrorOpt ) override;

    void pushToRequestOrder( znet_command_class_t commandClassId );

    bool processCommandClasses( znet_node_channel_id_t channelId = 0, uint8_t ccIndex = 0 );

    void updateReachableState( ZNode& node, bool forceNotify = false ) override;

private:
    std::vector<znet_command_class_t> m_requestsCcOrder;
};

}  // namespace znet
