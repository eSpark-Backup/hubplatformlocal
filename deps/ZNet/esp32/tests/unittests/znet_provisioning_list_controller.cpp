#include "znet/znet_provisioning_list_controller.hpp"

#include "znet_provisioning_list_controller_fixtures.hpp"
#include "znet_provisioning_list_entry_matchers.hpp"

#include "znet_mock_provisioning_list_observer.hpp"

namespace znet {
namespace ut {

using ::testing::_;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::ReturnPointee;
using ::testing::SetArgReferee;
using ::testing::StrictMock;

TEST_F( ProvisioningListEntryAdd, ShouldAddNewEntry_WhenStorageContainsNoEntriesWithSameDSK ) {
    // Arrange

    ON_CALL( m_provider, storeProvisioningListEntry( MatchFullEntryExceptID( m_entry ) ) )
        .WillByDefault( Return( true ) );

    // Act & assert

    MockProvisioningListObserver observer;

    // Mandatory contract
    EXPECT_CALL( observer, onProvisioningListEntryAdded( MatchFullEntryExceptID( m_entry ) ) );

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::SUCCESS,
              controller.addProvisioningListEntry( m_entry.m_dsk, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntryAdd, ShouldReplaceExistingEntryFields_WhenStorageContainsEntryWithSameDSK ) {
    // Arrange

    // Existing entry with same DSK
    auto existingEntry = makeEntry( m_entry.m_provisioningId, m_entry.m_dsk );
    ::znet::ProvisioningListEntry updatedEntry{ existingEntry.m_provisioningId, existingEntry.m_dsk, m_entry.m_fields };

    ON_CALL( m_provider, getProvisioningListEntryCount() ).WillByDefault( Return( 1 ) );

    ON_CALL( m_provider, findProvisioningListEntryByDSK( m_entry.m_dsk, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( existingEntry ), Return( true ) ) );

    ON_CALL( m_provider, storeProvisioningListEntry( MatchEntry( updatedEntry ) ) ).WillByDefault( Return( true ) );

    // Act & assert

    MockProvisioningListObserver observer;

    // Mandatory contract
    EXPECT_CALL( observer, onProvisioningListEntryUpdated( MatchEntry( existingEntry ), MatchEntry( updatedEntry ) ) );

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::ENTRY_REPLACED,
              controller.addProvisioningListEntry( m_entry.m_dsk, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntryAdd, ShouldSetDefaults_WhenRequiredEntryFieldsAreMissing ) {
    // Reset all input fields
    m_entry.m_fields = {};

    ::znet::ProvisioningListEntry expectedEntryWithDefaults{
        .m_provisioningId = m_entry.m_provisioningId, .m_dsk = m_entry.m_dsk, .m_fields = makeDefaultEntryFields()
    };

    MockProvisioningListProvider provider;

    ON_CALL( provider, storeProvisioningListEntry( MatchFullEntryExceptID( expectedEntryWithDefaults ) ) )
        .WillByDefault( Return( true ) );

    MockProvisioningListObserver observer;

    // Mandatory contract
    EXPECT_CALL( observer, onProvisioningListEntryAdded( MatchFullEntryExceptID( expectedEntryWithDefaults ) ) );

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::SUCCESS,
              controller.addProvisioningListEntry( m_entry.m_dsk, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntryAdd, ShouldReturnError_WhenStorageRejectsAllModifyingOperations ) {
    // Arrange

    ON_CALL( m_provider, storeProvisioningListEntry( _ ) ).WillByDefault( Return( false ) );

    // Act & assert

    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::STORAGE_NOT_AVAILABLE,
              controller.addProvisioningListEntry( m_entry.m_dsk, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntryAdd, ShouldReturnError_WhenAddingMoreEntriesThanAllowed ) {
    // Arrange

    ON_CALL( m_provider, storeProvisioningListEntry( _ ) ).WillByDefault( Return( true ) );

    ON_CALL( m_provider, getProvisioningListEntryCount() )
        .WillByDefault( Return(::znet::api::ProvisioningListController::MAX_RECORD_COUNT ) );

    // Act & assert

    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::ENTRY_LIMIT_REACHED,
              controller.addProvisioningListEntry( m_entry.m_dsk, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntryAdd, IncorrectAddingMode ) {
    m_entry.m_fields = makeDefaultEntryFields();

    m_entry.m_fields.m_addingMode = ::znet::ProvisioningListEntryFields::AddingMode::SMART;
    m_entry.m_fields.m_supportedAddingModes = { ::znet::ProvisioningListEntryFields::AddingMode::S2 };

    MockProvisioningListObserver observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::INCORRECT_ADDING_MODE,
              controller.addProvisioningListEntry( m_entry.m_dsk, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntrySet, ShouldReplaceExistingEntryFields_WhenStorageContainsEntryWithGivenID ) {
    // Arrange

    auto existingEntry = makeEntry( m_entry.m_provisioningId, m_entry.m_dsk );
    ::znet::ProvisioningListEntry updatedEntry{ existingEntry.m_provisioningId, existingEntry.m_dsk, m_entry.m_fields };

    ON_CALL( m_provider, findProvisioningListEntryByID( existingEntry.m_provisioningId, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( existingEntry ), Return( true ) ) );

    ON_CALL( m_provider, storeProvisioningListEntry( MatchEntry( updatedEntry ) ) ).WillByDefault( Return( true ) );

    // Act & assert

    MockProvisioningListObserver observer;

    // Mandatory contract
    EXPECT_CALL( observer, onProvisioningListEntryUpdated( MatchEntry( existingEntry ), MatchEntry( updatedEntry ) ) );

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::SUCCESS,
              controller.setProvisioningListEntry( m_entry.m_provisioningId, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntrySet, IncorrectAddingMode ) {
    // Arrange

    auto existingEntry = makeEntry( m_entry.m_provisioningId, m_entry.m_dsk );
    existingEntry.m_fields.m_supportedAddingModes = { ::znet::ProvisioningListEntryFields::AddingMode::S2 };
    existingEntry.m_fields.m_addingMode = ::znet::ProvisioningListEntryFields::AddingMode::S2;

    ::znet::ProvisioningListEntry updatedEntry{
        existingEntry.m_provisioningId, existingEntry.m_dsk, existingEntry.m_fields
    };

    updatedEntry.m_fields.m_addingMode = ::znet::ProvisioningListEntryFields::AddingMode::SMART;

    ON_CALL( m_provider, findProvisioningListEntryByID( existingEntry.m_provisioningId, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( existingEntry ), Return( true ) ) );

    MockProvisioningListObserver observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::INCORRECT_ADDING_MODE,
              controller.setProvisioningListEntry( updatedEntry.m_provisioningId, updatedEntry.m_fields ) );
}

TEST_F( ProvisioningListEntrySet, ShouldSetDefaults_WhenRequiredEntryFieldsAreMissing ) {
    auto existingEntry = makeEntry( m_entry.m_provisioningId, m_entry.m_dsk );

    // Reset all fields
    m_entry.m_fields = {};

    ::znet::ProvisioningListEntry updatedEntryWithDefaults{
        .m_provisioningId = m_entry.m_provisioningId, .m_dsk = m_entry.m_dsk, .m_fields = makeDefaultEntryFields()
    };

    MockProvisioningListProvider provider;

    ON_CALL( provider, findProvisioningListEntryByID( existingEntry.m_provisioningId, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( existingEntry ), Return( true ) ) );

    ON_CALL( provider, storeProvisioningListEntry( MatchEntry( updatedEntryWithDefaults ) ) )
        .WillByDefault( Return( true ) );

    MockProvisioningListObserver observer;

    // Mandatory contract
    EXPECT_CALL(
        observer,
        onProvisioningListEntryUpdated( MatchEntry( existingEntry ), MatchEntry( updatedEntryWithDefaults ) ) );

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::SUCCESS,
              controller.setProvisioningListEntry( m_entry.m_provisioningId, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntrySet, ShouldReturnError_WhenStorageContainsNoEntriesWithGivenID ) {
    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::ENTRY_NOT_FOUND,
              controller.setProvisioningListEntry( m_entry.m_provisioningId, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntrySet, ShouldReturnError_WhenStorageRejectsAllModifyingOperations ) {
    // Arrange

    auto existingEntry = makeEntry( m_entry.m_provisioningId, m_entry.m_dsk );

    ON_CALL( m_provider, findProvisioningListEntryByID( existingEntry.m_provisioningId, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( existingEntry ), Return( true ) ) );

    ON_CALL( m_provider, storeProvisioningListEntry( _ ) ).WillByDefault( Return( false ) );

    // Act & assert

    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::STORAGE_NOT_AVAILABLE,
              controller.setProvisioningListEntry( m_entry.m_provisioningId, m_entry.m_fields ) );
}

TEST_F( ProvisioningListEntryRemove, ShouldRemoveEntry_WhenStorageContainsEntryWithGivenID ) {
    // Arrange

    // Reset this flag once remove expectation is satisfied
    bool entryExists = true;

    ON_CALL( m_provider, findProvisioningListEntryByID( m_entry.m_provisioningId, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( m_entry ), ReturnPointee( &entryExists ) ) );

    // Remove on first call, then reject
    ON_CALL( m_provider, removeProvisioningListEntryByID( m_entry.m_provisioningId ) )
        .WillByDefault( InvokeWithoutArgs( [&] { return entryExists && !( entryExists = false ); } ) );

    // Act & assert

    MockProvisioningListObserver observer;

    // Mandatory contract
    EXPECT_CALL( observer, onProvisioningListEntryRemoved( m_entry.m_provisioningId ) );

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::SUCCESS,
              controller.removeProvisioningListEntry( m_entry.m_provisioningId ) );
}

TEST_F( ProvisioningListEntryRemove, ShouldReturnError_WhenStorageContainsNoEntriesWithGivenID ) {
    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::ENTRY_NOT_FOUND,
              controller.removeProvisioningListEntry( m_entry.m_provisioningId ) );
}

TEST_F( ProvisioningListEntryRemove, ShouldReturnError_WhenStorageRejectsAllModifyingOperations ) {
    // Arrange

    ON_CALL( m_provider, findProvisioningListEntryByID( m_entry.m_provisioningId, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( m_entry ), Return( true ) ) );

    ON_CALL( m_provider, removeProvisioningListEntryByID( _ ) ).WillByDefault( Return( false ) );

    // Act & assert

    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, m_provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::STORAGE_NOT_AVAILABLE,
              controller.removeProvisioningListEntry( m_entry.m_provisioningId ) );
}

TEST_F( ProvisioningListEnumerate, ShouldCallBackWithEachEntry_WhenStorageIsNonEmpty ) {
    // Arrange

    MockProvisioningListProvider provider;

    ON_CALL( provider, getProvisioningListEntryCount() ).WillByDefault( Return( m_entries.size() ) );
    ON_CALL( provider, loadProvisioningList( _ ) )
        .WillByDefault( Invoke( [&](::znet::IProvisioningListProvider::ProcessProvisioningCallback entryCallback ) {
            for( auto& entry : m_entries ) {
                entryCallback( entry );
            }
            return true;
        } ) );

    // Act & assert

    // A client-side callback that is supposed to be called by the controller for each entry
    MockFunction<bool( const ::znet::ProvisioningListEntry& )> userDefinedCallback;

    for( auto& entry : m_entries ) {
        EXPECT_CALL( userDefinedCallback, Call( MatchEntry( entry ) ) ).WillOnce( Return( false ) );
    }

    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::ENTRY_NOT_FOUND,
              controller.findProvisioningListEntry( userDefinedCallback.AsStdFunction() ) );
}

TEST_F( ProvisioningListEnumerate, ShouldStopEnumerateEntries_WhenCallbackReturnsTrue ) {
    // Arrange

    MockProvisioningListProvider provider;

    ON_CALL( provider, getProvisioningListEntryCount() ).WillByDefault( Return( m_entries.size() ) );
    ON_CALL( provider, loadProvisioningList( _ ) )
        .WillByDefault( Invoke( [&](::znet::IProvisioningListProvider::ProcessProvisioningCallback entryCallback ) {
            std::find_if( m_entries.begin(), m_entries.end(), entryCallback );
            return true;
        } ) );

    // Act & assert

    // A client-side callback that is supposed to be called by the controller for each entry
    MockFunction<bool( const ::znet::ProvisioningListEntry& )> userDefinedCallback;

    // Expect only one call, since the callback will return true
    EXPECT_CALL( userDefinedCallback, Call( MatchEntry( m_entries[0] ) ) ).WillOnce( Return( true ) );

    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::SUCCESS,
              controller.findProvisioningListEntry( userDefinedCallback.AsStdFunction() ) );
}

TEST_F( ProvisioningListEnumerate, ShouldNotCallBack_WhenStorageIsEmpty ) {
    // Arrange

    MockProvisioningListProvider provider;

    ON_CALL( provider, getProvisioningListEntryCount() ).WillByDefault( Return( 0 ) );
    ON_CALL( provider, loadProvisioningList( _ ) ).WillByDefault( Return( true ) );

    // Act & assert

    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    // A client-side callback that is supposed to be called by the controller for each entry, not expecting any calls
    StrictMock<MockFunction<bool( const ::znet::ProvisioningListEntry& )>> userDefinedCallback;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::ENTRY_NOT_FOUND,
              controller.findProvisioningListEntry( userDefinedCallback.AsStdFunction() ) );
}

TEST_F( ProvisioningListEnumerate, ShouldReturnError_WhenStorageRejectsReadOperations ) {
    // Arrange

    MockProvisioningListProvider provider;

    ON_CALL( provider, loadProvisioningList( _ ) ).WillByDefault( Return( false ) );

    // Act & assert

    // Explicitly deny any calls to observer
    StrictMock<MockProvisioningListObserver> observer;

    // A client-side callback that is to be called by the controller for each entry, not expecting any calls
    StrictMock<MockFunction<bool( const ::znet::ProvisioningListEntry& )>> userDefinedCallback;

    ::znet::api::ProvisioningListController controller{ m_classicInclusionDataSource, provider, observer };

    ASSERT_EQ(::znet::api::ProvisioningListController::OperationStatus::STORAGE_NOT_AVAILABLE,
              controller.findProvisioningListEntry( userDefinedCallback.AsStdFunction() ) );
}

}  // namespace ut
}  // namespace znet
