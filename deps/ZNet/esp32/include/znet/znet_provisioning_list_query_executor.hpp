#pragma once

#include <znet/iznet_provisioning_list_query_data_source.hpp>
#include <znet/znet_provisioning_list_query_statements.hpp>

namespace znet {
namespace api {

/**
 * Encapsulates the algorithm that combines combines read-only and modifying query statements and
 * performs the actual interaction with the data source.
 */
class ProvisioningListQueryExecutor {
public:
    /**
     * Performs query to the data source using given read-only and modifying statements.
     * In case if find operation reports success, the method also applies modifying statement to the target
     * entry and pushes the update to the data source.
     * Modifying statement is applied to the first entry that satisfies full read-only statement.
     * @return QUERY_NOT_READY if read-only statement is not set, or status of find/set operations otherwise.
     */
    static ::znet::api::IProvisioningListController::OperationStatus execute(
        IProvisioningListQueryDataSource& dataSource,
        ReadOnlyStatement readOnlyStatement,
        ModifyingStatement modifyingStatement );
};

}  // namespace api
}  // namespace znet
