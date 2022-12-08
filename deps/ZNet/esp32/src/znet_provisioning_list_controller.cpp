#include "znet/znet_provisioning_list_controller.hpp"

#include "znet/znet.h"
#include "znet/iznet_provisioning_list_provider.hpp"
#include "znet/iznet_provisioning_list_observer.hpp"

#include "znet/znet_provisioning_list_query.hpp"
#include "znet/znet_provisioning_list_query_filters.hpp"

#include <algorithm>

namespace fields = ::znet::api::filters::fields;

namespace znet {
namespace api {

ProvisioningListController::ProvisioningListController(
    IProvisioningListQueryDataSource& classicInclusionSecurityDataSource,
    IProvisioningListProvider& provider,
    IProvisioningListObserver& observer )
: m_classisInclusionSecurityDataSource( classicInclusionSecurityDataSource )
, m_provider( provider )
, m_observer( observer ) {}

IProvisioningListController::OperationStatus ProvisioningListController::addProvisioningListEntry(
    const DSK& dsk,
    const ProvisioningListEntryFields& newFields ) {
    ProvisioningListEntry existingEntry;
    if( m_provider.findProvisioningListEntryByDSK( dsk, existingEntry ) ) {
        ProvisioningListEntry newEntry{ .m_provisioningId = existingEntry.m_provisioningId,
                                        .m_dsk = existingEntry.m_dsk,
                                        .m_fields = setDefaults( newFields ) };
        mergeWithClassicInclusionData( dsk, newEntry.m_fields );

        if( !isCorrectAddingMode( newEntry.m_fields.m_supportedAddingModes, *newEntry.m_fields.m_addingMode ) ) {
            return OperationStatus::INCORRECT_ADDING_MODE;
        }

        if( m_provider.storeProvisioningListEntry( newEntry ) ) {
            m_observer.onProvisioningListEntryUpdated( existingEntry, newEntry );
            return OperationStatus::ENTRY_REPLACED;
        }
        else {
            return OperationStatus::STORAGE_NOT_AVAILABLE;
        }
    }
    else if( MAX_RECORD_COUNT == m_provider.getProvisioningListEntryCount() ) {
        return OperationStatus::ENTRY_LIMIT_REACHED;
    }
    else {
        ProvisioningListEntry newEntry{
            .m_provisioningId = generateProvisioningID( dsk ), .m_dsk = dsk, .m_fields = setDefaults( newFields )
        };

        if( !isCorrectAddingMode( newEntry.m_fields.m_supportedAddingModes, *newEntry.m_fields.m_addingMode ) ) {
            return OperationStatus::INCORRECT_ADDING_MODE;
        }

        mergeWithClassicInclusionData( dsk, newEntry.m_fields );
        if( m_provider.storeProvisioningListEntry( newEntry ) ) {
            m_observer.onProvisioningListEntryAdded( newEntry );
            return OperationStatus::SUCCESS;
        }
        else {
            return OperationStatus::STORAGE_NOT_AVAILABLE;
        }
    }
}

IProvisioningListController::OperationStatus ProvisioningListController::setProvisioningListEntry(
    uint32_t provisioning_id,
    const ProvisioningListEntryFields& newFields ) {
    ProvisioningListEntry existingEntry;
    if( !m_provider.findProvisioningListEntryByID( provisioning_id, existingEntry ) ) {
        return OperationStatus::ENTRY_NOT_FOUND;
    }
    else {
        ProvisioningListEntry newEntry{ .m_provisioningId = existingEntry.m_provisioningId,
                                        .m_dsk = existingEntry.m_dsk,
                                        .m_fields = setDefaults( newFields ) };

        if( !isCorrectAddingMode( newEntry.m_fields.m_supportedAddingModes, *newEntry.m_fields.m_addingMode ) ) {
            return OperationStatus::INCORRECT_ADDING_MODE;
        }

        if( m_provider.storeProvisioningListEntry( newEntry ) ) {
            m_observer.onProvisioningListEntryUpdated( existingEntry, newEntry );
            return OperationStatus::SUCCESS;
        }
        else {
            return OperationStatus::STORAGE_NOT_AVAILABLE;
        }
    }
}

IProvisioningListController::OperationStatus ProvisioningListController::removeProvisioningListEntry(
    uint32_t provisioningId ) {
    ProvisioningListEntry entry;
    if( !m_provider.findProvisioningListEntryByID( provisioningId, entry ) ) {
        return OperationStatus::ENTRY_NOT_FOUND;
    }
    else if( m_provider.removeProvisioningListEntryByID( entry.m_provisioningId ) ) {
        m_observer.onProvisioningListEntryRemoved( entry.m_provisioningId );
        return OperationStatus::SUCCESS;
    }
    else {
        return OperationStatus::STORAGE_NOT_AVAILABLE;
    }
}

IProvisioningListController::OperationStatus ProvisioningListController::findProvisioningListEntry(
    const ProcessProvisioningCallback& callback ) const {
    bool entryFound = false;
    auto processEntry = [&entryFound, &callback]( const ProvisioningListEntry& entry ) {
        if( !entryFound ) {
            entryFound = callback( entry );
        }
        return entryFound;
    };
    if( !m_provider.loadProvisioningList( processEntry ) ) {
        return OperationStatus::STORAGE_NOT_AVAILABLE;
    }
    else {
        return entryFound ? OperationStatus::SUCCESS : OperationStatus::ENTRY_NOT_FOUND;
    }
}

uint32_t ProvisioningListController::generateProvisioningID( const DSK& dsk ) {
    auto hasher = std::hash<uint16_t>();
    auto iter = dsk.begin();
    auto seed = hasher( *iter++ );
    while( iter != dsk.end() ) {
        // https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
        seed ^= hasher( *iter++ ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
    }
    return seed;
}

ProvisioningListEntryFields ProvisioningListController::setDefaults( const ProvisioningListEntryFields& fields ) {
    ProvisioningListEntryFields updatedFields( fields );
    setFieldDefaultValue( updatedFields.m_node, ZNET_NODE_ID_ANY );
    setFieldDefaultValue( updatedFields.m_status, ProvisioningListEntryFields::Status::NOT_DETECTED );
    setFieldDefaultValue( updatedFields.m_addingMode, ProvisioningListEntryFields::AddingMode::SMART );
    setFieldDefaultValue( updatedFields.m_inclusionSetting, ProvisioningListEntryFields::InclusionSetting::PENDING );
    setFieldDefaultValue( updatedFields.m_securityModes, 0 );
    setFieldDefaultValue( updatedFields.m_supportedModes, 0xFF );

    if( updatedFields.m_supportedAddingModes.empty() ) {
        updatedFields.m_supportedAddingModes = { ProvisioningListEntryFields::AddingMode::S2,
                                                 ProvisioningListEntryFields::AddingMode::SMART };
    }

    return updatedFields;
}

template <typename FieldType, typename ValueType>
void ProvisioningListController::setFieldDefaultValue( utils::optional<FieldType>& field, const ValueType& value ) {
    if( !field ) {
        field = value;
    }
}

void ProvisioningListController::mergeWithClassicInclusionData( const DSK& dsk,
                                                                ProvisioningListEntryFields& newEntry ) {
    ProvisioningListQuery{ m_classisInclusionSecurityDataSource }
        .filterBy( fields::dsk( dsk ) )
        .tap( [&newEntry]( const ProvisioningListEntry& existingEntry ) {
            newEntry.m_node = existingEntry.m_fields.m_node;
            newEntry.m_addingMode = ProvisioningListEntryFields::AddingMode::S2;
            newEntry.m_status = ProvisioningListEntryFields::Status::ADDED;
            if( existingEntry.m_fields.m_supportedModes ) {
                newEntry.m_supportedModes = existingEntry.m_fields.m_supportedModes;
            }
            if( existingEntry.m_fields.m_securityModes ) {
                newEntry.m_securityModes = existingEntry.m_fields.m_securityModes;
            }
            if( existingEntry.m_fields.m_nodeSecurityModes ) {
                newEntry.m_nodeSecurityModes = existingEntry.m_fields.m_nodeSecurityModes;
            }
            return true;
        } )
        .execute();
}

bool ProvisioningListController::isCorrectAddingMode(
    const std::vector<ProvisioningListEntryFields::AddingMode>& supportedModes,
    const ProvisioningListEntryFields::AddingMode& mode ) const {
    return std::find( supportedModes.begin(), supportedModes.end(), mode ) != supportedModes.end();
}

}  // namespace api
}  // namespace znet
