#include "znet/cc_0x81_clock.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x81Clock::requestTime( std::function<void( const znet_clock_value_t& )> onSuccess,
                                std::function<void( RequestFailType )> onFail,
                                WeakJobContext context ) {
    return getParentNode().getRequestsManager().doRequest(
        ClockRequest( getParentNode().getId(), getParentChannel().getId() ),
        context,
        std::move( onSuccess ),
        std::move( onFail ) );
}

bool ZCc0x81Clock::setTime( const znet_clock_value_t& clockValue ) {
    return ZNET_ERR_OK == znet_node_cmd_clock_set( getParentNode().getId(),
                                                   getParentChannel().getId(),
                                                   clockValue.hour,
                                                   clockValue.minute,
                                                   clockValue.weekday );
}

}  // namespace znet
