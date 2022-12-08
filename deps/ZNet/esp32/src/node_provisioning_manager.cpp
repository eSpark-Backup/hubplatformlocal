#include "znet/node_provisioning_manager.hpp"

#include "znet/iznet_provisioning_list_controller.hpp"
#include "znet/converters/manufacturer_names_converter.hpp"

namespace znet {

NodeProvisioningManager::NodeProvisioningManager( api::IProvisioningListController& controller )
: m_controller( controller ) {}

void NodeProvisioningManager::onNodeProvisioningSet( znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     const znet_node_provisioning_set_t* value ) {
    auto dsk = toDsk( value->dsk_key );
    ProvisioningListEntryFields fields;
    fillProvisioningEntryFields( fields, value->meta_data, value->meta_data_length );

    m_controller.addProvisioningListEntry( dsk, fields );
}

void NodeProvisioningManager::onNodeProvisioningDelete( znet_node_id_t nodeId,
                                                        znet_node_channel_id_t channelId,
                                                        const znet_node_provisioning_delete_t* value ) {
    std::vector<uint32_t> idsToDelete;
    utils::optional<DSK> specifiedDsk;
    if( !value->delete_all ) {
        specifiedDsk = toDsk( value->dsk_key );
    }

    m_controller.findProvisioningListEntry(
        [&specifiedDsk, &idsToDelete]( const ProvisioningListEntry& entry ) -> bool {
            if( !specifiedDsk.has_value() || *specifiedDsk == entry.m_dsk ) {
                idsToDelete.push_back( entry.m_provisioningId );
            }

            return specifiedDsk.has_value();
        } );

    for( const auto& id : idsToDelete ) {
        m_controller.removeProvisioningListEntry( id );
    }
}

void NodeProvisioningManager::onNodeProvisioningGet( znet_node_id_t nodeId,
                                                     znet_node_channel_id_t channelId,
                                                     uint8_t sequenceNumber,
                                                     const uint8_t dskKey[16] ) {
    auto dsk = toDsk( dskKey );
    auto result = m_controller.findProvisioningListEntry(
        [this, &dsk, nodeId, channelId, sequenceNumber]( const ProvisioningListEntry& entry ) -> bool {
            if( dsk == entry.m_dsk ) {
                generateEntryReport( nodeId, channelId, sequenceNumber, entry );
                return true;
            }

            return false;
        } );

    if( result != api::IProvisioningListController::OperationStatus::SUCCESS ) {
        generateEmtyEntryReport( nodeId, channelId, sequenceNumber );
    }
}

void NodeProvisioningManager::onNodeProvisioningListIterationGet( znet_node_id_t nodeId,
                                                                  znet_node_channel_id_t channelId,
                                                                  uint8_t sequenceNumber,
                                                                  uint8_t remainingCounter ) {
    if( remainingCounter == 0xff ) {
        startNewQuery();
        reportNextEntry( nodeId, channelId, sequenceNumber );
    }
    else if( remainingCounter == m_iterationList.size() ) {
        reportNextEntry( nodeId, channelId, sequenceNumber );
    }
}

DSK NodeProvisioningManager::toDsk( const uint8_t dskKey[16] ) const {
    DSK result;
    for( int i = 0; i < 8; ++i ) {
        result[i] = unit( dskKey[i * 2], dskKey[( i * 2 ) + 1] );
    }

    return result;
}

void NodeProvisioningManager::fromDsk( const DSK& source, uint8_t target[16] ) const {
    for( int i = 0; i < 8; ++i ) {
        auto tmp = split( source[i] );
        target[i * 2] = tmp.first;
        target[i * 2 + 1] = tmp.second;
    }
}

void NodeProvisioningManager::fillProvisioningEntryFields( ProvisioningListEntryFields& fields,
                                                           const uint8_t* metaData,
                                                           uint8_t metaDataLength ) const {
    uint8_t counter = 0;
    while( counter < metaDataLength ) {
        auto type = static_cast<InformationBlockType>( metaData[counter++] >> 1 );
        uint8_t length = metaData[counter++];
        assert( counter + length <= metaDataLength );
        processInformationBlock( fields, type, metaData + counter, length );
        counter += length;
    }

    // Fill default information
    fields.m_addingMode = ProvisioningListEntryFields::AddingMode::SMART;
    fields.m_supportedAddingModes = { ProvisioningListEntryFields::AddingMode::SMART,
                                      ProvisioningListEntryFields::AddingMode::S2 };
    fields.m_inclusionSetting = ProvisioningListEntryFields::InclusionSetting::PENDING;
    if( !fields.m_name ) {
        fields.m_name = generateEntryName( fields );
    }
}

void NodeProvisioningManager::processInformationBlock( ProvisioningListEntryFields& fields,
                                                       InformationBlockType type,
                                                       const uint8_t* metaData,
                                                       uint8_t length ) const {
    switch( type ) {
    case InformationBlockType::ProductType: {
        ProvisioningListEntryFields::Type productType;
        productType.m_generic = metaData[0];
        productType.m_specific = metaData[1];
        productType.m_icon = unit( metaData[2], metaData[3] );
        fields.m_type = std::move( productType );
    } break;

    case InformationBlockType::ProductId: {
        ProvisioningListEntryFields::Product product;
        product.m_manufacturerId = unit( metaData[0], metaData[1] );
        product.m_productType = unit( metaData[2], metaData[3] );
        product.m_productId = unit( metaData[4], metaData[5] );
        product.m_version = metaData[6];
        product.m_subversion = metaData[7];
        fields.m_product = std::move( product );
    } break;

    case InformationBlockType::MaxInclusionRequestInterval: {
        fields.m_maxRequestInterval = metaData[0] * 128;
    } break;

    case InformationBlockType::UUID16: {
        ProvisioningListEntryFields::UuidInformation information;
        information.m_presentationFormat = metaData[0] > 6 ? 0 : metaData[0];

        for( int i = 0; i < 16; ++i ) {
            information.m_uuid[i] = metaData[i + 1];
        }
        fields.m_uuidInformation = std::move( information );
        fields.m_uuid = generateUuidString( *fields.m_uuidInformation );
    } break;

    case InformationBlockType::Name: {
        std::string name;
        for( int i = 0; i < length; ++i ) {
            name.push_back( metaData[i] );
        }
        fields.m_name = std::move( name );
    } break;

    case InformationBlockType::Location: {
        std::string location;
        for( int i = 0; i < length; ++i ) {
            location.push_back( metaData[i] );
        }
        fields.m_location = std::move( location );
    } break;

    case InformationBlockType::AdvancedJoining: {
        fields.m_securityModes = metaData[0];
    } break;
    default:
        break;
    }
}

void NodeProvisioningManager::startNewQuery() {
    std::queue<uint32_t> empty;
    m_iterationList.swap( empty );

    m_controller.findProvisioningListEntry( [this]( const ProvisioningListEntry& entry ) -> bool {
        m_iterationList.push( entry.m_provisioningId );
        return false;
    } );
}

void NodeProvisioningManager::reportNextEntry( znet_node_id_t nodeId,
                                               znet_node_channel_id_t channelId,
                                               uint8_t sequenceNumber ) {
    if( !m_iterationList.empty() ) {
        auto currentId = m_iterationList.front();
        m_iterationList.pop();

        auto result = m_controller.findProvisioningListEntry(
            [this, &currentId, nodeId, channelId, sequenceNumber]( const ProvisioningListEntry& entry ) -> bool {
                if( currentId == entry.m_provisioningId ) {
                    generateListIterationEntryReport( nodeId, channelId, sequenceNumber, entry );
                    return true;
                }

                return false;
            } );

        if( result == api::IProvisioningListController::OperationStatus::SUCCESS ) {
            return;
        }
    }

    generateEmptyListIterationEntryReport( nodeId, channelId, sequenceNumber );
}

void NodeProvisioningManager::generateEntryReport( znet_node_id_t nodeId,
                                                   znet_node_channel_id_t channelId,
                                                   uint8_t sequenceNumber,
                                                   const ProvisioningListEntry& entry ) const {
    uint8_t dsk[16];
    fromDsk( entry.m_dsk, dsk );

    std::vector<uint8_t> metaData;
    extractMetaData( entry.m_fields, metaData );

    znet_node_cmd_node_provisioning_report(
        nodeId, channelId, sequenceNumber, dsk, &metaData.front(), metaData.size() );
}

void NodeProvisioningManager::generateEmtyEntryReport( znet_node_id_t nodeId,
                                                       znet_node_channel_id_t channelId,
                                                       uint8_t sequenceNumber ) const {
    znet_node_cmd_node_provisioning_report( nodeId, channelId, sequenceNumber, nullptr, nullptr, 0x00 );
}

void NodeProvisioningManager::generateListIterationEntryReport( znet_node_id_t nodeId,
                                                                znet_node_channel_id_t channelId,
                                                                uint8_t sequenceNumber,
                                                                const ProvisioningListEntry& entry ) const {
    uint8_t dsk[16];
    fromDsk( entry.m_dsk, dsk );

    std::vector<uint8_t> metaData;
    extractMetaData( entry.m_fields, metaData );

    znet_node_cmd_node_provisioning_list_iteration_report(
        nodeId, channelId, sequenceNumber, m_iterationList.size(), dsk, &metaData.front(), metaData.size() );
}

void NodeProvisioningManager::generateEmptyListIterationEntryReport( znet_node_id_t nodeId,
                                                                     znet_node_channel_id_t channelId,
                                                                     uint8_t sequenceNumber ) const {
    znet_node_cmd_node_provisioning_list_iteration_report(
        nodeId, channelId, sequenceNumber, 0x00, nullptr, nullptr, 0x00 );
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<NodeProvisioningManager::InformationBlockType::ProductType,
                                                   ProvisioningListEntryFields::Type>(
    const ProvisioningListEntryFields::Type& value,
    std::vector<uint8_t>& result ) const {
    auto iconPair = split( value.m_icon );

    result.push_back( concatenateMetaDataType( InformationBlockType::ProductType, false ) );
    result.push_back( 0x04 );  // Length
    result.push_back( value.m_generic );
    result.push_back( value.m_specific );
    result.push_back( iconPair.first );
    result.push_back( iconPair.second );
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<NodeProvisioningManager::InformationBlockType::ProductId,
                                                   ProvisioningListEntryFields::Product>(
    const ProvisioningListEntryFields::Product& value,
    std::vector<uint8_t>& result ) const {
    auto manufacturerIdPair = split( value.m_manufacturerId );
    auto productTypePair = split( value.m_productType );
    auto productIdPair = split( value.m_productId );

    result.push_back( concatenateMetaDataType( InformationBlockType::ProductId, false ) );
    result.push_back( 0x08 );  // Length
    result.push_back( manufacturerIdPair.first );
    result.push_back( manufacturerIdPair.second );
    result.push_back( productTypePair.first );
    result.push_back( productTypePair.second );
    result.push_back( productIdPair.first );
    result.push_back( productIdPair.second );
    result.push_back( value.m_version );
    result.push_back( value.m_subversion );
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<
    NodeProvisioningManager::InformationBlockType::MaxInclusionRequestInterval,
    int>( const int& value, std::vector<uint8_t>& result ) const {
    result.push_back( concatenateMetaDataType( InformationBlockType::MaxInclusionRequestInterval, false ) );
    result.push_back( 0x01 );  // Length
    result.push_back( value / 128 );
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<NodeProvisioningManager::InformationBlockType::UUID16,
                                                   ProvisioningListEntryFields::UuidInformation>(
    const ProvisioningListEntryFields::UuidInformation& value,
    std::vector<uint8_t>& result ) const {
    result.push_back( concatenateMetaDataType( InformationBlockType::UUID16, false ) );
    result.push_back( 0x11 );  // Length
    result.push_back( value.m_presentationFormat );
    for( auto byte : value.m_uuid ) {
        result.push_back( byte );
    }
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<NodeProvisioningManager::InformationBlockType::Name, std::string>(
    const std::string& value,
    std::vector<uint8_t>& result ) const {
    result.push_back( concatenateMetaDataType( InformationBlockType::Name, false ) );
    result.push_back( value.size() );  // Length
    for( auto character : value ) {
        result.push_back( character );
    }
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<NodeProvisioningManager::InformationBlockType::Location,
                                                   std::string>( const std::string& value,
                                                                 std::vector<uint8_t>& result ) const {
    result.push_back( concatenateMetaDataType( InformationBlockType::Name, false ) );
    result.push_back( value.size() );  // Length
    for( auto character : value ) {
        result.push_back( character );
    }
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<
    NodeProvisioningManager::InformationBlockType::SmartStartInclusionSetting,
    ProvisioningListEntryFields::InclusionSetting>( const ProvisioningListEntryFields::InclusionSetting& value,
                                                    std::vector<uint8_t>& result ) const {
    result.push_back( concatenateMetaDataType( InformationBlockType::SmartStartInclusionSetting, true ) );
    result.push_back( 0x01 );  // Length
    result.push_back( static_cast<uint8_t>( value ) );
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<NodeProvisioningManager::InformationBlockType::AdvancedJoining,
                                                   uint8_t>( const uint8_t& value,
                                                             std::vector<uint8_t>& result ) const {
    result.push_back( concatenateMetaDataType( InformationBlockType::AdvancedJoining, true ) );
    result.push_back( 0x01 );  // Length
    result.push_back( value );
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<NodeProvisioningManager::InformationBlockType::BootstrappingMode,
                                                   ProvisioningListEntryFields::AddingMode>(
    const ProvisioningListEntryFields::AddingMode& value,
    std::vector<uint8_t>& result ) const {
    uint8_t valueToSend;
    switch( value ) {
    case ProvisioningListEntryFields::AddingMode::S2:
    case ProvisioningListEntryFields::AddingMode::SMART:
        valueToSend = static_cast<uint8_t>( value );
        break;

    default:
        valueToSend = static_cast<uint8_t>( ProvisioningListEntryFields::AddingMode::SMART );
    }

    result.push_back( concatenateMetaDataType( InformationBlockType::BootstrappingMode, true ) );
    result.push_back( 0x01 );  // Length
    result.push_back( valueToSend );
}

template <>
void NodeProvisioningManager::extractMetaDataBlock<NodeProvisioningManager::InformationBlockType::NetworkStatus,
                                                   std::pair<uint8_t, ProvisioningListEntryFields::Status>>(
    const std::pair<uint8_t, ProvisioningListEntryFields::Status>& value,
    std::vector<uint8_t>& result ) const {
    uint8_t status;
    switch( value.second ) {
    case ProvisioningListEntryFields::Status::ADDED:
        status = 1;
        break;
    case ProvisioningListEntryFields::Status::NOT_DETECTED:
        status = 0;
        break;

    default:
        status = 2;
    }
    result.push_back( concatenateMetaDataType( InformationBlockType::NetworkStatus, false ) );
    result.push_back( 0x02 );  // Length
    result.push_back( value.first );
    result.push_back( status );
}

template <NodeProvisioningManager::InformationBlockType BlockType, typename ValueType>
void NodeProvisioningManager::tryExtractMetaDataBlock( utils::optional<ValueType> value,
                                                       std::vector<uint8_t>& result ) const {
    if( value ) {
        extractMetaDataBlock<BlockType>( *value, result );
    }
}

void NodeProvisioningManager::extractMetaData( const ProvisioningListEntryFields& fields,
                                               std::vector<uint8_t>& result ) const {
    tryExtractMetaDataBlock<InformationBlockType::ProductType>( fields.m_type, result );
    tryExtractMetaDataBlock<InformationBlockType::ProductId>( fields.m_product, result );
    tryExtractMetaDataBlock<InformationBlockType::MaxInclusionRequestInterval>( fields.m_maxRequestInterval, result );
    tryExtractMetaDataBlock<InformationBlockType::UUID16>( fields.m_uuidInformation, result );
    tryExtractMetaDataBlock<InformationBlockType::Name>( fields.m_name, result );
    tryExtractMetaDataBlock<InformationBlockType::Location>( fields.m_location, result );

    assert( fields.m_inclusionSetting.has_value() );
    extractMetaDataBlock<InformationBlockType::SmartStartInclusionSetting>( *fields.m_inclusionSetting, result );

    tryExtractMetaDataBlock<InformationBlockType::AdvancedJoining>( fields.m_securityModes, result );

    assert( fields.m_addingMode.has_value() );
    extractMetaDataBlock<InformationBlockType::BootstrappingMode>( *fields.m_addingMode, result );

    assert( fields.m_addingMode.has_value() );
    assert( fields.m_node.has_value() );
    extractMetaDataBlock<InformationBlockType::NetworkStatus>( std::make_pair( *fields.m_node, *fields.m_status ),
                                                               result );
}

uint8_t NodeProvisioningManager::concatenateMetaDataType( InformationBlockType type, bool critical ) const {
    return ( static_cast<uint8_t>( type ) << 1 ) | ( critical ? 1 : 0 );
}

std::string NodeProvisioningManager::generateUuidString(
    const ProvisioningListEntryFields::UuidInformation& info ) const {
    auto to32Hex = []( const std::array<uint8_t, 16>& uuid ) -> std::string {
        std::string str;
        str.reserve( 32 );

        char buf[3];
        for( int i = 0; i < 16; ++i ) {
            sprintf( buf, "%02hx", uuid[i] );
            str += buf;
        }

        return str;
    };

    auto to16Char = []( const std::array<uint8_t, 16>& uuid ) -> std::string {
        std::string str;
        str.reserve( 16 );

        for( int i = 0; i < 16; ++i ) {
            str += static_cast<char>( uuid[i] );
        }

        return str;
    };

    std::string result;
    switch( info.m_presentationFormat ) {
    case 0:
        result = to32Hex( info.m_uuid );
        break;

    case 1:
        result = to16Char( info.m_uuid );
        break;

    case 2:
        result = "sn:" + to32Hex( info.m_uuid );
        break;

    case 3:
        result = "sn:" + to16Char( info.m_uuid );
        break;

    case 4:
        result = "UUID:" + to32Hex( info.m_uuid );
        break;

    case 5:
        result = "UUID:" + to16Char( info.m_uuid );
        break;

    case 6: {
        result = to32Hex( info.m_uuid );
        size_t positions[] = { 20, 16, 12, 8 };
        for( auto position : positions ) {
            result.insert( position, "-" );
        }
    } break;
    }

    return result;
}

std::string NodeProvisioningManager::generateEntryName( const ProvisioningListEntryFields& fields ) const {
    std::string name{ "Zwave Device" };
    if( fields.m_product ) {
        name = std::string{ manufacturer_names_converter::manufacturerIdToName( fields.m_product->m_manufacturerId ) } +
               " " + name;
    }

    return name;
}

uint16_t NodeProvisioningManager::unit( uint8_t lowByte, uint8_t high_byte ) const {
    return static_cast<uint16_t>( high_byte ) << 8 | lowByte;
}

std::pair<uint8_t, uint8_t> NodeProvisioningManager::split( uint16_t value ) const {
    return std::make_pair( (uint8_t)value, ( uint8_t )( value >> 8 ) );
}

}  // namespace znet
