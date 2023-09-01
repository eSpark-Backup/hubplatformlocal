#pragma once

#include <cstdint>

namespace znet {
namespace FirmwareUpdate {

enum class UpdateMdRequestReportStatus : uint8_t {
    InvalidIdsCombination = 0x00,
    AuthenticationError = 0x01,
    InvalidMaxFragmentSize = 0x02,
    FirmwareNotUpgradable = 0x03,
    InvalidHardwareVersion = 0x04,
    NoError = 0xFF
};

enum class DataSendingStatus : uint8_t {
    ChecksumError = 0x00,
    ReceivingError = 0x01,
    ManufacturerIdMatchingError = 0x02,
    FirmwareIdMatchingError = 0x03,
    FirmwareTargetMatchingError = 0x04,
    InvalidHeaderInfo = 0x04,
    InvalidHeaderformat = 0x05,
    InsufficientMemory = 0x07,
    HardwareVersionMatchingError = 0x08,

    WaitingForActivation = 0xFD,
    ImageStoredWithoutDeviceRestarting = 0xFE,
    ImageStoredWithDeviceRestarting = 0xFF
};

enum class ActivationStatus : uint8_t { InvalidIdsCombination = 0x00, ActivatingError = 0x01, NoError = 0xFF };

enum class PreparationStatus : uint8_t {
    InvalidIdsCombination = 0x00,
    FwUpdateAuthentificationExpectedError = 0x01,
    FragmetSizeExceedsMaximum = 0x02,
    FirmwareNotDownloadable = 0x03,
    InvalidHardwareVersion = 0x04,

    NoError = 0xFF
};

}  // namespace FirmwareUpdate
}  // namespace znet
