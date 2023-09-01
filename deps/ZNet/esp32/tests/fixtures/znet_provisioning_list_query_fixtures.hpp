#pragma once

#include <gtest/gtest.h>

#include "znet_provisioning_list_entry_generator.hpp"
#include "znet_mock_provisioning_list_data_source.hpp"

namespace znet {
namespace ut {

class ProvisioningListQueryTest : public ::testing::Test, public ProvisioningListEntryGenerator {
protected:
    static std::unique_ptr<MockProvisioningListQueryDataSource> makeDataSourceWithOneEntry(
        const ::znet::ProvisioningListEntry& entry ) {
        using controller_t = ::znet::api::IProvisioningListController;
        std::unique_ptr<MockProvisioningListQueryDataSource> dataSource( new MockProvisioningListQueryDataSource );
        ON_CALL( *dataSource, iterate(::testing::_ ) )
            .WillByDefault( [entry]( const controller_t::ProcessProvisioningCallback& cb ) {
                return cb( entry ) ? controller_t::OperationStatus::SUCCESS :
                                     controller_t::OperationStatus::ENTRY_NOT_FOUND;
            } );
        return dataSource;
    }
};

class ProvisioningListQueryFilterFixture : public ProvisioningListQueryTest {
protected:
    void SetUp() override {
        m_entryToBeFiltered = makeEntry( makeProvisioningID(), makeDSK() );
    }

    ::znet::ProvisioningListEntry m_entryToBeFiltered;
};

class ProvisioningListModifyingQueryFixture : public ProvisioningListQueryTest {
protected:
    void SetUp() override {
        m_entryToBeModified = makeEntry( makeProvisioningID(), makeDSK() );
    }

    ::znet::ProvisioningListEntry m_entryToBeModified;
};

}  // namespace ut
}  // namespace znet
