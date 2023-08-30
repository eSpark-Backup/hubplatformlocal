#pragma once

namespace HubPlatform {
namespace LogCmd {

struct LogSettings {
    enum class RotationPolicy { Off, EachRotation, StoreLocally };
    RotationPolicy rotationPolicy = RotationPolicy::EachRotation;
    unsigned int rotationInterval = 12;
    unsigned int archivesTotalSizeLimit = 1 * 1024 * 1024;
    unsigned int fileSizeLimit = 2 * 1024 * 1024;
    unsigned int pendingLogsSizeLimit = 200 * 1024 * 1024;
};

void loadLogSettings( LogSettings& logSettings );

}  // namespace LogCmd
}  // namespace HubPlatform
