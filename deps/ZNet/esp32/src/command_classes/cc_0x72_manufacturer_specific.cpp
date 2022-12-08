#include "znet/cc_0x72_manufacturer_specific.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x72ManufacturerSpecific::requestValue( const RequestValue& request ) {
    const bool result = requestValueImpl( *this, ManufacturerSpecificRequest( getNodeId(), getChannelId() ), request );

    if( !result ) {
        ZNET_LOGE( "manufacturer specific get error, node %u\n", getNodeId() );
    }
    return result;
}

bool ZCc0x72ManufacturerSpecific::requestManufacturerSpecific(
    std::function<void( const znet_manufacturer_specific_report_t& )> onSuccess,
    OnFailCallback onFail,
    WeakJobContext context ) {
    return getParentNode().getRequestsManager().doRequest( ManufacturerSpecificRequest( getNodeId(), getChannelId() ),
                                                           context,
                                                           std::move( onSuccess ),
                                                           std::move( onFail ) );
}

}  // namespace znet
