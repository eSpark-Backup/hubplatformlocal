#pragma once

#include <znet/enums/znet_firmware_update.hpp>
#include <znet/znet_io_wrapper.hpp>
#include <functional>

namespace znet {
namespace FirmwareUpdate {

using Source = IOWrapper::Source;
using Destination = IOWrapper::Destination;

using SourceUPtr = IOWrapper::SourceUPtr;
using DestinationUPtr = IOWrapper::DestinationUPtr;

using UpdatingResultCallback = std::function<void( UpdatingResult, uint8_t )>;
using DownloadingResultCallback = std::function<void( DownloadingResult )>;

}  // namespace FirmwareUpdate
}  // namespace znet
