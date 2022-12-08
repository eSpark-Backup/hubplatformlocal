#include <random>

#include "znet/znet_provisioning_list_provider_data_source.hpp"

namespace znet {
namespace api {

using operation_status_t = ::znet::api::IProvisioningListController::OperationStatus;

ProvisioningListProviderDataSource::ProvisioningListProviderDataSource(::znet::IProvisioningListProvider& provider )
: m_provider( provider ) {}

operation_status_t ProvisioningListProviderDataSource::iterate( const iterate_function_t& iterateFunction ) const {
    bool hasEntry = false;
    if( !m_provider.loadProvisioningList( [&hasEntry, &iterateFunction]( const ::znet::ProvisioningListEntry& entry ) {
            return ( hasEntry = hasEntry || iterateFunction( entry ) );
        } ) ) {
        return operation_status_t::STORAGE_NOT_AVAILABLE;
    }
    return hasEntry ? operation_status_t::SUCCESS : operation_status_t::ENTRY_NOT_FOUND;
}

operation_status_t ProvisioningListProviderDataSource::update( const ProvisioningListEntry& entry ) {
    ProvisioningListEntry newEntry = entry, unused;
    if( !m_provider.findProvisioningListEntryByID( entry.m_provisioningId, unused ) ) {
        std::random_device rd;
        std::default_random_engine e( rd() );
        newEntry.m_provisioningId = static_cast<uint32_t>( e() );
    }
    return m_provider.storeProvisioningListEntry( newEntry ) ? operation_status_t::SUCCESS :
                                                               operation_status_t::STORAGE_NOT_AVAILABLE;
}

operation_status_t ProvisioningListProviderDataSource::remove( uint32_t id ) {
    return m_provider.removeProvisioningListEntryByID( id ) ? operation_status_t::SUCCESS :
                                                              operation_status_t::STORAGE_NOT_AVAILABLE;
}

}  // namespace api
}  // namespace znet
