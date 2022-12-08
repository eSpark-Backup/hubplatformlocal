#pragma once

#include <znet_handler_monitoring_base.hpp>

namespace znet {

class TimeSynchronizationHandler : public MonitoringBaseHandler {
public:
    using MonitoringBaseHandler::MonitoringBaseHandler;

    void initialize() override;

    void onDeactivated() override;

    bool onLoop() override;

    bool canHandleNode( znet_node_id_t nodeId ) const override;
};

};  // namespace znet
