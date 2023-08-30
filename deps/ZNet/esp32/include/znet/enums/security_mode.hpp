#pragma once

#include <znet/znet.h>
#include <cstdint>

namespace znet {
namespace SecurityMode {

enum class Class { None, S2_Authenticated, S2_Unauthenticated, S2_AccessControl, S0, Unknown };

inline Class fromByte( uint8_t value ) {
    switch( value & ZNET_NODEINFO_SECURITY_MASK ) {
    case ZNET_NODEINFO_SECURITY_NONE:
        return Class::None;

    case ZNET_NODEINFO_SECURITY_S2_AUTH:
        return Class::S2_Authenticated;

    case ZNET_NODEINFO_SECURITY_S2_UNAUTH:
        return Class::S2_Unauthenticated;

    case ZNET_NODEINFO_SECURITY_S2_ACCESS:
        return Class::S2_AccessControl;

    case ZNET_NODEINFO_SECURITY_S0:
        return Class::S0;

    default:
        return Class::Unknown;
    }
}

inline uint8_t toByte( Class value ) {
    switch( value ) {
    case Class::None:
        return ZNET_NODEINFO_SECURITY_NONE;

    case Class::S2_Authenticated:
        return ZNET_NODEINFO_SECURITY_S2_AUTH;

    case Class::S2_Unauthenticated:
        return ZNET_NODEINFO_SECURITY_S2_UNAUTH;

    case Class::S2_AccessControl:
        return ZNET_NODEINFO_SECURITY_S2_ACCESS;

    case Class::S0:
        return ZNET_NODEINFO_SECURITY_S0;

    default:
        return ZNET_NODEINFO_SECURITY_UNKNOWN;
    }
}

inline Class getHighest( uint8_t mask ) {
    if( mask & ZNET_SECURITY2_S2_CLASS_2 ) {
        return SecurityMode::Class::S2_AccessControl;
    }
    else if( mask & ZNET_SECURITY2_S2_CLASS_1 ) {
        return SecurityMode::Class::S2_Authenticated;
    }
    else if( mask & ZNET_SECURITY2_S2_CLASS_0 ) {
        return SecurityMode::Class::S2_Unauthenticated;
    }
    else if( mask & ZNET_SECURITY2_S0_NET_KEY ) {
        return SecurityMode::Class::S0;
    }
    else {
        return SecurityMode::Class::None;
    }
}

}  // namespace SecurityMode
}  // namespace znet
