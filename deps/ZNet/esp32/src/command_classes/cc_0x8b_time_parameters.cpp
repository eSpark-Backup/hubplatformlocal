#include "znet/cc_0x8b_time_parameters.hpp"
#include "znet/znet_node.hpp"

#include "znet/znet_requests_manager.hpp"

namespace znet {

bool ZCc0x8BTimeParameters::setValue( const znet_date_t& dateUtc, const znet_time_t& timeUtc ) {
    return ZNET_ERR_OK == znet_node_cmd_time_parameters_set( getNodeId(), getChannelId(), &dateUtc, &timeUtc );
}

bool ZCc0x8BTimeParameters::requestValue( const RequestValue& request ) {
    return requestValueImpl( *this, TimeParametersRequest( getNodeId(), getChannelId() ), request );
}

bool ZCc0x8BTimeParameters::requestTimeParameters( std::function<void( const znet_date_time_t& )> onSuccess,
                                                   std::function<void( RequestFailType )> onFail,
                                                   WeakJobContext context ) const {
    return getParentNode().getRequestsManager().doRequest( TimeParametersRequest( getNodeId(), getChannelId() ),
                                                           context,
                                                           [=]( const znet_cmd_time_parameters_report_t& report ) {
                                                               onSuccess( { report.date_utc, report.time_utc } );
                                                           },
                                                           std::move( onFail ) );
}

}  // namespace znet
