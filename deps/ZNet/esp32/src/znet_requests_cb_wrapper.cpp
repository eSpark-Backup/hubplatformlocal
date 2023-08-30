#include "znet/znet_requests_cb_wrapper.hpp"

namespace znet {

namespace RequestCbUtils {

void getErrorBegin( std::stringstream& stream,
                    RequestFailHandlingResult failHandlingResult,
                    const char* name,
                    znet_error_t err ) {
    const auto action = failHandlingResult == RequestFailHandlingResult::RequestContinued ?
                            "continued" :
                            failHandlingResult == RequestFailHandlingResult::RequestStopped ? "stopped" : "not found";
    stream << name << " fail (" << action << "). Error=" << err << ", args=[";
}

void getErrorEnd( std::stringstream& stream ) {
    stream << "]";
}

void getErrorArgs( std::stringstream& stream, const uint8_t& value ) {
    stream << static_cast<size_t>( value );
}

}  // namespace RequestCbUtils

}  // namespace znet
