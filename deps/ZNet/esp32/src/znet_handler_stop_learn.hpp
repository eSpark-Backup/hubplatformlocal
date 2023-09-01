#pragma once

#include "znet_handler_monitoring_base.hpp"

namespace znet {

class StopLearnHandler : public MonitoringBaseHandler {
public:
    using MonitoringBaseHandler::MonitoringBaseHandler;

    void stopLearn() override;
    void onNodeLearnBreak(::znet_error_t, ::znet_node_id_t ) override;
};

}  // namespace znet
