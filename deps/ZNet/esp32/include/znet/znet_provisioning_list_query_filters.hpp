/**
 * The file contains filters for ::znet::api::IProvisioningListQuery::filterBy method.
 */

#pragma once

#include <unordered_set>
#include <functional>

extern "C" {
#include <byte_ops.h>
}

#include <znet/znet_provisioning_list_entry.hpp>

namespace znet {
namespace api {
namespace filters {

struct Filter {
    bool operator()( const ::znet::ProvisioningListEntry& entry ) const {
        return m_clause( entry );
    }

    std::function<bool( const ::znet::ProvisioningListEntry& )> m_clause;
};

inline Filter operator||( const Filter& lhs, const Filter& rhs ) {
    return Filter{ [lhs, rhs]( const ::znet::ProvisioningListEntry& entry ) { return lhs( entry ) || rhs( entry ); } };
}

inline Filter operator&&( const Filter& lhs, const Filter& rhs ) {
    return Filter{ [lhs, rhs]( const ::znet::ProvisioningListEntry& entry ) { return lhs( entry ) && rhs( entry ); } };
}

inline Filter operator!( const Filter& filter ) {
    return Filter{ [filter]( const ::znet::ProvisioningListEntry& entry ) { return !filter( entry ); } };
}

namespace fields {

inline Filter nodeId( uint8_t value ) {
    return Filter{ [value]( const ::znet::ProvisioningListEntry& entry ) {
        return entry.m_fields.m_node && *entry.m_fields.m_node == value;
    } };
}

inline Filter nodeId( const std::unordered_set<uint8_t>& valueList ) {
    return Filter{ [valueList]( const ::znet::ProvisioningListEntry& entry ) {
        return entry.m_fields.m_node && valueList.find( *entry.m_fields.m_node ) != valueList.end();
    } };
}

inline Filter provisioningId( const std::unordered_set<uint32_t>& valueList ) {
    return Filter{ [valueList]( const ::znet::ProvisioningListEntry& entry ) {
        return valueList.find( entry.m_provisioningId ) != valueList.end();
    } };
}

inline Filter nhid( const std::array<uint8_t, 4>& nhid ) {
    return Filter{ [nhid]( const ::znet::ProvisioningListEntry& entry ) {
        return set_last_2_bits( entry.m_dsk[4] ) == bytes_to_int16( &nhid[0] ) &&
               reset_first_bit( entry.m_dsk[5] ) == bytes_to_int16( &nhid[2] );
    } };
}

inline Filter addingMode(::znet::ProvisioningListEntryFields::AddingMode mode ) {
    return Filter{ [mode]( const ::znet::ProvisioningListEntry& entry ) {
        return entry.m_fields.m_addingMode && *entry.m_fields.m_addingMode == mode;
    } };
}

inline Filter status(::znet::ProvisioningListEntryFields::Status status ) {
    return Filter{ [status]( const ::znet::ProvisioningListEntry& entry ) {
        return entry.m_fields.m_status && *entry.m_fields.m_status == status;
    } };
}

inline Filter dsk( const DSK& dsk ) {
    return Filter{ [dsk]( const ::znet::ProvisioningListEntry& entry ) { return entry.m_dsk == dsk; } };
}

}  // namespace fields
}  // namespace filters
}  // namespace api
}  // namespace znet
