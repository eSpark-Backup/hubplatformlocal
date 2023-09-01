#include "znet/znet_provisioning_list_query_executor.hpp"

#include "znet_provisioning_list_entry_generator.hpp"
#include "znet_provisioning_list_entry_matchers.hpp"
#include "znet_mock_provisioning_list_data_source.hpp"

namespace znet {
namespace ut {

using ::testing::_;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::StrictMock;

class ProvisioningListQuery_execute : public ::testing::Test, public ProvisioningListEntryGenerator {
public:
    static std::unique_ptr<MockProvisioningListQueryDataSource> makeDataSourceWithOneEntry(
        const ::znet::ProvisioningListEntry& entry ) {
        using controller_t = ::znet::api::IProvisioningListController;
        std::unique_ptr<MockProvisioningListQueryDataSource> dataSource( new MockProvisioningListQueryDataSource );
        ON_CALL( *dataSource, iterate( _ ) )
            .WillByDefault( [entry]( const controller_t::ProcessProvisioningCallback& cb ) {
                return cb( entry ) ? controller_t::OperationStatus::SUCCESS :
                                     controller_t::OperationStatus::ENTRY_NOT_FOUND;
            } );
        return dataSource;
    }
};

TEST_F( ProvisioningListQuery_execute, ShouldReturnError_WhenExecutingWithoutFilters ) {
    // Arrange

    MockProvisioningListQueryDataSource dataSource;

    // Act

    ::znet::api::IProvisioningListController::OperationStatus status =
        ::znet::api::ProvisioningListQueryExecutor::execute(
            dataSource, ::znet::api::ReadOnlyStatement{}, ::znet::api::ModifyingStatement{} );

    // Assert

    ASSERT_EQ( status, ::znet::api::IProvisioningListController::OperationStatus::QUERY_NOT_READY );
}

TEST_F( ProvisioningListQuery_execute, ShouldInvokeFilter_WhenStorageHasEntries ) {
    // Arrange

    auto entry = makeEntry( makeProvisioningID(), makeDSK() );
    auto dataSource = makeDataSourceWithOneEntry( entry );

    MockFunction<bool( const ::znet::ProvisioningListEntry& )> userFilter;
    ON_CALL( userFilter, Call( MatchEntry( entry ) ) )
        .WillByDefault( Return( true ) );  // Simulate match to get OperationStatus::SUCCESS

    ::znet::api::ReadOnlyStatement readOnlyStatement;
    readOnlyStatement.addClause( userFilter.AsStdFunction() );

    // Act

    ::znet::api::IProvisioningListController::OperationStatus status =
        ::znet::api::ProvisioningListQueryExecutor::execute(
            *dataSource, readOnlyStatement, ::znet::api::ModifyingStatement{} );

    // Assert

    ASSERT_EQ( status, ::znet::api::IProvisioningListController::OperationStatus::SUCCESS );
}

TEST_F( ProvisioningListQuery_execute, ShouldInvokeSequenceOfFilters_WhenStorageHasEntries ) {
    // Arrange

    auto entry = makeEntry( makeProvisioningID(), makeDSK() );
    auto dataSource = makeDataSourceWithOneEntry( entry );

    // Simulate match to proceed to the next filter
    MockFunction<bool( const ::znet::ProvisioningListEntry& )> userFilter1;
    ON_CALL( userFilter1, Call( MatchEntry( entry ) ) ).WillByDefault( Return( true ) );

    // Simulate mismatch to get OperationStatus::ENTRY_NOT_FOUND
    MockFunction<bool( const ::znet::ProvisioningListEntry& )> userFilter2;
    EXPECT_CALL( userFilter2, Call( MatchEntry( entry ) ) ).WillOnce( Return( false ) );

    ::znet::api::ReadOnlyStatement readOnlyStatement;
    readOnlyStatement.addClause( userFilter1.AsStdFunction() );
    readOnlyStatement.addClause( userFilter2.AsStdFunction() );

    // Act

    ::znet::api::IProvisioningListController::OperationStatus status =
        ::znet::api::ProvisioningListQueryExecutor::execute(
            *dataSource, readOnlyStatement, ::znet::api::ModifyingStatement{} );

    // Assert

    ASSERT_EQ( status, ::znet::api::IProvisioningListController::OperationStatus::ENTRY_NOT_FOUND );
}

TEST_F( ProvisioningListQuery_execute, ShouldInvokeModifyingClause_WhenFilterMatchedEntry ) {
    // Arrange

    auto originalEntry = makeEntry( makeProvisioningID(), makeDSK() );
    auto modifiedEntry = makeEntry( originalEntry.m_provisioningId, originalEntry.m_dsk );

    auto dataSource = makeDataSourceWithOneEntry( originalEntry );
    ON_CALL( *dataSource, update( MatchEntry( modifiedEntry ) ) )
        .WillByDefault( Return(::znet::api::IProvisioningListController::OperationStatus::SUCCESS ) );

    // Simulate match to proceed to the modifying clause
    MockFunction<bool( const ::znet::ProvisioningListEntry& )> userFilter;
    ON_CALL( userFilter, Call( MatchEntry( originalEntry ) ) ).WillByDefault( Return( true ) );

    // Simulate some modifying operation
    MockFunction<void(::znet::ProvisioningListEntry& )> modifyingClause;
    ON_CALL( modifyingClause, Call( MatchRefEntry( originalEntry ) ) )
        .WillByDefault( SetArgReferee<0>( modifiedEntry ) );

    ::znet::api::ReadOnlyStatement readOnlyStatement;
    readOnlyStatement.addClause( userFilter.AsStdFunction() );

    ::znet::api::ModifyingStatement modifyingStatement;
    modifyingStatement.addClause( modifyingClause.AsStdFunction() );

    // Act

    ::znet::api::IProvisioningListController::OperationStatus status =
        ::znet::api::ProvisioningListQueryExecutor::execute( *dataSource, readOnlyStatement, modifyingStatement );

    // Assert

    ASSERT_EQ( status, ::znet::api::IProvisioningListController::OperationStatus::SUCCESS );
}

TEST_F( ProvisioningListQuery_execute, ShouldNotInvokeModifyingClause_WhenFilterMatchedNoEntries ) {
    // Arrange

    auto entry = makeEntry( makeProvisioningID(), makeDSK() );
    auto dataSource = makeDataSourceWithOneEntry( entry );

    // Simulate mismatch
    MockFunction<bool( const ::znet::ProvisioningListEntry& )> userFilter;
    ON_CALL( userFilter, Call( MatchEntry( entry ) ) ).WillByDefault( Return( false ) );

    // Explicitly deny any calls
    StrictMock<MockFunction<void(::znet::ProvisioningListEntry& )>> modifyingClause;

    ::znet::api::ReadOnlyStatement readOnlyStatement;
    readOnlyStatement.addClause( userFilter.AsStdFunction() );

    ::znet::api::ModifyingStatement modifyingStatement;
    modifyingStatement.addClause( modifyingClause.AsStdFunction() );

    // Act

    ::znet::api::IProvisioningListController::OperationStatus status =
        ::znet::api::ProvisioningListQueryExecutor::execute( *dataSource, readOnlyStatement, modifyingStatement );

    // Assert
    ASSERT_EQ( status, ::znet::api::IProvisioningListController::OperationStatus::ENTRY_NOT_FOUND );
}

}  // namespace ut
}  // namespace znet
