#include "znet/znet_provisioning_list_query.hpp"

#include "znet_provisioning_list_entry_matchers.hpp"
#include "znet_provisioning_list_query_fixtures.hpp"

namespace znet {
namespace ut {

using ::testing::_;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::SaveArg;

using ListOfNodeIds = std::unordered_set<uint8_t>;
using NodeIdField = utils::optional<uint8_t>;

class ProvisioningListQueryFilter_byNodeId
: public ProvisioningListQueryFilterFixture,
  public ::testing::WithParamInterface<std::tuple<NodeIdField, uint8_t, bool>> {};

TEST_P( ProvisioningListQueryFilter_byNodeId, Test ) {
    // Arrange

    m_entryToBeFiltered.m_fields.m_node = std::get<0>( GetParam() );

    // Act

    auto filter = ::znet::api::filters::fields::nodeId( std::get<1>( GetParam() ) );
    bool filterResult = filter.m_clause( m_entryToBeFiltered );

    // Assert

    ASSERT_EQ( filterResult, std::get<2>( GetParam() ) );
}

INSTANTIATE_TEST_SUITE_P( ProvisioningListQueryFilter_byNodeId_parametrized,
                          ProvisioningListQueryFilter_byNodeId,
                          ::testing::Values( std::make_tuple( NodeIdField{ 11 }, 11, true ),
                                             std::make_tuple( NodeIdField{ 11 }, 99, false ),
                                             std::make_tuple( NodeIdField{}, 99, false ) ) );

class ProvisioningListQueryFilter_byNodeIdList
: public ProvisioningListQueryFilterFixture,
  public ::testing::WithParamInterface<std::tuple<NodeIdField, ListOfNodeIds, bool>> {};

TEST_P( ProvisioningListQueryFilter_byNodeIdList, Test ) {
    // Arrange

    m_entryToBeFiltered.m_fields.m_node = std::get<0>( GetParam() );

    // Act

    auto filter = ::znet::api::filters::fields::nodeId( std::get<1>( GetParam() ) );
    bool filterResult = filter.m_clause( m_entryToBeFiltered );

    // Assert

    ASSERT_EQ( filterResult, std::get<2>( GetParam() ) );
}

INSTANTIATE_TEST_SUITE_P( ProvisioningListQueryFilter_byNodeIdList_parametrized,
                          ProvisioningListQueryFilter_byNodeIdList,
                          ::testing::Values( std::make_tuple( NodeIdField{ 11 }, ListOfNodeIds{ 9, 10, 11 }, true ),
                                             std::make_tuple( NodeIdField{ 11 }, ListOfNodeIds{ 5, 6, 7 }, false ),
                                             std::make_tuple( NodeIdField{ 11 }, ListOfNodeIds{}, false ),
                                             std::make_tuple( NodeIdField{}, ListOfNodeIds{}, false ),
                                             std::make_tuple( NodeIdField{}, ListOfNodeIds{ 99 }, false ) ) );

using ListOfProvisioningIds = std::unordered_set<uint32_t>;

class ProvisioningListQueryFilter_byProvisioningIdList
: public ProvisioningListQueryFilterFixture,
  public ::testing::WithParamInterface<std::tuple<uint32_t, ListOfProvisioningIds, bool>> {};

TEST_P( ProvisioningListQueryFilter_byProvisioningIdList, Test ) {
    // Arrange

    m_entryToBeFiltered.m_provisioningId = std::get<0>( GetParam() );

    // Act

    auto filter = ::znet::api::filters::fields::provisioningId( std::get<1>( GetParam() ) );
    bool filterResult = filter.m_clause( m_entryToBeFiltered );

    // Assert

    ASSERT_EQ( filterResult, std::get<2>( GetParam() ) );
}

INSTANTIATE_TEST_SUITE_P( ProvisioningListQueryFilter_byProvisioningIdList_parametrized,
                          ProvisioningListQueryFilter_byProvisioningIdList,
                          ::testing::Values( std::make_tuple( 11, ListOfProvisioningIds{ 9, 10, 11 }, true ),
                                             std::make_tuple( 11, ListOfProvisioningIds{ 5, 6, 7 }, false ),
                                             std::make_tuple( 11, ListOfProvisioningIds{}, false ) ) );

using NHID = std::array<uint8_t, 4>;

class ProvisioningListQueryFilter_byNhid : public ProvisioningListQueryFilterFixture,
                                           public ::testing::WithParamInterface<std::tuple<::znet::DSK, NHID, bool>> {};

TEST_P( ProvisioningListQueryFilter_byNhid, Test ) {
    // Arrange

    m_entryToBeFiltered.m_dsk = std::get<0>( GetParam() );

    // Act

    auto filter = ::znet::api::filters::fields::nhid( std::get<1>( GetParam() ) );
    bool filterResult = filter.m_clause( m_entryToBeFiltered );

    // Assert

    ASSERT_EQ( filterResult, std::get<2>( GetParam() ) );
}

INSTANTIATE_TEST_SUITE_P(
    ProvisioningListQueryFilter_byNhid_parametrized,
    ProvisioningListQueryFilter_byNhid,
    ::testing::Values( std::make_tuple(::znet::DSK{ 1, 2, 3, 4, 5, 6, 7, 8 }, NHID{ 0xC0, 0x5, 0, 0x6 }, true ),
                       std::make_tuple(::znet::DSK{ 1, 2, 3, 4, 5, 6, 7, 8 }, NHID{ 0xC0, 0x5, 0, 0 }, false ),
                       std::make_tuple(::znet::DSK{ 1, 2, 3, 4, 5, 6, 7, 8 }, NHID{ 0, 0x5, 0, 0x6 }, false ) ) );

class ProvisioningListQuery_getDskDerivedHomeId : public ProvisioningListQueryFilterFixture {};

TEST_F( ProvisioningListQuery_getDskDerivedHomeId, ShouldProvideValue_WhenProcessingAnyEntry ) {
    // Arrange

    m_entryToBeFiltered.m_dsk = { 1, 2, 3, 4, 5, 6, 7, 8 };

    auto dataSource = makeDataSourceWithOneEntry( m_entryToBeFiltered );

    // Act

    DskDerivedHomeId receivedDskDerivedHomeId;
    ::znet::api::ProvisioningListReadQuery{ *dataSource }
        .getDskDerivedHomeId( receivedDskDerivedHomeId )
        .execute();  // Ignore result, only need to verify filter clause

    // Assert

    ASSERT_EQ( receivedDskDerivedHomeId, ( DskDerivedHomeId{ 0xC0, 0x5, 0, 0x6, 0xC0, 0x7, 0, 0x8 } ) );
}

class ProvisioningListQuery_getPubKey : public ProvisioningListQueryFilterFixture {};

TEST_F( ProvisioningListQuery_getPubKey, ShouldProvideValue_WhenProcessingAnyEntry ) {
    // Arrange

    m_entryToBeFiltered.m_dsk = { 1, 2, 3, 4, 5, 6, 7, 8 };

    auto dataSource = makeDataSourceWithOneEntry( m_entryToBeFiltered );

    // Act

    using PubKey = std::array<uint8_t, 2>;
    PubKey receivedPubKey;
    ::znet::api::ProvisioningListReadQuery{ *dataSource }
        .getPubKey( receivedPubKey )
        .execute();  // Ignore result, only need to verify filter clause

    // Assert

    ASSERT_EQ( receivedPubKey, ( PubKey{ 0, 0x1 } ) );
}

class ProvisioningListQuery_getSecurityModes : public ProvisioningListQueryFilterFixture {};

TEST_F( ProvisioningListQuery_getSecurityModes, ShouldProvideFieldValue_WhenSuchFieldIsSet ) {
    // Arrange

    NodeIdField actualValue = 99;
    m_entryToBeFiltered.m_fields.m_securityModes = actualValue;

    auto dataSource = makeDataSourceWithOneEntry( m_entryToBeFiltered );

    // Act

    NodeIdField receivedValue;
    ::znet::api::ProvisioningListReadQuery{ *dataSource }
        .getField( &::znet::ProvisioningListEntryFields::m_securityModes, receivedValue )
        .execute();  // Ignore result, only need to verify filter clause

    // Assert

    ASSERT_EQ( receivedValue.has_value(), actualValue.has_value() );
    ASSERT_EQ( *receivedValue, *actualValue );
}

TEST_F( ProvisioningListQuery_getSecurityModes, ShouldProvideDefaultValue_WhenTargetFieldIsNotSet ) {
    // Arrange

    // Empty value
    NodeIdField actualValue;
    m_entryToBeFiltered.m_fields.m_securityModes = actualValue;

    uint8_t defaultValue = 99;

    auto dataSource = makeDataSourceWithOneEntry( m_entryToBeFiltered );

    // Act

    uint8_t receivedValue = 0;
    ::znet::api::ProvisioningListReadQuery{ *dataSource }
        .getField( &::znet::ProvisioningListEntryFields::m_securityModes, receivedValue, defaultValue )
        .execute();  // Ignore result, only need to verify filter clause

    // Assert

    ASSERT_EQ( receivedValue, defaultValue );
}

class ProvisioningListQuery_setStatus : public ProvisioningListModifyingQueryFixture {};

TEST_F( ProvisioningListQuery_setStatus, ShouldChangeFieldValue_WhenProcessingAnyEntry ) {
    // Arrange

    m_entryToBeModified.m_fields.m_status = ::znet::ProvisioningListEntryFields::Status::NOT_DETECTED;

    auto expectedModifiedEntry = m_entryToBeModified;
    expectedModifiedEntry.m_fields.m_status = ::znet::ProvisioningListEntryFields::Status::ADDED;

    auto dataSource = makeDataSourceWithOneEntry( m_entryToBeModified );
    ::znet::ProvisioningListEntry actualModifiedEntry;
    ON_CALL( *dataSource, update( _ ) ).WillByDefault( SaveArg<0>( &actualModifiedEntry ) );

    // Simulate match to proceed to the modifying clause
    MockFunction<bool( const ::znet::ProvisioningListEntry& )> userFilter;
    ON_CALL( userFilter, Call( MatchEntry( m_entryToBeModified ) ) ).WillByDefault( Return( true ) );

    // Act

    ::znet::api::ProvisioningListReadQuery{ *dataSource }
        .tap( userFilter.AsStdFunction() )
        .setField( &::znet::ProvisioningListEntryFields::m_status, *expectedModifiedEntry.m_fields.m_status )
        .execute();  // Ignore result, only need to verify filter clause

    // Assert

    ASSERT_THAT( actualModifiedEntry, MatchEntry( expectedModifiedEntry ) );
}

}  // namespace ut
}  // namespace znet
