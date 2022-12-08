#include "fake_owner_fixture.h"
#include "MainFixture.h"
#include <znet_handler_backup_restoring.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace znet {
namespace ut {

namespace {

class FakeSourceImpl {
public:
    FakeSourceImpl()
    : m_data( 200 ) {
        for( uint8_t i = 0; i < 200; ++i )
            m_data[i] = i;
    }

    size_t read( size_t size, uint8_t* dst ) {
        const auto result = std::min( size, m_data.size() - m_pos );
        memcpy( dst, m_data.data() + m_pos, result );
        m_pos += result;
        return result;
    }

    const std::vector<uint8_t>& getData() const {
        return m_data;
    }

    size_t getPos() const {
        return m_pos;
    }

private:
    std::vector<uint8_t> m_data;
    size_t m_pos = 0;
};

class FakeSource : public IOWrapper::Source {
public:
    FakeSource()
    : m_impl( std::make_shared<FakeSourceImpl>() ) {}

    size_t read( size_t size, uint8_t* dst ) override {
        return m_impl->read( size, dst );
    }

    void seek( size_t pos ) override {
        FAIL();
    }

    std::shared_ptr<FakeSourceImpl> getImpl() const {
        return m_impl;
    }

private:
    std::shared_ptr<FakeSourceImpl> m_impl;
};

}  // namespace

class ZNetHandlerBackupRestoringTestSuite : public MainFixture, public FakeOwnerFixture {
protected:
    void SetUp() override {
        MainFixture::SetUp();

        propagateCallback<ZNetHandlerBackupRestoringTestSuite,
                          decltype( &znet_callbacks_t::backup_session_open_result ),
                          &znet_callbacks_t::backup_session_open_result>();

        propagateCallback<ZNetHandlerBackupRestoringTestSuite,
                          decltype( &znet_callbacks_t::backup_session_close_result ),
                          &znet_callbacks_t::backup_session_close_result>();

        propagateCallback<ZNetHandlerBackupRestoringTestSuite,
                          decltype( &znet_callbacks_t::backup_read_result ),
                          &znet_callbacks_t::backup_read_result>();

        propagateCallback<ZNetHandlerBackupRestoringTestSuite,
                          decltype( &znet_callbacks_t::backup_write_result ),
                          &znet_callbacks_t::backup_write_result>();
    }
};

TEST_F( ZNetHandlerBackupRestoringTestSuite, successfulBackupRestoring ) {
    std::unique_ptr<FakeSource> source( new FakeSource );
    const auto sourceImpl = source->getImpl();

    EXPECT_CALL( getOwner(), znetStoreReset() ).Times( 0 );
    EXPECT_CALL( getOwner(), znetStoreSave( _, _, _ ) ).Times( 0 );

    getImpl().restoreChipBackup( std::move( source ) );

    // activation
    EXPECT_CALL( getOwner(), onUpdateState( IZNet::Mode::BackupRestoring, ModeStatus::Invoked, _ ) );

    getImpl().activateMode( IZNet::Mode::BackupRestoring );

    EXPECT_TRUE( getImpl().checkState( IZNet::Mode::BackupRestoring ) );

    // set default
    getOwner().addTime( std::chrono::milliseconds( 20 ) );
    EXPECT_CALL( getOwner(), onUpdateState( _, _, _ ) ).Times( 0 );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( SetDefaultRawRequest() ) );
    getImpl().getRequestsManager().onRespond( SetDefaultRawRequest() );

    // open session

    getOwner().addTime( std::chrono::milliseconds( 20 ) );

    EXPECT_CALL( getOwner(), onUpdateState( IZNet::Mode::BackupRestoring, ModeStatus::Started, _ ) );
    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( OpenBackupSessionRequest() ) );
    getImpl().getRequestsManager().onRespond( OpenBackupSessionRequest(), ZNET_BACKUP_STATUS_OK, 200 );

    // start writting
    EXPECT_CALL( getOwner(), onUpdateState( _, _, _ ) ).Times( 0 );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( WriteBackupRequest( 0, 64, nullptr ) ) );
    getImpl().getRequestsManager().onRespond( WriteBackupRequest( 0, 64, nullptr ), ZNET_BACKUP_STATUS_OK, 0, 64 );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( WriteBackupRequest( 64, 64, nullptr ) ) );
    getImpl().getRequestsManager().onRespond( WriteBackupRequest( 64, 64, nullptr ), ZNET_BACKUP_STATUS_OK, 64, 64 );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( WriteBackupRequest( 128, 64, nullptr ) ) );
    getImpl().getRequestsManager().onRespond( WriteBackupRequest( 128, 64, nullptr ), ZNET_BACKUP_STATUS_OK, 128, 64 );

    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( WriteBackupRequest( 192, 64, nullptr ) ) );
    getImpl().getRequestsManager().onRespond( WriteBackupRequest( 192, 8, nullptr ), ZNET_BACKUP_STATUS_OK, 192, 8 );

    EXPECT_EQ( sourceImpl.use_count(), 2 );

    // close session
    getOwner().addTime( std::chrono::milliseconds( 20 ) );
    EXPECT_TRUE( getImpl().getRequestsManager().isWaitingForRespond( CloseBackupSessionRequest() ) );
    getImpl().getRequestsManager().onRespond( CloseBackupSessionRequest(), ZNET_BACKUP_STATUS_OK );

    // soft reset
    EXPECT_CALL( getOwner(), onUpdateState( IZNet::Mode::BackupRestoring, ModeStatus::Finished, _ ) );
    getOwner().addTime( std::chrono::milliseconds( 2000 ) );

    EXPECT_EQ( sourceImpl.use_count(), 1 );

    EXPECT_EQ( sourceImpl->getPos(), 200 );

    EXPECT_TRUE( getImpl().checkState( IZNet::Mode::BackupRestoring ) );  // ?
}

}  // namespace ut
}  // namespace znet
