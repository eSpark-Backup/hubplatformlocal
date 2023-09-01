#include "znet_handler_backup_restoring.hpp"
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
constexpr std::chrono::milliseconds SoftResetDelay( 1500 + 300 );
}  // namespace

bool BackupRestoringHandler::setSource( IOWrapper::SourceUPtr source ) {
    if( m_source || !source ) {
        return false;
    }

    m_source = std::move( source );

    if( m_parentMesh.checkState( IZNet::Mode::BackupRestoring, ModeStatus::Failed ) ) {
        initialize();
    }

    return true;
}

bool BackupRestoringHandler::isDirty() const {
    return m_source != nullptr;
}

void BackupRestoringHandler::initialize() {
    ZNET_LOGI( "prepare backup restoring (reset chip)\n" );

    m_parentMesh.setState( IZNet::Mode::BackupRestoring, ModeStatus::Invoked );

    m_parentMesh.getRequestsManager().doRequest(
        SetDefaultRawRequest(),
        {},
        [this]() {
            ZNET_LOGI( "chip reset done, start opening backup session\n" );

            openSession();
        },
        [this]( RequestFailType failType ) { onFinished( err::message( ZNET_ERR_BKP_RST_SET_DEFAULT, failType ) ); },
        PostponedRequestsDelay );
}

void BackupRestoringHandler::openSession() {
    m_parentMesh.getRequestsManager().doRequest(
        OpenBackupSessionRequest(),
        {},
        [this]( znet_backup_status_t status, uint16_t fileSize ) {
            if( status == ZNET_BACKUP_STATUS_OK ) {
                ZNET_LOGI( "Backup restoring session open\n" );

                m_parentMesh.setState( IZNet::Mode::BackupRestoring, ModeStatus::Started );

                writeDataRequest( 0, false, CRC16_DEFAULT_INIT_VALUE );
            }
            else {
                closeSession( err::bkp::message( ZNET_ERR_BKP_RST_OPEN_STATUS, status ) );
            }
        },
        [this]( RequestFailType failType ) { closeSession( err::message( ZNET_ERR_BKP_RST_OPEN, failType ) ); },
        PostponedRequestsDelay );
}

void BackupRestoringHandler::writeDataRequest( uint16_t offset, bool expectedEof, uint16_t checksum ) {
    uint8_t data[FrameSize] = {
        0,
    };
    const auto readBytes = m_source->read( FrameSize, data );
    checksum = crc16_with_init_value( data, readBytes, checksum );
    if( readBytes == 0 ) {
        ZNET_LOGI( "All data has been written. Size = %u, checksum = 0x%x\n", offset + readBytes, checksum );
        closeSession();
    }
    else if( expectedEof ) {
        closeSession( err::bkp::backup_write_eof_expected( readBytes, offset ) );
    }
    else {
        const auto sendingResult = m_parentMesh.getRequestsManager().doRequest(
            WriteBackupRequest( offset, readBytes, data ),
            {},
            [=]( znet_backup_status_t status, uint16_t reportedOffset, uint8_t reportedLength ) {
                assert( readBytes <= FrameSize );

                if( status == ZNET_BACKUP_STATUS_OK || status == ZNET_BACKUP_STATUS_END_OF_FILE ) {
                    if( offset != reportedOffset ) {
                        closeSession( err::bkp::message( ZNET_ERR_BKP_WRITE, reportedOffset, offset ) );
                    }
                    else {
                        writeDataRequest( static_cast<uint16_t>( offset + readBytes ),
                                          status == ZNET_BACKUP_STATUS_END_OF_FILE,
                                          checksum );
                    }
                }
                else {
                    closeSession( err::bkp::wrong_backup_write_status( status, offset ) );
                }
            },
            [this]( RequestFailType failType ) { closeSession( err::message( ZNET_ERR_BKP_WRITE, failType ) ); } );

        if( !sendingResult ) {
            closeSession( err::message( ZNET_ERR_BKP_WRITE_REQ ) );
        }
    }
}

void BackupRestoringHandler::closeSession( ErrorOpt previousErrorMessage ) {
    if( previousErrorMessage ) {
        ZNET_LOGE( "%s\n", utils::stringify( *previousErrorMessage ).c_str() );
    }

    auto onSessionClosed = [this]( ErrorOpt errorMessage ) {
        if( !errorMessage ) {
            ZNET_LOGI( "Backup session closed, start soft reset\n" );
            if( znet_serial_api_soft_reset() == ZNET_ERR_OK ) {
                m_timer = m_parentMesh.getOwner().createTimer( [=]() {
                    ZNET_LOGI( "Soft reset done. Backup restored\n" );

                    onFinished();

                    m_timer.reset();
                } );

                m_timer->runOnce( SoftResetDelay );
            }
            else {
                ZNET_LOGW( "Fail to run znet soft reset\n" );

                onFinished();  // append error here?
            }
        }
        else {
            onFinished( errorMessage );
        }
    };

    m_parentMesh.getRequestsManager().doRequest(
        CloseBackupSessionRequest(),
        {},
        [=]( znet_backup_status_t status ) {
            if( status == ZNET_BACKUP_STATUS_OK || status == ZNET_BACKUP_STATUS_END_OF_FILE ) {
                ZNET_LOGI( "Backup session closed\n" );
                onSessionClosed( {} );
            }
            else {
                auto errorMessage = err::bkp::message( ZNET_ERR_BKP_CLOSE_STATUS, status );
                if( previousErrorMessage ) {
                    errorMessage.m_details.emplace_back( resources::err::PreviousError, previousErrorMessage->m_data );
                }
                onSessionClosed( std::move( errorMessage ) );
            }
        },
        [onSessionClosed, previousErrorMessage]( RequestFailType failType ) {
            auto errorMessage = err::message( ZNET_ERR_BKP_CLOSE_REQ, failType );
            if( previousErrorMessage ) {
                errorMessage.m_details.emplace_back( resources::err::PreviousError, previousErrorMessage->m_data );
            }
            onSessionClosed( std::move( errorMessage ) );
        },
        PostponedRequestsDelay );
}

void BackupRestoringHandler::onFinished( ErrorOpt errorMessage ) {
    m_source.reset();

    if( !errorMessage ) {
        m_parentMesh.setState( IZNet::Mode::BackupRestoring, ModeStatus::Finished );
    }
    else {
        ZNET_LOGE( "Backup finished with error: %s\n", utils::stringify( *errorMessage ).c_str() );
        m_parentMesh.setState( IZNet::Mode::BackupRestoring, ModeStatus::Failed, errorMessage );
    }
}

void BackupRestoringHandler::onDeactivated() {
    assert( !m_source );
}

}  // namespace znet
