#pragma once

#include "http_bridge/endpoint.h"
#include "tiny/tiny_utils.h"
#include <map>

namespace HubPlatform {
namespace Uid {
namespace Extensions {

class HttpSession {
public:
    struct UserData {
        virtual ~UserData() = default;
    };

    virtual ~HttpSession() = default;

    std::string addon;

    std::string url;
    std::string method;
    std::map< std::string, std::string > headers;
    std::unique_ptr< UserData > userData;
    bool responseHeadersWritten = false;

    bool hasHeader( const std::string& header ) const {
        return headers.count( tiny::tools::to_lower( header ) ) != 0;
    }

    bool hasHeader( const std::string& header, const std::string& value ) const {
        auto it = headers.find( tiny::tools::to_lower( header ) );
        if( it == headers.end() )
            return false;

        return tiny::tools::to_lower( it->second ) == tiny::tools::to_lower( value );
    }
};

}  // namespace Extensions
}  // namespace Uid
}  // namespace HubPlatform
