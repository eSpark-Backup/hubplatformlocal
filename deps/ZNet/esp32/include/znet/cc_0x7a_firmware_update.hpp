#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/enums/firmware_update.hpp>
#include <znet/iznet_timer_provider.hpp>
#include <znet/optional.hpp>

#include <memory>

namespace znet {

class ZCc0x7AFirmwareUpdate final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_FIRMWARE_UPDATE_MD>;
    using ZClass::ZClass;

    using FirmwareIds = std::vector<uint16_t>;

    using Source = FirmwareUpdate::Source;
    using Destination = FirmwareUpdate::Destination;
    using SourceUPtr = std::unique_ptr<Source>;
    using DestinationUPtr = std::unique_ptr<Destination>;

    using UpdatingResult = FirmwareUpdate::UpdatingResult;
    using DownloadingResult = FirmwareUpdate::DownloadingResult;

    using UpdatingResultCallback = FirmwareUpdate::UpdatingResultCallback;
    using DownloadingResultCallback = FirmwareUpdate::DownloadingResultCallback;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool update( uint8_t target, SourceUPtr source, UpdatingResultCallback resultCallback = nullptr );

    bool download( uint8_t target, DestinationUPtr destination, DownloadingResultCallback resultCallback = nullptr );

    void onDataRequested( uint8_t reportsCount, uint16_t reportNumber );

    void onDataRequestedUnexpectedly( uint16_t reportNumber );

    void onUpdateRequestStatusReceived( FirmwareUpdate::UpdateMdRequestReportStatus status );

    // TODO: implement via requests manager and move to private
    void onDataSendingStatusReceived( FirmwareUpdate::DataSendingStatus status, uint16_t waitingTime );

    // TODO: implement via requests manager and move to private
    void onActivationStatusReceived( uint8_t target,
                                     const znet_firmware_info_t& firmware,
                                     FirmwareUpdate::ActivationStatus status );

    uint16_t getManufacturerId() const;

    uint8_t getFirmwaresCount() const;
    uint16_t getFirmwareId( uint8_t idx = 0 ) const;

    const FirmwareIds& getFirmwareIds() const;

    uint16_t getFirmwareChecksum() const;

    bool isFirmwareUpgradable() const;

    uint16_t getPrefferedFragmentSize() const;

    utils::optional<uint16_t> getMaxFragmentSize() const;

    utils::optional<uint8_t> getHardwareRevision() const;

    bool storeData( IFile& file ) const override;

    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT();

private:
    enum class UpdatingState : uint8_t {
        Initial,
        WaitionForMdRequestReport,
        WaitingForDataRequest,
        WaitingForActivation
    };

    struct UpdatingInfo {
        SourceUPtr m_source;
        size_t m_sourcePos = 0;
        size_t m_sourceSize = 0;
        UpdatingResultCallback m_resultCallback;
        uint8_t m_target = 0;
        int8_t m_progress = -1;
        uint8_t m_progressInterval = 10;
        uint16_t m_firmwareId = 0;
        uint16_t m_checksum = 0;
        uint16_t m_fragmentSize = 0;
        UpdatingState m_state = UpdatingState::Initial;
        ITimerProvider::TimerUPtr m_timer;
    };

    struct DownloadingInfo {
        DestinationUPtr m_destination;
        uint8_t m_target = 0;
        uint16_t m_fragmentSize = 0;
        DownloadingResultCallback m_resultCallback;
        uint16_t m_actualChecksum = 0;
        uint16_t m_expectedChecksum = 0;
    };

    using DownloadingInfoPtr = std::shared_ptr<DownloadingInfo>;
    using DownloadingInfoWPtr = std::weak_ptr<DownloadingInfo>;

private:
    void onUpdatingResult( UpdatingResult result );

    static void onDownloadingResult( DownloadingInfoPtr downloadingInfo, DownloadingResult result );

    void requestImageFragment( DownloadingInfoPtr downloadingInfo, uint16_t fragmentIndex = 0 );

    bool isDownloadingInProgress() const;

private:
    uint16_t m_manufacturerId = 0;
    uint16_t m_checksum = 0;

    bool m_isFirmwareUpgradable = false;
    FirmwareIds m_targetFirmwareIds;
    utils::optional<uint16_t> m_maxFragmentSize;
    utils::optional<uint8_t> m_hardwareRevision;

    std::unique_ptr<UpdatingInfo> m_updatingInfo;
    DownloadingInfoWPtr m_downloadingInfo;
};

}  // namespace znet
