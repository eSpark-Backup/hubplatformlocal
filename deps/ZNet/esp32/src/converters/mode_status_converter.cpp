#include <znet/converters/mode_status_converter.hpp>
#include <znet/converters/resources.hpp>

#include "znet/znet_command.hpp"

namespace znet {
namespace mode_status_converter {

using namespace resources;

const char* toString( const znet::ModeStatus modeStatus ) noexcept {
    switch( modeStatus ) {
    case znet::ModeStatus::Invoked:
        return mode_statuses::Invoked;
    case znet::ModeStatus::Started:
        return mode_statuses::Started;
    case znet::ModeStatus::Finished:
        return mode_statuses::Finished;
    case znet::ModeStatus::InProgress:
        return mode_statuses::InProgress;
    case znet::ModeStatus::Failed:
        return mode_statuses::Failed;
    case znet::ModeStatus::Timeout:
        return mode_statuses::Timeout;
    case znet::ModeStatus::Busy:
        return mode_statuses::Busy;
    case znet::ModeStatus::Empty:
        return mode_statuses::Empty;
    case znet::ModeStatus::Undefined:
        return mode_statuses::Undefined;
    };

    return mode_statuses::Unknown;
}

}  // namespace mode_status_converter
}  // namespace znet
