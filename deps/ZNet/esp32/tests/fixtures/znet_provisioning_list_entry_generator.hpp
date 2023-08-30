#pragma once

#include <random>
#include <algorithm>

#include "znet/znet_provisioning_list_entry.hpp"

namespace znet {
namespace ut {

class ProvisioningListEntryGenerator {
public:
    static DSK makeDSK() {
        std::random_device rd;
        std::default_random_engine e( rd() );
        DSK result{};
        std::generate( result.begin(), result.end(), e );
        return result;
    }

    static uint32_t makeProvisioningID() {
        std::random_device rd;
        std::default_random_engine e( rd() );
        return e();
    }

    static utils::optional<uint8_t> makeNodeID() {
        std::random_device rd;
        std::default_random_engine e( rd() );
        return utils::optional<uint8_t>{ static_cast<uint8_t>( e() % 4 ) };
    }

    static utils::optional<::znet::ProvisioningListEntryFields::Status> makeStatus() {
        std::random_device rd;
        std::default_random_engine e{ rd() };
        return utils::optional<::znet::ProvisioningListEntryFields::Status>{
            static_cast<::znet::ProvisioningListEntryFields::Status>( e() % 4 )
        };
    }

    static utils::optional<::znet::ProvisioningListEntryFields::AddingMode> makeAddingMode() {
        std::random_device rd;
        std::default_random_engine e{ rd() };
        return utils::optional<::znet::ProvisioningListEntryFields::AddingMode>{
            static_cast<::znet::ProvisioningListEntryFields::AddingMode>( e() % 2 )
        };
    }

    static utils::optional<::znet::ProvisioningListEntryFields::InclusionSetting> makeInclusionSetting() {
        std::random_device rd;
        std::default_random_engine e{ rd() };
        return utils::optional<::znet::ProvisioningListEntryFields::InclusionSetting>{
            static_cast<::znet::ProvisioningListEntryFields::InclusionSetting>( e() % 3 )
        };
    }

    static utils::optional<uint8_t> makeSecurityModes() {
        std::random_device rd;
        std::default_random_engine e{ rd() };
        return e();
    }

    static ::znet::ProvisioningListEntryFields makeFields() {
        ::znet::ProvisioningListEntryFields fields{ .m_node = makeNodeID(),
                                                    .m_status = makeStatus(),
                                                    .m_addingMode = makeAddingMode(),
                                                    .m_supportedAddingModes = {},
                                                    .m_inclusionSetting = makeInclusionSetting() };
        fields.m_securityModes = makeSecurityModes();
        return fields;
    }

    static ::znet::ProvisioningListEntryFields makeDefaultEntryFields() {
        ::znet::ProvisioningListEntryFields defaultEntryFields{
            .m_node = 0,
            .m_status = ::znet::ProvisioningListEntryFields::Status::NOT_DETECTED,
            .m_addingMode = ::znet::ProvisioningListEntryFields::AddingMode::SMART,
            .m_supportedAddingModes = { ::znet::ProvisioningListEntryFields::AddingMode::S2,
                                        ::znet::ProvisioningListEntryFields::AddingMode::SMART },
            .m_inclusionSetting = ProvisioningListEntryFields::InclusionSetting::PENDING
        };
        defaultEntryFields.m_securityModes = 0;
        defaultEntryFields.m_supportedModes = 0xFF;
        return defaultEntryFields;
    }

    static ::znet::ProvisioningListEntry makeEntry( uint32_t provisioningId, const DSK& dsk ) {
        return ::znet::ProvisioningListEntry{
            .m_provisioningId = provisioningId, .m_dsk = dsk, .m_fields = makeFields()
        };
    }

    static std::vector<::znet::ProvisioningListEntry> makeEntryList() {
        return { makeEntry( makeProvisioningID(), makeDSK() ), makeEntry( makeProvisioningID(), makeDSK() ) };
    }
};

}  // namespace ut
}  // namespace znet
