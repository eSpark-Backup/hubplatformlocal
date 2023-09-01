#pragma once

#include "znet_provisioning_list_entry_generator.hpp"
#include "znet_mock_provisioning_list_provider.hpp"
#include "znet_mock_provisioning_list_data_source.hpp"

#include "znet/iznet_provisioning_list_observer.hpp"
#include "znet/iznet_provisioning_list_provider.hpp"

namespace znet {
namespace ut {

class ProvisioningListEntryTest : public ::testing::Test, public ProvisioningListEntryGenerator {
protected:
    using operation_status_t = ::znet::api::IProvisioningListController::OperationStatus;

    void SetUp() override {
        using ::testing::_;
        using ::testing::Return;

        m_entry = makeEntry( makeProvisioningID(), makeDSK() );

        // The default behaviour is an empty data provider.
        ON_CALL( m_provider, findProvisioningListEntryByDSK( _, _ ) ).WillByDefault( Return( false ) );
        ON_CALL( m_provider, findProvisioningListEntryByID( _, _ ) ).WillByDefault( Return( false ) );
        ON_CALL( m_provider, removeProvisioningListEntryByID( _ ) ).WillByDefault( Return( false ) );

        ON_CALL( m_classicInclusionDataSource, iterate( _ ) )
            .WillByDefault( Return( operation_status_t::ENTRY_NOT_FOUND ) );
        ON_CALL( m_classicInclusionDataSource, update( _ ) )
            .WillByDefault( Return( operation_status_t::STORAGE_NOT_AVAILABLE ) );
        ON_CALL( m_classicInclusionDataSource, remove( _ ) )
            .WillByDefault( Return( operation_status_t::ENTRY_NOT_FOUND ) );
    }

    ::znet::ProvisioningListEntry m_entry;
    MockProvisioningListProvider m_provider;
    MockProvisioningListQueryDataSource m_classicInclusionDataSource;
};

class ProvisioningListEntryAdd : public ProvisioningListEntryTest {};
class ProvisioningListEntrySet : public ProvisioningListEntryTest {};
class ProvisioningListEntryRemove : public ProvisioningListEntryTest {};

class ProvisioningListEnumerate : public ::testing::Test, public ProvisioningListEntryGenerator {
protected:
    void SetUp() override {
        m_entries = makeEntryList();
    }

    std::vector<::znet::ProvisioningListEntry> m_entries;
    MockProvisioningListQueryDataSource m_classicInclusionDataSource;
};

}  // namespace ut
}  // namespace znet
