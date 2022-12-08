#pragma once

#include "znet_handler_monitoring_base.hpp"

namespace znet {

class MonitoringHandler : public MonitoringBaseHandler {
public:
    using MonitoringBaseHandler::MonitoringBaseHandler;

    void initialize() override;
    void onDeactivated() override;

    void onNodeAdd( const znet_nodeinfo_t* node_info ) override;
    void onNodeInclusionControllerInitiate( znet_node_id_t nodeId,
                                            znet_node_id_t childNodeId,
                                            znet_inclution_controller_step_id_t stepId ) override;

private:
    static bool hasSisRole();
};

}  // namespace znet
