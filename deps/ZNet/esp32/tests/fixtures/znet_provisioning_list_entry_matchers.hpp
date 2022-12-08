#pragma once

#include <gmock/gmock.h>

#include "znet_provisioning_list_query_printers.hpp"

namespace znet {
namespace ut {

using ::testing::ElementsAreArray;
using ::testing::Field;
using ::testing::Matcher;

MATCHER( IsNullopt, "is an empty optional" ) {
    return !static_cast<bool>( arg );
}

inline Matcher<const utils::optional<::znet::ProvisioningListEntryFields::Product>&> MatchProduct(
    const utils::optional<::znet::ProvisioningListEntryFields::Product>& expected ) {
    using product_t = ::znet::ProvisioningListEntryFields::Product;
    if( expected ) {
        return Pointee( AllOf( Field( "manufacturer_id", &product_t::m_manufacturerId, expected->m_manufacturerId ),
                               Field( "product_type", &product_t::m_productType, expected->m_productType ),
                               Field( "product_id", &product_t::m_productId, expected->m_productId ),
                               Field( "subversion", &product_t::m_subversion, expected->m_subversion ),
                               Field( "version", &product_t::m_version, expected->m_version ) ) );
    }
    else {
        return IsNullopt();
    }
}

inline Matcher<const utils::optional<::znet::ProvisioningListEntryFields::Type>&> MatchType(
    const utils::optional<::znet::ProvisioningListEntryFields::Type>& expected ) {
    using type_t = ::znet::ProvisioningListEntryFields::Type;
    if( expected ) {
        return Pointee( AllOf( Field( "generic", &type_t::m_generic, expected->m_generic ),
                               Field( "specific", &type_t::m_specific, expected->m_specific ),
                               Field( "icon", &type_t::m_icon, expected->m_icon ) ) );
    }
    else {
        return IsNullopt();
    }
}

inline Matcher<const ::znet::ProvisioningListEntryFields&> MatchFields(
    const ::znet::ProvisioningListEntryFields& expected ) {
    using fields_t = ::znet::ProvisioningListEntryFields;
    return AllOf( Field( "node", &fields_t::m_node, expected.m_node ),
                  Field( "status", &fields_t::m_status, expected.m_status ),
                  Field( "adding_mode", &fields_t::m_addingMode, expected.m_addingMode ),
                  Field( "name", &fields_t::m_name, expected.m_name ),
                  Field( "room", &fields_t::m_room, expected.m_room ),
                  Field( "uuid", &fields_t::m_uuid, expected.m_uuid ),
                  Field( "max_request_interval", &fields_t::m_maxRequestInterval, expected.m_maxRequestInterval ),
                  Field( "type", &fields_t::m_type, MatchType( expected.m_type ) ),
                  Field( "product", &fields_t::m_product, MatchProduct( expected.m_product ) ),
                  Field( "security_modes", &fields_t::m_securityModes, expected.m_securityModes ),
                  Field( "inclusion_setting", &fields_t::m_inclusionSetting, expected.m_inclusionSetting ) );
}

inline Matcher<const ::znet::ProvisioningListEntry&> MatchFullEntryExceptID(
    const ::znet::ProvisioningListEntry& expected ) {
    using entry_t = ::znet::ProvisioningListEntry;
    return AllOf( Field( "dsk", &entry_t::m_dsk, expected.m_dsk ),
                  Field( "fields", &entry_t::m_fields, MatchFields( expected.m_fields ) ) );
}

inline Matcher<::znet::ProvisioningListEntry&> MatchRefEntry( const ::znet::ProvisioningListEntry& expected ) {
    using entry_t = ::znet::ProvisioningListEntry;
    auto dsk = new uint16_t[expected.m_dsk.size()];
    std::copy( expected.m_dsk.begin(), expected.m_dsk.end(), dsk );
    return AllOf( Field( "id", &entry_t::m_provisioningId, expected.m_provisioningId ),
                  Field( "dsk", &entry_t::m_dsk, ElementsAreArray( dsk, expected.m_dsk.size() ) ),
                  Field( "fields", &entry_t::m_fields, MatchFields( expected.m_fields ) ) );
}

inline Matcher<const ::znet::ProvisioningListEntry&> MatchEntry( const ::znet::ProvisioningListEntry& expected ) {
    using entry_t = ::znet::ProvisioningListEntry;
    auto dsk = new uint16_t[expected.m_dsk.size()];
    std::copy( expected.m_dsk.begin(), expected.m_dsk.end(), dsk );
    return AllOf( Field( "id", &entry_t::m_provisioningId, expected.m_provisioningId ),
                  Field( "dsk", &entry_t::m_dsk, ElementsAreArray( dsk, expected.m_dsk.size() ) ),
                  Field( "fields", &entry_t::m_fields, MatchFields( expected.m_fields ) ) );
}

}  // namespace ut
}  // namespace znet
