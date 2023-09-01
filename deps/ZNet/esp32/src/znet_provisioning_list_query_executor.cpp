#include "znet/znet_provisioning_list_query_executor.hpp"

namespace znet {
namespace api {

using operation_status_t = ::znet::api::IProvisioningListController::OperationStatus;

::znet::api::IProvisioningListController::OperationStatus ProvisioningListQueryExecutor::execute(
    IProvisioningListQueryDataSource& dataSource,
    ReadOnlyStatement readOnlyStatement,
    ModifyingStatement modifyingStatement ) {
    ::znet::ProvisioningListEntry entry{ .m_provisioningId = 0, .m_dsk = { 0 }, .m_fields = {} };
    operation_status_t iterateStatus = operation_status_t::QUERY_NOT_READY;
    if( !readOnlyStatement.empty() ) {
        // Append a clause that stores filtered entry for further processing by the modifying statement.
        if( !modifyingStatement.empty() ) {
            readOnlyStatement.addClause( [&entry]( const ::znet::ProvisioningListEntry& e ) {
                entry = e;
                return true;
            } );
        }
        iterateStatus = dataSource.iterate( readOnlyStatement.asStdFunction() );
    }
    if( operation_status_t::SUCCESS == iterateStatus || modifyingStatement.forceUpdate() ) {
        if( modifyingStatement.isMarkedForRemove() ) {
            return dataSource.remove( entry.m_provisioningId );
        }
        if( !modifyingStatement.empty() ) {
            modifyingStatement.asStdFunction()( entry );
            return dataSource.update( entry );
        }
    }
    return iterateStatus;
}

}  // namespace api
}  // namespace znet
