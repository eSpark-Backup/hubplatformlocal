#pragma once

#include <stdexcept>
#include <string>

namespace HubPlatform {
namespace CloudDevices {

class SubscriptionError : public std::runtime_error {
public:
    explicit SubscriptionError( const std::string& msg )
    : runtime_error{ msg } {}
};

}  // namespace CloudDevices
}  // namespace HubPlatform
