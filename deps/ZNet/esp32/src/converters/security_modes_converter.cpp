#include <znet/converters/security_modes_converter.hpp>

#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>
#include <znet/znet.h>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace security_modes_converter {

const char* typeToString( SecurityMode::Class mode ) noexcept {
    switch( mode ) {
    case SecurityMode::Class::S2_Unauthenticated:
        return resources::security_modes::S2Unauthenticated;
    case SecurityMode::Class::S2_Authenticated:
        return resources::security_modes::S2Authenticated;
    case SecurityMode::Class::S2_AccessControl:
        return resources::security_modes::S2;
    case SecurityMode::Class::S0:
        return resources::security_modes::S0;
    default:
        return resources::security_modes::none;
    }
}

uint8_t typeToBitset( SecurityMode::Class mode ) noexcept {
    switch( mode ) {
    case SecurityMode::Class::S2_Unauthenticated:
        return ZNET_SECURITY2_S2_CLASS_0;
    case SecurityMode::Class::S2_Authenticated:
        return ZNET_SECURITY2_S2_CLASS_1;
    case SecurityMode::Class::S2_AccessControl:
        return ZNET_SECURITY2_S2_CLASS_2;
    case SecurityMode::Class::S0:
        return ZNET_SECURITY2_S0_NET_KEY;
    default:
        return 0;
    }
}

uint8_t listToBitset( const std::vector<const char*>& list ) noexcept {
    uint8_t result = 0;

    for( auto& str : list ) {
        if( strcmp( str, resources::security_modes::S2Unauthenticated ) == 0 ) {
            result |= ZNET_SECURITY2_S2_CLASS_0;
        }
        else if( strcmp( str, resources::security_modes::S2Authenticated ) == 0 ) {
            result |= ZNET_SECURITY2_S2_CLASS_1;
        }
        else if( strcmp( str, resources::security_modes::S2 ) == 0 ) {
            result |= ZNET_SECURITY2_S2_CLASS_2;
        }
        else if( strcmp( str, resources::security_modes::CSA ) == 0 ) {
            result |= ZNET_SECURITY2_S2_CSA;
        }
        else if( strcmp( str, resources::security_modes::S0 ) == 0 ) {
            result |= ZNET_SECURITY2_S0_NET_KEY;
        }
    }

    return result;
}

std::vector<const char*> bitsetToList( uint8_t mode ) {
    std::vector<const char*> authModeList;

    if( mode & ZNET_SECURITY2_S2_CLASS_0 ) {
        authModeList.push_back( resources::security_modes::S2Unauthenticated );
    }
    if( mode & ZNET_SECURITY2_S2_CLASS_1 ) {
        authModeList.push_back( resources::security_modes::S2Authenticated );
    }
    if( mode & ZNET_SECURITY2_S2_CLASS_2 ) {
        authModeList.push_back( resources::security_modes::S2 );
    }
    if( mode & ZNET_SECURITY2_S0_NET_KEY ) {
        authModeList.push_back( resources::security_modes::S0 );
    }
    if( mode & ZNET_SECURITY2_S2_CSA ) {
        authModeList.push_back( resources::security_modes::CSA );
    }

    return authModeList;
}

uint8_t strToType( const char* str ) noexcept {
    if( strcmp( str, resources::security_modes::S2Unauthenticated ) == 0 ) {
        return ZNET_SECURITY2_S2_CLASS_0;
    }
    else if( strcmp( str, resources::security_modes::S2Authenticated ) == 0 ) {
        return ZNET_SECURITY2_S2_CLASS_1;
    }
    else if( strcmp( str, resources::security_modes::S2 ) == 0 ) {
        return ZNET_SECURITY2_S2_CLASS_2;
    }
    else if( strcmp( str, resources::security_modes::CSA ) == 0 ) {
        return ZNET_SECURITY2_S2_CSA;
    }
    else if( strcmp( str, resources::security_modes::S0 ) == 0 ) {
        return ZNET_SECURITY2_S0_NET_KEY;
    }
    return ZNET_NODEINFO_SECURITY_UNKNOWN;
}

}  // namespace security_modes_converter
}  // namespace znet
