#pragma once

#include "znet_handler_monitoring_base.hpp"

namespace znet {

class ExcludeHandler : public MonitoringBaseHandler {
public:
    using MonitoringBaseHandler::MonitoringBaseHandler;

    void startExclude() override;
    void startExclude( const ::znet::ZNode& ) override;
    void stopExclude() override;

    void onNodeRem(::znet_error_t, const ::znet_nodeinfo_t* ) override;
    void onNodeRemInvoked() override;
    void onNodeRemBreak(::znet_error_t, const ::znet_nodeinfo_t* ) override;
    void onNodeNoOperation( znet_node_id_t, znet_error_t error ) override;

private:
    znet_node_id_t m_nodeId;
};

}  // namespace znet
