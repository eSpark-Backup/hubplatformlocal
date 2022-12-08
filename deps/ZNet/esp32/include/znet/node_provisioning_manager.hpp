#pragma once

#include "znet/znet.h"
#include "znet/znet_provisioning_list_entry.hpp"

#include <queue>

namespace znet {
namespace api {
class IProvisioningListController;
}

class NodeProvisioningManager {
public:
    NodeProvisioningManager( api::IProvisioningListController& controller );

    void onNodeProvisioningSet( znet_node_id_t nodeId,
                                znet_node_channel_id_t channelId,
                                const znet_node_provisioning_set_t* value );

    void onNodeProvisioningDelete( znet_node_id_t nodeId,
                                   znet_node_channel_id_t channelId,
                                   const znet_node_provisioning_delete_t* value );

    void onNodeProvisioningGet( znet_node_id_t nodeId,
                                znet_node_channel_id_t channelId,
                                uint8_t sequenceNumber,
                                const uint8_t dskKey[16] );

    void onNodeProvisioningListIterationGet( znet_node_id_t nodeId,
                                             znet_node_channel_id_t channelId,
                                             uint8_t sequenceNumber,
                                             uint8_t remainingCounter );

private:
    enum class InformationBlockType {
        ProductType = 0x00,
        ProductId = 0x01,
        MaxInclusionRequestInterval = 0x02,
        UUID16 = 0x03,
        Name = 0x32,
        Location = 0x33,
        SmartStartInclusionSetting = 0x34,
        AdvancedJoining = 0x35,
        BootstrappingMode = 0x36,
        NetworkStatus = 0x37
    };

    DSK toDsk( const uint8_t dskKey[16] ) const;
    void fromDsk( const DSK& source, uint8_t target[16] ) const;

    void fillProvisioningEntryFields( ProvisioningListEntryFields& fields,
                                      const uint8_t* metaData,
                                      uint8_t metaDataLength ) const;

    void processInformationBlock( ProvisioningListEntryFields& fields,
                                  InformationBlockType type,
                                  const uint8_t* metaData,
                                  uint8_t length ) const;

    void startNewQuery();
    void reportNextEntry( znet_node_id_t nodeId, znet_node_channel_id_t channelId, uint8_t sequenceNumber );

    void generateEntryReport( znet_node_id_t nodeId,
                              znet_node_channel_id_t channelId,
                              uint8_t sequenceNumber,
                              const ProvisioningListEntry& entry ) const;

    void generateEmtyEntryReport( znet_node_id_t nodeId,
                                  znet_node_channel_id_t channelId,
                                  uint8_t sequenceNumber ) const;

    void generateListIterationEntryReport( znet_node_id_t nodeId,
                                           znet_node_channel_id_t channelId,
                                           uint8_t sequenceNumber,
                                           const ProvisioningListEntry& entry ) const;

    void generateEmptyListIterationEntryReport( znet_node_id_t nodeId,
                                                znet_node_channel_id_t channelId,
                                                uint8_t sequenceNumber ) const;

    void extractMetaData( const ProvisioningListEntryFields& fields, std::vector<uint8_t>& result ) const;
    uint8_t concatenateMetaDataType( InformationBlockType type, bool critical ) const;

    template <InformationBlockType BlockType, typename ValueType>
    void tryExtractMetaDataBlock( utils::optional<ValueType> value, std::vector<uint8_t>& result ) const;

    template <InformationBlockType BlockType, typename ValueType>
    void extractMetaDataBlock( const ValueType& value, std::vector<uint8_t>& result ) const;

    std::string generateUuidString( const ProvisioningListEntryFields::UuidInformation& info ) const;
    std::string generateEntryName( const ProvisioningListEntryFields& fields ) const;

    uint16_t unit( uint8_t lowByte, uint8_t high_byte ) const;
    std::pair<uint8_t, uint8_t> split( uint16_t value ) const;

private:
    api::IProvisioningListController& m_controller;

    std::queue<uint32_t> m_iterationList;
};

}  // namespace znet
