#include "znet/cc_0x7e_anti_theft_unlock.hpp"
#include "znet/znet.h"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cassert>

namespace znet {

bool ZCc0x7EAntiTheftUnlock::unlock( const std::vector<uint8_t>& magicCode ) {
    return znet_node_cmd_anti_theft_unlock_set(
               getNodeId(), getChannelId(), static_cast<uint8_t>( magicCode.size() ), magicCode.data() ) == ZNET_ERR_OK;
}

bool ZCc0x7EAntiTheftUnlock::unlock( const std::string& hexMagicCode ) {
    auto magicCode = utils::hexToBytes( hexMagicCode );

    if( !magicCode ) {
        ZNET_LOGE( "Wrong hex magic code %s\n", hexMagicCode.c_str() );
        return false;
    }

    return unlock( *magicCode );
}

bool ZCc0x7EAntiTheftUnlock::requestValue( const RequestValue& request ) {
    return requestValueImpl( *this, AntiTheftUnlockStateRequest( getNodeId(), getChannelId() ), request );
}

bool ZCc0x7EAntiTheftUnlock::interview( InterviewCallback callback, WeakJobContext context ) {
    return singleInterviewRequestImpl( *this,
                                       AntiTheftUnlockStateRequest( getNodeId(), getChannelId() ),
                                       context,
                                       [this]( const znet_cmd_anti_theft_report_t& report ) {
                                           setInitialLockInfo( AntiTheftInfo::makeFromReport( report ) );
                                       },
                                       std::move( callback ) );
}

}  // namespace znet
