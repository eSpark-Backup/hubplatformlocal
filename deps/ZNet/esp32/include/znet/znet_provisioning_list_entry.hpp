#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <bitset>
#include <string>
#include <array>

#include <znet/optional.hpp>

namespace znet {

using DSK = std::array<uint16_t, 8>;

struct ProvisioningListEntryFields {
    enum class Status { NOT_DETECTED, ADDED, OTHER_NETWORK, FAILED };
    enum class AddingMode { S2, SMART, UNDEFINED };
    enum class InclusionSetting { PENDING = 0x00, PASSIVE = 0x02, IGNORED = 0x03 };

    struct Product {
        uint16_t m_manufacturerId;
        uint16_t m_productType;
        uint16_t m_productId;
        uint16_t m_version;
        uint16_t m_subversion;
    };

    struct Type {
        uint8_t m_generic;
        uint8_t m_specific;
        uint16_t m_icon;
    };

    struct UuidInformation {
        uint8_t m_presentationFormat;
        std::array<uint8_t, 16> m_uuid;
    };

    utils::optional<uint8_t> m_node;
    utils::optional<Status> m_status;
    utils::optional<AddingMode> m_addingMode;
    std::vector<AddingMode> m_supportedAddingModes;
    utils::optional<InclusionSetting> m_inclusionSetting;
    utils::optional<Product> m_product;
    utils::optional<Type> m_type;
    utils::optional<int> m_maxRequestInterval;
    utils::optional<UuidInformation> m_uuidInformation;
    utils::optional<std::string> m_uuid;
    utils::optional<std::string> m_name;
    utils::optional<std::string> m_room;
    utils::optional<std::string> m_location;
    /**
     * A bitset of security modes selected by a user:
     *      - ZNET_SECURITY2_S0_NET_KEY
     *      - ZNET_SECURITY2_S2_CLASS_0
     *      - ZNET_SECURITY2_S2_CLASS_1
     *      - ZNET_SECURITY2_S2_CLASS_2
     */
    utils::optional<uint8_t> m_securityModes;

    /**
     * A bitset of security modes used during bootstrapping:
     *      - ZNET_SECURITY2_S0_NET_KEY
     *      - ZNET_SECURITY2_S2_CLASS_0
     *      - ZNET_SECURITY2_S2_CLASS_1
     *      - ZNET_SECURITY2_S2_CLASS_2
     */
    utils::optional<uint8_t> m_nodeSecurityModes;

    /**
     * A bitset of security modes supported by node:
     *      - ZNET_SECURITY2_S0_NET_KEY
     *      - ZNET_SECURITY2_S2_CLASS_0
     *      - ZNET_SECURITY2_S2_CLASS_1
     *      - ZNET_SECURITY2_S2_CLASS_2
     */
    utils::optional<uint8_t> m_supportedModes;
};

struct ProvisioningListEntry {
    uint32_t m_provisioningId;
    DSK m_dsk;
    ProvisioningListEntryFields m_fields;
};

inline bool operator==( const ProvisioningListEntryFields::Product& lhs,
                        const ProvisioningListEntryFields::Product& rhs ) {
    return std::tie( lhs.m_manufacturerId, lhs.m_productType, lhs.m_productId, lhs.m_version, lhs.m_subversion ) ==
           std::tie( rhs.m_manufacturerId, rhs.m_productType, rhs.m_productId, rhs.m_version, rhs.m_subversion );
}

inline bool operator==( const ProvisioningListEntryFields::Type& lhs, const ProvisioningListEntryFields::Type& rhs ) {
    return std::tie( lhs.m_generic, lhs.m_specific, lhs.m_icon ) ==
           std::tie( rhs.m_generic, rhs.m_specific, rhs.m_icon );
}

}  // namespace znet
