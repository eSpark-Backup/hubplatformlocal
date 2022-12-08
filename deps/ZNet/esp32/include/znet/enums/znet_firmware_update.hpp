#include <cstdint>

namespace znet {
namespace FirmwareUpdate {

enum class UpdatingResult : uint8_t {
    InProgress,
    ImageUploaded,
    ImageUploadedWithoutDeviceRestarting,
    ImageUploadedWithDeviceRestarting,
    Error  // TODO: specify errors
};

enum class DownloadingResult : uint8_t {
    ImageDownloaded,
    Error  // TODO: specify errors
};

}  // namespace FirmwareUpdate
}  // namespace znet
