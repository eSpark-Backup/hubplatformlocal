#pragma once

#include <znet_handler_monitoring_base.hpp>
#include <znet_nodes_processor.hpp>

#include <unordered_map>

namespace znet {

enum class ModeStatus;

class NodeCheckHandler : public MonitoringBaseHandler, public NodesProcessor {
public:
    NodeCheckHandler( ZNetImpl& parent );

    ~NodeCheckHandler() = default;

    void initialize() override;

    void onDeactivated() override;

    bool onLoop() override;

    bool canHandleNode( znet_node_id_t nodeId ) const override;

    void addDataToCheck( uint8_t nodeId, std::vector<std::pair<uint8_t, uint8_t> > data );

protected:
    bool processNode( ZNode& currentNode ) override;

    void onReportWaitingExpired() override;

    void onNodeNoOperation( const znet_node_id_t nodeId, znet_error_t error ) override;

    bool processCommandClasses( size_t index = 0 );

    void updateReachableState( ZNode& node, bool forceNotify = false ) override;

private:
    void finishNodeProcessing( ModeStatus status );

    std::unordered_map<uint8_t, std::vector<std::pair<uint8_t, uint8_t> > > m_valuesToCheck;
};

}  // namespace znet
