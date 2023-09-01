#include "znet_handler_backup_creation.hpp"
#include "znet_impl.hpp"
#include "znet/znet_utils.hpp"
#include "znet/converters/resources.hpp"

extern "C" {
#include "znet_log.h"
#include "byte_ops.h"
}

namespace znet {

namespace {
constexpr uint8_t FrameSize = 64;
constexpr std::chrono::milliseconds PostponedRequestsDelay( 20 );
constexpr std::chrono::milliseconds ChipResetDelay( 600 );
}  // namespace

bool BackupCreationHandler::setDestination( IOWrapper::DestinationUPtr destination ) {
    if( m_destination || !destination ) {
        return false;
    }

    assert( !m_timer );

    m_destination = std::move( destination );
    return true;
}

bool BackupCreationHandler::isDirty() const {
    return m_destination != nullptr;
}

void BackupCreationHandler::initialize() {
    ZNET_LOGI( "start backup creation\n" );
    m_parentMesh.setState( IZNet::Mode::BackupCreation, ModeStatus::Invoked );

    openSession();
}

void BackupCreationHandler::openSession() {
    m_parentMesh.getRequestsManager().doRequest(
        OpenBackupSessionRequest(),
        {},
        [this]( znet_backup_status_t status, uint16_t fileSize ) {
            if( status == ZNET_BACKUP_STATUS_OK ) {
                ZNET_LOGI( "Backup session open\n" );

                m_parentMesh.setState( IZNet::Mode::BackupCreation, ModeStatus::Started );

                readDataRequest( 0, fileSize, CRC16_DEFAULT_INIT_VALUE );
            }
            else {
                closeSession( err::bkp::message( ZNET_ERR_BKP_OPEN_STATUS, status ) );
            }
        },
        [this]( RequestFailType failType ) { closeSession( err::message( ZNET_ERR_BKP_OPEN, failType ) ); },
        PostponedRequestsDelay );
}

void BackupCreationHandler::readDataRequest( uint16_t offset, uint16_t fileSize, uint16_t checksum ) {
    const auto readingResult = m_parentMesh.getRequestsManager().doRequest(
        ReadBackupRequest( offset, FrameSize ),
        {},
        [=]( znet_backup_status_t status, uint16_t reportedOffset, uint8_t dataLength, const uint8_t* data ) {
            if( status == ZNET_BACKUP_STATUS_OK ) {
                if( offset == reportedOffset ) {
                    m_destination->write( dataLength, data );
                    readDataRequest(
                        reportedOffset + dataLength, fileSize, crc16_with_init_value( data, dataLength, checksum ) );
                }
                else {
                    closeSession( err::bkp::message( ZNET_ERR_BKP_READ, reportedOffset, offset ) );
                }
            }
            else if( status == ZNET_BACKUP_STATUS_END_OF_FILE ) {
                if( fileSize != reportedOffset + dataLength ) {
                    ZNET_LOGW( "Expected size is %u but EOF is %u\n", fileSize, reportedOffset + dataLength );
                }
                else {
                    ZNET_LOGI( "Backup EOF (%u) has been successfully reached, checksum = 0x%x\n",
                               fileSize,
                               crc16_with_init_value( data, dataLength, checksum ) );
                }

                if( offset == reportedOffset ) {
                    m_destination->write( dataLength, data );
                    closeSession();
                }
                else {
                    closeSession( err::bkp::message( ZNET_ERR_BKP_READ, reportedOffset, offset ) );
                }
            }
            else {
                closeSession( err::bkp::wrong_backup_read_status( status, offset, dataLength ) );
            }
        },
        [this]( RequestFailType failType ) { closeSession( err::message( ZNET_ERR_BKP_READ, failType ) ); } );

    if( !readingResult ) {
        closeSession( err::message( ZNET_ERR_BKP_READ_REQ ) );
    }
}

void BackupCreationHandler::closeSession( ErrorOpt previousErrorMessage ) {
    if( previousErrorMessage ) {
        ZNET_LOGE( "%s\n", utils::stringify( *previousErrorMessage ).c_str() );
    }

    auto onSessionClosed = [this]( ErrorOpt errorMessage ) {
        ZNET_LOGI( "Do 700 series hanging workaround: assert/release pin\n" );

        m_parentMesh.getOwner().zwaveModuleResetPinAssert();

        m_parentMesh.getOwner().zwaveModuleResetPinRelease();

        m_timer = m_parentMesh.getRequestsManager().getTimerProvider().createTimer( [=]() {
            onFinished( errorMessage );

            m_timer.reset();
        } );

        m_timer->runOnce( ChipResetDelay );
    };

    m_parentMesh.getRequestsManager().doRequest(
        CloseBackupSessionRequest(),
        {},
        [=]( znet_backup_status_t status ) {
            if( status == ZNET_BACKUP_STATUS_OK || status == ZNET_BACKUP_STATUS_END_OF_FILE ) {
                ZNET_LOGI( "Backup session closed" );
                onSessionClosed( previousErrorMessage );
            }
            else {
                auto errorMessage = err::bkp::message( ZNET_ERR_BKP_CLOSE_STATUS, status );
                if( previousErrorMessage ) {
                    errorMessage.m_details.emplace_back( resources::err::PreviousError, previousErrorMessage->m_data );
                }
                onSessionClosed( std::move( errorMessage ) );
            }
        },
        [onSessionClosed]( RequestFailType failType ) {
            onSessionClosed( err::message( ZNET_ERR_BKP_CLOSE, failType ) );
        },
        PostponedRequestsDelay );
}

void BackupCreationHandler::onFinished( ErrorOpt errorMessage ) {
    m_destination.reset();

    if( !errorMessage ) {
        ZNET_LOGI( "Backup creation finished\n" );
        m_parentMesh.setState( IZNet::Mode::BackupCreation, ModeStatus::Finished );
    }
    else {
        ZNET_LOGE( "Backup creation finished with error: %s\n", utils::stringify( *errorMessage ).c_str() );
        m_parentMesh.setState( IZNet::Mode::BackupCreation, ModeStatus::Failed, std::move( errorMessage ) );
    }

    m_parentMesh.activateMode( IZNet::Mode::Monitoring );
}

void BackupCreationHandler::onDeactivated() {
    assert( !m_destination );
}

}  // namespace znet
