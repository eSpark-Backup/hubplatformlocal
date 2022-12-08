#include <znet/converters/mode_converter.hpp>
#include <znet/converters/resources.hpp>

namespace znet {
namespace mode_converter {

using namespace resources;

const char* toString( znet::IZNet::Mode mode ) noexcept {
    switch( mode ) {
    case znet::IZNet::Mode::Initializing:
        return modes::Initializing;
    case znet::IZNet::Mode::IncludeStarting:
        return modes::IncludeStarting;
    case znet::IZNet::Mode::IncludeStopping:
        return modes::IncludeStopping;
    case znet::IZNet::Mode::ExcludeStarting:
        return modes::ExcludeStarting;
    case znet::IZNet::Mode::ExcludeStopping:
        return modes::ExcludeStopping;
    case znet::IZNet::Mode::LearnStarting:
        return modes::LearnStarting;
    case znet::IZNet::Mode::LearnStopping:
        return modes::LearnStopping;
    case znet::IZNet::Mode::Monitoring:
        return modes::Monitoring;
    case znet::IZNet::Mode::NifSending:
        return modes::NifSending;
    case znet::IZNet::Mode::NodeAdding:
        return modes::NodeAdding;
    case znet::IZNet::Mode::NodeRemoving:
        return modes::NodeRemoving;
    case znet::IZNet::Mode::NodeLearning:
        return modes::NodeLearning;
    case znet::IZNet::Mode::NodeListGetting:
        return modes::NodeListGetting;
    case znet::IZNet::Mode::ValuesGetting:
        return modes::ValuesGetting;
    case znet::IZNet::Mode::Reseting:
        return modes::Reseting;
    case znet::IZNet::Mode::Idle:
        return modes::Idle;
    case znet::IZNet::Mode::TimeSynchronization:
        return modes::TimeSynchronization;
    case znet::IZNet::Mode::Rediscover:
        return modes::Rediscover;
    case znet::IZNet::Mode::FirmwareUpdate:
        return modes::FirmwareUpdate;
    case znet::IZNet::Mode::NodeChecking:
        return modes::NodeChecking;
    default:
        return modes::Unknown;
    }
}

}  // namespace mode_converter
}  // namespace znet
