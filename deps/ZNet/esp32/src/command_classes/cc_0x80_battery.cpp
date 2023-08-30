#include "znet/cc_0x80_battery.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

bool ZCc0x80Battery::requestValue( const RequestValue& request ) {
    const bool result = requestValueImpl( *this, BatteryLevelRequest( getNodeId(), getChannelId() ), request );

    if( !result ) {
        ZNET_LOGE( "batteru level get error, node %u\n", getNodeId() );
    }
    return result;
}

bool ZCc0x80Battery::storeData( IFile& file ) const {
    return file.write( &m_needForcedRequest, sizeof( m_needForcedRequest ) );
}

bool ZCc0x80Battery::restoreData( IFile& file, int version ) {
    bool result = file.read( &m_needForcedRequest, sizeof( m_needForcedRequest ) );
    if( result && m_needForcedRequest ) {
        getParentNode().setForcedRequests( true );
    }
    return result;
}

void ZCc0x80Battery::setForcedRequest( bool needForcedRequest ) {
    m_needForcedRequest = needForcedRequest;
}

bool ZCc0x80Battery::hasForcedRequest() {
    return m_needForcedRequest;
}

}  // namespace znet
