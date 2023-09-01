#include "znet/cc_0x7a_firmware_update.hpp"
#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"
#include "znet/znet_utils.hpp"

#include <cassert>

extern "C" {
#include "znet_log.h"
#include "byte_ops.h"
}

namespace znet {

namespace {

enum Flags {
    IsFirmwareUpgradable = 1,
    HasFragmentSize = 1 << 1,
    HasHardwareRevision = 1 << 2,
};

constexpr uint8_t ZwaveFrameSize = 39;
constexpr uint8_t FirmwareUpdateDataFrameHeaderSize = 4;
constexpr uint8_t FirmwareUpdateDataChecksumSize = 2;
constexpr std::chrono::seconds Timeout( 120 );
constexpr size_t DesiredProgressDataSendingSize = 6000;
constexpr size_t MinProgressInterval = 1;
constexpr size_t MaxProgressInterval = 25;
}  // namespace

bool ZCc0x7AFirmwareUpdate::interview( InterviewCallback callback, WeakJobContext context ) {
    m_targetFirmwareIds.clear();

    m_manufacturerId = 0;
    m_checksum = 0;

    m_isFirmwareUpgradable = false;
    m_maxFragmentSize.reset();
    m_hardwareRevision.reset();

    return singleInterviewRequestImpl(
        *this,
        FirmwareMdRequest( getNodeId(), getChannelId() ),
        context,
        [this]( const znet_firmware_md_report_v1_t& reportV1,
                const znet_firmware_md_report_v3_t* reportV3Opt,
                const znet_firmware_md_report_v5_t* reportV5Opt ) {
            m_manufacturerId = reportV1.manufacturer_id;
            m_checksum = reportV1.checksum;

            m_targetFirmwareIds.push_back( reportV1.firmware_id );

            if( reportV3Opt ) {
                m_isFirmwareUpgradable = reportV3Opt->firmware_upgradable;
                m_maxFragmentSize = reportV3Opt->max_fragment_size;
                m_targetFirmwareIds.insert( m_targetFirmwareIds.end(),
                                            reportV3Opt->additional_firmware,
                                            reportV3Opt->additional_firmware + reportV3Opt->additional_firmware_count );
            }
            else {
                m_isFirmwareUpgradable = true;
            }

            if( reportV5Opt ) {
                m_hardwareRevision = reportV5Opt->hardware_revision;
            }
        },
        std::move( callback ) );
}

bool ZCc0x7AFirmwareUpdate::update( uint8_t target, SourceUPtr source, UpdatingResultCallback resultCallback ) {
    assert( source );

    if( m_targetFirmwareIds.size() <= target ) {
        ZNET_LOGW( "Invalid target %u\n", target );
        return false;
    }

    if( !m_isFirmwareUpgradable ) {
        ZNET_LOGW( "Attempt to update not upgradable firmware\n" );
        return false;
    }

    if( isDownloadingInProgress() ) {
        ZNET_LOGW( "Downloading is in progress, cannot start update\n" );
        return false;
    }

    if( m_updatingInfo ) {
        // TODO: investigate what to do with this case

        ZNET_LOGW( "Update is in progress, restarting session\n" );
        onUpdatingResult( UpdatingResult::Error );
    }

    m_updatingInfo.reset( new UpdatingInfo );

    m_updatingInfo->m_timer = getParentNode().getRequestsManager().getTimerProvider().createTimer( [this]() {
        ZNET_LOGE( "Firmware update timeout!\n" );
        onUpdatingResult( UpdatingResult::Error );
    } );

    m_updatingInfo->m_target = target;
    m_updatingInfo->m_firmwareId = m_targetFirmwareIds[target];
    m_updatingInfo->m_source = std::move( source );
    m_updatingInfo->m_state = UpdatingState::WaitionForMdRequestReport;
    m_updatingInfo->m_resultCallback = std::move( resultCallback );
    m_updatingInfo->m_fragmentSize = getPrefferedFragmentSize();
    if( m_maxFragmentSize && m_updatingInfo->m_fragmentSize > *m_maxFragmentSize ) {
        m_updatingInfo->m_fragmentSize = *m_maxFragmentSize;
    }

    constexpr size_t bufferSize = 256;

    m_updatingInfo->m_source->seek( 0 );

    m_updatingInfo->m_checksum = CRC16_DEFAULT_INIT_VALUE;
    m_updatingInfo->m_sourceSize = 0;
    uint8_t buffer[bufferSize];
    for( size_t pos = 0;; pos += bufferSize ) {
        const size_t readBytes = m_updatingInfo->m_source->read( bufferSize, buffer );
        assert( readBytes <= bufferSize );

        m_updatingInfo->m_sourceSize += readBytes;
        m_updatingInfo->m_checksum = crc16_with_init_value( buffer, readBytes, m_updatingInfo->m_checksum );

        if( readBytes != bufferSize )
            break;
    }

    if( m_updatingInfo->m_sourceSize == 0 ) {
        ZNET_LOGE( "firmware updae source is empty!\n" );
        onUpdatingResult( UpdatingResult::Error );
        return false;
    }

    m_updatingInfo->m_progressInterval = static_cast<uint8_t>(
        std::min( std::max( DesiredProgressDataSendingSize * 100 / m_updatingInfo->m_sourceSize, MinProgressInterval ),
                  MaxProgressInterval ) );

    m_updatingInfo->m_source->seek( 0 );
    m_updatingInfo->m_sourcePos = 0;

    znet_firmware_update_md_request_get_params_v1 params_v1;
    params_v1.checksum = m_updatingInfo->m_checksum;
    params_v1.firmware_id = m_updatingInfo->m_firmwareId;
    params_v1.manufacturer_id = m_manufacturerId;

    znet_firmware_update_md_request_get_params_v3 params_v3;
    params_v3.fragment_size = m_updatingInfo->m_fragmentSize;
    params_v3.firmware_target = target;

    znet_firmware_update_md_request_get_params_v4 params_v4;
    params_v4.is_postponed_activation_allowed = false;

    znet_firmware_update_md_request_get_params_v5 params_v5;
    params_v5.hardware_revision = m_hardwareRevision ? *m_hardwareRevision : 0;

    ZNET_LOGI( "Start device update session:\n"
               "node: %u\n"
               "firmware id: %u\n"
               "target: %u\n"
               "checksum: 0x%x\n"
               "fragment size: %u\n"
               "source size: %u\n"
               "expected fragments count: %u\n",
               getNodeId(),
               m_updatingInfo->m_firmwareId,
               m_updatingInfo->m_target,
               m_updatingInfo->m_checksum,
               m_updatingInfo->m_fragmentSize,
               m_updatingInfo->m_sourceSize,
               ( m_updatingInfo->m_sourceSize + m_updatingInfo->m_fragmentSize - 1 ) / m_updatingInfo->m_fragmentSize );

    if( znet_node_cmd_firmware_update_md_request_get( getNodeId(),
                                                      getChannelId(),
                                                      &params_v1,
                                                      getVersion() >= 3 ? &params_v3 : nullptr,
                                                      getVersion() >= 4 ? &params_v4 : nullptr,
                                                      getVersion() >= 5 ? &params_v5 : nullptr ) != ZNET_ERR_OK ) {
        ZNET_LOGE( "Cannot send znet_node_cmd_firmware_update_md_request\n" );
        onUpdatingResult( UpdatingResult::Error );
        return false;
    }

    onUpdatingResult( UpdatingResult::InProgress );

    return true;
}

bool ZCc0x7AFirmwareUpdate::download( uint8_t target,
                                      DestinationUPtr destination,
                                      DownloadingResultCallback resultCallback ) {
    if( getVersion() < 5 ) {
        ZNET_LOGW( "Downloading is supported from at least 5th version. Actual version %u\n", getVersion() );
        return false;
    }

    if( m_updatingInfo ) {
        ZNET_LOGW( "Updating is in progress, cannot start download\n" );
        return false;
    }

    const auto fragmentSize = getPrefferedFragmentSize();

    const FirmwareUpdateMdPrepareRequest request( getNodeId(),
                                                  getChannelId(),
                                                  m_manufacturerId,
                                                  m_targetFirmwareIds[target],
                                                  target,
                                                  fragmentSize,
                                                  m_hardwareRevision ? *m_hardwareRevision : 0 );

    if( getParentNode().getRequestsManager().isWaitingForRespond( request ) ) {
        ZNET_LOGW( "FirmwareUpdateMdPrepareRequest is in progress, cannot start new session\n" );
        return false;
    }

    if( isDownloadingInProgress() ) {
        ZNET_LOGW( "Downloading is in progress, cannot start new session\n" );
        return false;
    }

    auto downloadingInfo = std::make_shared<DownloadingInfo>();
    m_downloadingInfo = downloadingInfo;

    downloadingInfo->m_destination = std::move( destination );
    downloadingInfo->m_resultCallback = std::move( resultCallback );
    downloadingInfo->m_target = target;
    downloadingInfo->m_fragmentSize = fragmentSize;
    downloadingInfo->m_actualChecksum = CRC16_DEFAULT_INIT_VALUE;

    getParentNode().getRequestsManager().doRequest(
        request,
        {},
        [this, downloadingInfo]( uint8_t status, uint16_t checksum ) {
            switch( static_cast<FirmwareUpdate::PreparationStatus>( status ) ) {
            case FirmwareUpdate::PreparationStatus::NoError:
                ZNET_LOGI( "Activation report ok\n" );
                requestImageFragment( downloadingInfo );
                break;

            default:
                ZNET_LOGW( "Cannot download firmware: preparation status %u\n", status );
                onDownloadingResult( downloadingInfo, DownloadingResult::Error );
                break;
            }

            downloadingInfo->m_expectedChecksum = checksum;
        },
        [downloadingInfo]( RequestFailType ) {
            ZNET_LOGW( "Cannot download firmware: preparation failed\n" );
            onDownloadingResult( downloadingInfo, DownloadingResult::Error );
        } );

    return true;
}

void ZCc0x7AFirmwareUpdate::onDataRequested( uint8_t reportsCount, uint16_t reportNumber ) {
    if( reportNumber == 0 ) {
        ZNET_LOGW( "Invalid report number 0\n" );
        return;
    }

    if( !m_updatingInfo ) {
        ZNET_LOGW( "Cannot send firmware update data: updating is not activated\n" );
        onDataRequestedUnexpectedly( reportNumber );
        return;
    }

    if( m_updatingInfo->m_state != UpdatingState::WaitingForDataRequest ) {
        ZNET_LOGW( "Cannot send firmware update data: invalid state, %u\n", m_updatingInfo->m_state );

        // onDataRequestedUnexpectedly( reportNumber ); // TODO: investigate, should we send it?
        return;
    }

    const bool useChecksum = getVersion() >= 2;

    const size_t dataSize = m_updatingInfo->m_fragmentSize;
    uint8_t buffer[dataSize];
    bool isLastFragment = false;

    onUpdatingResult( UpdatingResult::InProgress );

    // TODO: send by timer instead of cycle.
    for( uint16_t i = 0; i < reportsCount && !isLastFragment; ++i ) {
        const size_t sourcePos = std::min( dataSize * ( reportNumber - 1 ), m_updatingInfo->m_sourceSize );
        if( m_updatingInfo->m_sourcePos != sourcePos ) {
            m_updatingInfo->m_source->seek( sourcePos );
            m_updatingInfo->m_sourcePos = sourcePos;
        }

        const auto readBytes =
            m_updatingInfo->m_source->read( std::min( dataSize, m_updatingInfo->m_sourceSize - sourcePos ), buffer );
        m_updatingInfo->m_sourcePos += readBytes;

        assert( readBytes <= dataSize );
        assert( m_updatingInfo->m_sourcePos <= m_updatingInfo->m_sourceSize );

        isLastFragment = m_updatingInfo->m_sourcePos >= m_updatingInfo->m_sourceSize;

        const auto reportResult = znet_node_cmd_firmware_update_md_report( getNodeId(),
                                                                           getChannelId(),
                                                                           reportNumber + i,
                                                                           isLastFragment,
                                                                           static_cast<uint8_t>( readBytes ),
                                                                           buffer,
                                                                           useChecksum );

        if( reportResult != ZNET_ERR_OK ) {
            ZNET_LOGE( "Error sending znet_node_cmd_firmware_update_md_report %i\n", reportResult );
        }
    }
}

void ZCc0x7AFirmwareUpdate::onDataRequestedUnexpectedly( uint16_t reportNumber ) {
    znet_node_cmd_firmware_update_md_report(
        getNodeId(), getChannelId(), reportNumber, true, 0, nullptr, getVersion() >= 2 );
}

void ZCc0x7AFirmwareUpdate::onDataSendingStatusReceived( FirmwareUpdate::DataSendingStatus status,
                                                         uint16_t waitingTime ) {
    if( !m_updatingInfo || m_updatingInfo->m_state != UpdatingState::WaitingForDataRequest ) {
        ZNET_LOGW( "Unexpected onDataSendingStatusReceived, status %u\n", status );
        return;
    }

    switch( status ) {
    case FirmwareUpdate::DataSendingStatus::WaitingForActivation: {
        m_updatingInfo->m_state = UpdatingState::WaitingForActivation;

        znet_firmware_update_activation_set_params_v4 param_v4;
        param_v4.firmware.firmware_id = m_updatingInfo->m_firmwareId;
        param_v4.firmware.manufacturer_id = m_manufacturerId;
        param_v4.firmware.checksum = m_updatingInfo->m_checksum;
        param_v4.firmware_target = m_updatingInfo->m_target;

        znet_firmware_update_activation_set_params_v5 param_v5;
        param_v5.hardware_version = m_hardwareRevision ? *m_hardwareRevision : 0;
        if( znet_node_cmd_firmware_update_activation_set(
                getNodeId(), getChannelId(), &param_v4, getVersion() >= 5 ? &param_v5 : nullptr ) != ZNET_ERR_OK ) {
            ZNET_LOGE( "Error sending znet_node_cmd_firmware_update_activation_set\n" );
            onUpdatingResult( UpdatingResult::Error );
        }
    } break;
    case FirmwareUpdate::DataSendingStatus::ImageStoredWithoutDeviceRestarting:
        // TODO: Handle waitingTime
        onUpdatingResult( UpdatingResult::ImageUploadedWithoutDeviceRestarting );
        break;
    case FirmwareUpdate::DataSendingStatus::ImageStoredWithDeviceRestarting:
        // TODO: Handle waitingTime
        onUpdatingResult( UpdatingResult::ImageUploadedWithDeviceRestarting );
        break;
    default:
        ZNET_LOGW( "Data sending error status received, %u\n", status );
        onUpdatingResult( UpdatingResult::Error );
        break;
    }
}

void ZCc0x7AFirmwareUpdate::onUpdateRequestStatusReceived( FirmwareUpdate::UpdateMdRequestReportStatus status ) {
    if( !m_updatingInfo || m_updatingInfo->m_state != UpdatingState::WaitionForMdRequestReport ) {
        ZNET_LOGW( "Unexpected onUpdateRequestStatusReceived, status %u\n", status );
        return;
    }

    switch( status ) {
    case FirmwareUpdate::UpdateMdRequestReportStatus::NoError:
        m_updatingInfo->m_state = UpdatingState::WaitingForDataRequest;
        break;
    default:
        ZNET_LOGW( "Update request error %u\n", status );
        onUpdatingResult( UpdatingResult::Error );
        break;
    }
}
void ZCc0x7AFirmwareUpdate::onActivationStatusReceived( uint8_t target,
                                                        const znet_firmware_info_t& firmware,
                                                        FirmwareUpdate::ActivationStatus status ) {
    if( !m_updatingInfo || m_updatingInfo->m_state != UpdatingState::WaitingForActivation ) {
        ZNET_LOGW( "Unexpected onActivationStatusReceived, status %u\n", status );
        return;
    }

    if( target != m_updatingInfo->m_target ) {
        ZNET_LOGW( "Invalid target %u\n", target );
        // return ?
    }

    if( firmware.manufacturer_id != m_manufacturerId ) {
        ZNET_LOGW( "Invalid manufacturer id %u\n", firmware.manufacturer_id );
        // return ?
    }

    if( firmware.firmware_id != m_updatingInfo->m_firmwareId ) {
        ZNET_LOGW(
            "Invalid firmware_id %u, expected firmware_id %u\n", firmware.firmware_id, m_updatingInfo->m_firmwareId );
        // return ?
    }

    if( firmware.checksum != m_updatingInfo->m_checksum ) {
        ZNET_LOGW( "Invalid checkum %u, expected checkum %u\n", firmware.checksum, m_updatingInfo->m_checksum );
    }

    switch( status ) {
    case FirmwareUpdate::ActivationStatus::NoError:
        onUpdatingResult( UpdatingResult::ImageUploaded );
        break;
    default:
        ZNET_LOGW( "Activation error %u\n", status );
        onUpdatingResult( UpdatingResult::Error );
        break;
    }
}

uint16_t ZCc0x7AFirmwareUpdate::getManufacturerId() const {
    return m_manufacturerId;
}

uint8_t ZCc0x7AFirmwareUpdate::getFirmwaresCount() const {
    return static_cast<uint8_t>( m_targetFirmwareIds.size() );
}

uint16_t ZCc0x7AFirmwareUpdate::getFirmwareId( uint8_t idx ) const {
    return m_targetFirmwareIds.empty() ? 0 : m_targetFirmwareIds.at( idx );
}

const ZCc0x7AFirmwareUpdate::FirmwareIds& ZCc0x7AFirmwareUpdate::getFirmwareIds() const {
    return m_targetFirmwareIds;
}

uint16_t ZCc0x7AFirmwareUpdate::getFirmwareChecksum() const {
    return m_checksum;
}

bool ZCc0x7AFirmwareUpdate::isFirmwareUpgradable() const {
    return m_isFirmwareUpgradable;
}

uint16_t ZCc0x7AFirmwareUpdate::getPrefferedFragmentSize() const {
    return ZwaveFrameSize - FirmwareUpdateDataFrameHeaderSize -
           ( getVersion() >= 2 ? FirmwareUpdateDataChecksumSize : 0 );
}

utils::optional<uint16_t> ZCc0x7AFirmwareUpdate::getMaxFragmentSize() const {
    return m_maxFragmentSize;
}

utils::optional<uint8_t> ZCc0x7AFirmwareUpdate::getHardwareRevision() const {
    return m_hardwareRevision;
}

bool ZCc0x7AFirmwareUpdate::storeData( IFile& file ) const {
    bool result = true;

    const uint8_t flags = ( m_isFirmwareUpgradable ? Flags::IsFirmwareUpgradable : 0 ) |
                          ( m_maxFragmentSize ? Flags::HasFragmentSize : 0 ) |
                          ( m_hardwareRevision ? Flags::HasHardwareRevision : 0 );

    result = result && file.write( &flags, sizeof( flags ) );
    result = result && utils::writeVectorToFile( file, m_targetFirmwareIds );
    result = result && file.write( &m_manufacturerId, sizeof( m_manufacturerId ) );
    result = result && file.write( &m_checksum, sizeof( m_checksum ) );

    if( m_maxFragmentSize ) {
        result = result && file.write( m_maxFragmentSize.get(), sizeof( *m_maxFragmentSize ) );
    }

    if( m_hardwareRevision ) {
        result = result && file.write( m_hardwareRevision.get(), sizeof( *m_hardwareRevision ) );
    }

    return result;
}

bool ZCc0x7AFirmwareUpdate::restoreData( IFile& file, int version ) {
    bool result = true;

    uint8_t flags = 0;
    result = result && file.read( &flags, sizeof( flags ) );

    m_isFirmwareUpgradable = ( flags & Flags::IsFirmwareUpgradable ) != 0;
    result = result && utils::readVectorFromFile( file, m_targetFirmwareIds );
    result = result && file.read( &m_manufacturerId, sizeof( m_manufacturerId ) );
    result = result && file.read( &m_checksum, sizeof( m_checksum ) );

    if( flags & Flags::HasFragmentSize ) {
        m_maxFragmentSize = 0;
        result = result && file.read( m_maxFragmentSize.get(), sizeof( *m_maxFragmentSize ) );
    }

    if( flags & Flags::HasHardwareRevision ) {
        m_hardwareRevision = 0;
        result = result && file.read( m_hardwareRevision.get(), sizeof( *m_hardwareRevision ) );
    }

    return result;
}

void ZCc0x7AFirmwareUpdate::onUpdatingResult( UpdatingResult result ) {
    assert( m_updatingInfo );

    const auto inProgress = result == UpdatingResult::InProgress;

    if( m_updatingInfo->m_resultCallback ) {
        const auto progress = static_cast<int8_t>(
            m_updatingInfo->m_sourceSize ?
                std::min( m_updatingInfo->m_progressInterval *
                              ( m_updatingInfo->m_sourcePos * ( 100 / m_updatingInfo->m_progressInterval ) /
                                m_updatingInfo->m_sourceSize ),
                          size_t( inProgress ? 95 : 100 ) ) :
                0 );

        if( !inProgress || m_updatingInfo->m_progress != progress ) {
            m_updatingInfo->m_progress = progress;
            m_updatingInfo->m_resultCallback( result, m_updatingInfo->m_progress );
        }
    }

    if( inProgress ) {
        m_updatingInfo->m_timer->runOnce( Timeout );
    }
    else {
        m_updatingInfo.reset();
    }
}

void ZCc0x7AFirmwareUpdate::onDownloadingResult( DownloadingInfoPtr downloadingInfo, DownloadingResult result ) {
    assert( downloadingInfo );

    if( downloadingInfo->m_resultCallback ) {
        downloadingInfo->m_resultCallback( result );
    }
}

void ZCc0x7AFirmwareUpdate::requestImageFragment( DownloadingInfoPtr downloadingInfo, uint16_t fragmentIndex ) {
    getParentNode().getRequestsManager().doRequest(
        FirmwareUpdateMdRequest( getNodeId(), getChannelId(), 1, downloadingInfo->m_fragmentSize ),
        {},
        [downloadingInfo, fragmentIndex, this]( bool isLast, uint8_t dataSize, const uint8_t* data ) {
            dataSize -= ( getVersion() >= 2 ? FirmwareUpdateDataChecksumSize : 0 );

            downloadingInfo->m_actualChecksum =
                crc16_with_init_value( data, dataSize, downloadingInfo->m_actualChecksum );
            downloadingInfo->m_destination->write(
                dataSize - ( getVersion() >= 2 ? FirmwareUpdateDataChecksumSize : 0 ), data );

            if( isLast ) {
                if( downloadingInfo->m_actualChecksum == downloadingInfo->m_expectedChecksum ) {
                    ZNET_LOGI( "Image downloaded, checksum is ok" );
                    onDownloadingResult( downloadingInfo, DownloadingResult::ImageDownloaded );
                }
                else {
                    ZNET_LOGW( "Fail to download image: actual checksum: 0x%x, expected checksum 0x%x",
                               downloadingInfo->m_actualChecksum,
                               downloadingInfo->m_expectedChecksum );
                    onDownloadingResult( downloadingInfo, DownloadingResult::Error );
                }
            }
            else {
                requestImageFragment( downloadingInfo, fragmentIndex + 1 );
            }
        },
        [downloadingInfo]( RequestFailType ) {
            ZNET_LOGW( "Request image fragment error\n" );
            onDownloadingResult( downloadingInfo, DownloadingResult::Error );
        } );
}

bool ZCc0x7AFirmwareUpdate::isDownloadingInProgress() const {
    return !m_downloadingInfo.expired();
}

}  // namespace znet
