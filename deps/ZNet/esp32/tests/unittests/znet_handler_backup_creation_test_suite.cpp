#include "fake_owner_fixture.h"
#include "MainFixture.h"
#include <znet_handler_backup_creation.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace znet {
namespace ut {

namespace {

class FakeDestinationImpl {
public:
    void write( size_t size, const uint8_t* src ) {
        m_data.insert( m_data.end(), src, src + size );
    }

    const std::vector<uint8_t>& getData() const {
        return m_data;
    }

private:
    std::vector<uint8_t> m_data;
};

class FakeDestination : public IOWrapper::Destination {
public:
    FakeDestination()
    : m_impl( std::make_shared<FakeDestinationImpl>() ) {}

    void write( size_t size, const uint8_t* src ) override {
        m_impl->write( size, src );
    }

    std::shared_ptr<FakeDestinationImpl> getImpl() const {
        return m_impl;
    }

private:
    std::shared_ptr<FakeDestinationImpl> m_impl;
};

}  // namespace

class ZNetHandlerBackupCreationTestSuite : public MainFixture, public FakeOwnerFixture {
protected:
    void SetUp() override {
        MainFixture::SetUp();

        propagateCallback<ZNetHandlerBackupCreationTestSuite,
                          decltype( &znet_callbacks_t::backup_session_open_result ),
                          &znet_callbacks_t::backup_session_open_result>();

        propagateCallback<ZNetHandlerBackupCreationTestSuite,
                          decltype( &znet_callbacks_t::backup_session_close_result ),
                          &znet_callbacks_t::backup_session_close_result>();

        propagateCallback<ZNetHandlerBackupCreationTestSuite,
                          decltype( &znet_callbacks_t::backup_read_result ),
                          &znet_callbacks_t::backup_read_result>();

        propagateCallback<ZNetHandlerBackupCreationTestSuite,
                          decltype( &znet_callbacks_t::backup_write_result ),
                          &znet_callbacks_t::backup_write_result>();
    }
};

TEST_F( ZNetHandlerBackupCreationTestSuite, successfulBackupCreation ) {
    std::vector<uint8_t> image( 200 );
    for( uint8_t i = 0; i < 200; ++i )
        image[i] = i;

    std::unique_ptr<FakeDestination> destination( new FakeDestination );
    const auto destinationImpl = destination->getImpl();

    EXPECT_CALL( getOwner(), znetStoreReset() ).Times( 0 );
    EXPECT_CALL( getOwner(), znetStoreSave( _, _, _ ) ).Times( 0 );

    EXPECT_CALL( getOwner(), zwaveModuleResetPinAssert() ).Times( 0 );
    EXPECT_CALL( getOwner(), zwaveModuleResetPinRelease() ).Times( 0 );

    getImpl().createChipBackup( std::move( destination ) );

    // activation
    EXPECT_CALL( getOwner(), onUpdateState( IZNet::Mode::BackupCreation, ModeStatus::Invoked, _ ) );

    getImpl().activateMode( IZNet::Mode::BackupCreation );

    EXPECT_TRUE( getImpl().checkState( IZNet::Mode::BackupCreation ) );

    // open session

    getOwner().addTime( std::chrono::milliseconds( 20 ) );

    EXPECT_CALL( getOwner(), onUpdateState( IZNet::Mode::BackupCreation, ModeStatus::Started, _ ) );
    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( OpenBackupSessionRequest() ) );
    getImpl().getRequestsManager().onRespond( OpenBackupSessionRequest(), ZNET_BACKUP_STATUS_OK, 200 );

    // start reading

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( ReadBackupRequest( 0, 64 ) ) );
    EXPECT_CALL( getOwner(), onUpdateState( _, _, _ ) ).Times( 0 );
    getImpl().getRequestsManager().onRespond( ReadBackupRequest( 0, 64 ), ZNET_BACKUP_STATUS_OK, 0, 64, image.data() );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( ReadBackupRequest( 64, 64 ) ) );
    getImpl().getRequestsManager().onRespond(
        ReadBackupRequest( 64, 64 ), ZNET_BACKUP_STATUS_OK, 64, 64, image.data() + 64 );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( ReadBackupRequest( 128, 64 ) ) );
    getImpl().getRequestsManager().onRespond(
        ReadBackupRequest( 128, 64 ), ZNET_BACKUP_STATUS_OK, 128, 64, image.data() + 128 );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( ReadBackupRequest( 192, 64 ) ) );
    getImpl().getRequestsManager().onRespond(
        ReadBackupRequest( 192, 64 ), ZNET_BACKUP_STATUS_END_OF_FILE, 192, 8, image.data() + 192 );

    EXPECT_EQ( destinationImpl.use_count(), 2 );

    // close session
    getOwner().addTime( std::chrono::milliseconds( 20 ) );

    EXPECT_CALL( getOwner(), zwaveModuleResetPinAssert() );
    EXPECT_CALL( getOwner(), zwaveModuleResetPinRelease() );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( CloseBackupSessionRequest() ) );
    getImpl().getRequestsManager().onRespond( CloseBackupSessionRequest(), ZNET_BACKUP_STATUS_OK );

    getOwner().addTime( std::chrono::milliseconds( 500 ) );
    EXPECT_EQ( destinationImpl.use_count(), 2 );

    // check destination is closed
    EXPECT_CALL( getOwner(), zwaveModuleResetPinAssert() ).Times( 0 );
    EXPECT_CALL( getOwner(), zwaveModuleResetPinRelease() ).Times( 0 );
    EXPECT_CALL( getOwner(), onUpdateState( IZNet::Mode::BackupCreation, ModeStatus::Finished, _ ) );

    getOwner().addTime( std::chrono::milliseconds( 500 ) );

    EXPECT_EQ( destinationImpl.use_count(), 1 );

    EXPECT_EQ( destinationImpl->getData(), image );

    EXPECT_TRUE( getImpl().checkState( IZNet::Mode::Idle ) );
}

}  // namespace ut
}  // namespace znet
