#pragma once

#include <ostream>

#include "znet/znet.h"
#include "znet/znet_provisioning_list_entry.hpp"

namespace znet {
inline std::ostream& operator<<( std::ostream& stream, const ::znet::ProvisioningListEntryFields::Type& type ) {
    stream << R"({"generic":")" << type.m_generic << R"(","specific":")" << type.m_specific << R"(","icon":")"
           << type.m_icon << "\"}";
    return stream;
}
inline std::ostream& operator<<( std::ostream& stream, const ::znet::ProvisioningListEntryFields::Product& product ) {
    stream << R"({"product_id":")" << product.m_productId << R"(","product_type":")" << product.m_productType
           << R"(","m_manufacturerId":")" << product.m_manufacturerId << R"(","version":")" << product.m_version
           << R"(","subversion":")" << product.m_subversion << "\"}";
    return stream;
}
inline std::ostream& operator<<( std::ostream& stream, const ::znet::ProvisioningListEntryFields& fields ) {
    stream << R"("room":)";
    if( fields.m_room ) {
        stream << '"' << *fields.m_room << '"';
    }
    else {
        stream << "null";
    }
    stream << R"(,"name":)";
    if( fields.m_name ) {
        stream << '"' << *fields.m_name << '"';
    }
    else {
        stream << "null";
    }
    stream << R"(,"adding_mode":)";
    if( fields.m_addingMode ) {
        stream << '"'
               << ( *fields.m_addingMode == ::znet::ProvisioningListEntryFields::AddingMode::S2 ? "S2\"" : "SMART\"" );
    }
    else {
        stream << "null";
    }
    stream << R"(,"node":)";
    if( fields.m_node ) {
        stream << "\"0x" << std::hex << (int)*fields.m_node << '"';
    }
    else {
        stream << "null";
    }
    stream << R"(,"status":)";
    if( fields.m_status ) {
        switch( *fields.m_status ) {
        case ::znet::ProvisioningListEntryFields::Status::ADDED:
            stream << "\"added\"";
            break;
        case ::znet::ProvisioningListEntryFields::Status::FAILED:
            stream << "\"failed\"";
            break;
        case ::znet::ProvisioningListEntryFields::Status::NOT_DETECTED:
            stream << "\"notDetected\"";
            break;
        case ::znet::ProvisioningListEntryFields::Status::OTHER_NETWORK:
            stream << "\"otherNetwork\"";
            break;
        }
    }
    else {
        stream << "null";
    }
    stream << R"(,"uuid":)";
    if( fields.m_uuid ) {
        stream << '"' << *fields.m_uuid << '"';
    }
    else {
        stream << "null";
    }
    stream << R"(,"max_request_interval":)";
    if( fields.m_maxRequestInterval ) {
        stream << '"' << *fields.m_maxRequestInterval << '"';
    }
    else {
        stream << "null";
    }
    stream << R"(,"product":)";
    if( fields.m_product ) {
        stream << *fields.m_product;
    }
    else {
        stream << "null";
    }
    stream << R"(,"type":)";
    if( fields.m_type ) {
        stream << *fields.m_type;
    }
    else {
        stream << "null";
    }
    stream << R"(,"security_modes":)";
    if( fields.m_securityModes ) {
        stream << '[';
        if( *fields.m_securityModes & ZNET_SECURITY2_S2_CLASS_2 ) {
            stream << "accessControl,";
        }
        if( *fields.m_securityModes & ZNET_SECURITY2_S2_CLASS_1 ) {
            stream << "authenticated,";
        }
        if( *fields.m_securityModes & ZNET_SECURITY2_S2_CLASS_0 ) {
            stream << "unauthenticated,";
        }
        if( *fields.m_securityModes & ZNET_SECURITY2_S0_NET_KEY ) {
            stream << "s0,";
        }
        stream << ']';
    }
    else {
        stream << "null";
    }
    return stream;
}
inline std::ostream& operator<<( std::ostream& stream, const ::znet::ProvisioningListEntry& entry ) {
    stream << R"({"id":")" << entry.m_provisioningId << R"(","dsk":[)";
    for( auto key : entry.m_dsk ) {
        stream << key << ',';
    }
    stream << "]," << entry.m_fields << "}";
    return stream;
}
}  // namespace znet
