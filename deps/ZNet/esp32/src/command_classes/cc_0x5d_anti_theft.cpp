#include "znet/cc_0x5d_anti_theft.hpp"
#include "znet/znet.h"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <cassert>

namespace znet {

bool ZCc0x5DAntiTheft::lock( const std::vector<uint8_t>& magicCode,
                             const std::vector<uint8_t>& hint,
                             uint16_t manufacturerId,
                             uint16_t zwaveLockingId ) {
    if( magicCode.size() < MinMagicCodeLength || magicCode.size() > MaxMagicCodeLength ) {
        return false;
    }

    return znet_node_cmd_anti_theft_set( getNodeId(),
                                         getChannelId(),
                                         true,
                                         static_cast<uint8_t>( magicCode.size() ),
                                         magicCode.data(),
                                         manufacturerId,
                                         static_cast<uint8_t>( hint.size() ),
                                         hint.data(),
                                         zwaveLockingId ) == ZNET_ERR_OK;
}

bool ZCc0x5DAntiTheft::lock( const std::string& hexMagicCode,
                             const std::string& hexHint,
                             uint16_t manufacturerId,
                             uint16_t zwaveLockingId ) {
    auto magicCode = utils::hexToBytes( hexMagicCode );

    if( !magicCode ) {
        ZNET_LOGE( "Wrong hex magic code %s\n", hexMagicCode.c_str() );
        return false;
    }

    auto hint = utils::hexToBytes( hexHint );

    if( !hint ) {
        ZNET_LOGE( "Wrong hex hint code %s\n", hexHint.c_str() );
        return false;
    }

    return lock( *magicCode, *hint, manufacturerId, zwaveLockingId );
}

bool ZCc0x5DAntiTheft::unlock( const std::vector<uint8_t>& magicCode ) {
    return znet_node_cmd_anti_theft_set( getNodeId(),
                                         getChannelId(),
                                         false,
                                         static_cast<uint8_t>( magicCode.size() ),
                                         magicCode.data(),
                                         0,
                                         0,
                                         nullptr,
                                         0 ) == ZNET_ERR_OK;
}

bool ZCc0x5DAntiTheft::unlock( const std::string& hexMagicCode ) {
    auto magicCode = utils::hexToBytes( hexMagicCode );

    if( !magicCode ) {
        ZNET_LOGE( "Wrong hex magic code %s\n", hexMagicCode.c_str() );
        return false;
    }

    return unlock( *magicCode );
}

bool ZCc0x5DAntiTheft::requestValue( const RequestValue& request ) {
    return requestValueImpl( *this, AntiTheftRequest( getNodeId(), getChannelId() ), request );
}

bool ZCc0x5DAntiTheft::interview( InterviewCallback callback, WeakJobContext context ) {
    return singleInterviewRequestImpl( *this,
                                       AntiTheftRequest( getNodeId(), getChannelId() ),
                                       context,
                                       [this]( const znet_cmd_anti_theft_report_t& report ) {
                                           setInitialLockInfo( AntiTheftInfo::makeFromReport( report ) );
                                       },
                                       std::move( callback ) );
}

}  // namespace znet
