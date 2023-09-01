#pragma once

#include <functional>

#include <znet/znet_provisioning_list_entry.hpp>

namespace znet {
namespace api {

/**
 * Interface of a piece of Z-wave gateway functionality responsible for management of a Provisioning List.
 *
 * Provisioning List stores S2 DSKs used during device inclusion in Smart Start mode.
 * For more details on Smart Start topic see the following quick start guide:
 * https://www.silabs.com/documents/login/white-papers/introduction_to_z-wave_smartstart_091317.pdf
 */
class IProvisioningListController {
public:
    using ProcessProvisioningCallback = std::function<bool( const ProvisioningListEntry& )>;
    static constexpr auto MAX_RECORD_COUNT = 232;

    enum class OperationStatus {
        SUCCESS,               /* Operation successfully completed. */
        ENTRY_REPLACED,        /* Entry with a given DSK found and updated. */
        ENTRY_NOT_FOUND,       /* Entry with a given DSK / ID does not present in the Provisioning List. */
        ENTRY_LIMIT_REACHED,   /* Number of entries in the Provisioning List reached the limit. */
        STORAGE_NOT_AVAILABLE, /* Provisioning List storage is not available. */
        QUERY_NOT_READY,       /* Provisioning List Query must have at least one filter criteria. */
        INCORRECT_ADDING_MODE, /* Unsupported adding mode was used */
    };

    virtual ~IProvisioningListController() = default;

    /**
     * Adds or replaces a S2 DSK entry in the Provisioning List.
     *
     * Once the entry is created, it is assigned a unique ID. In case if an entry with such a DSK already exists,
     * the method will only replace its fields, however the ID will remain unchanged.
     * Maximum number of records is limited by MAX_RECORD_COUNT.
     *
     * @param  dsk          Unique DSK key for Z-Wave device which can be included via Smart Start flow.
     * @param  adding_mode  This Information Type is used to advertise the bootstrapping mode to use when including
     *                      the node advertised in Provisioning List entry.
     * @param  fields       Additional entry fields. Required: addingMode, status, nodeId.
     * @return Operation status:
     *           - success: SUCCESS / ENTRY_REPLACED
     *           - errors: STORAGE_NOT_AVAILABLE / ENTRY_LIMIT_REACHED / ENTRY_INCOMPLETE
     */
    virtual OperationStatus addProvisioningListEntry( const DSK& dsk, const ProvisioningListEntryFields& fields ) = 0;

    /**
     * Updates fields of a given entry in the Provisioning List.
     *
     * @param provisioningId    Unique ID of a Provisioning List entry.
     * @param addingMode        This Information Type is used to advertise the bootstrapping mode to use when including
     *                          the node advertised in Provisioning List entry.
     * @param fields            Additional entry fields that replace existing entry fields.
     *                          Required: addingMode, status, nodeId.
     * @return Operation status:
     *           - success: SUCCESS
     *           - errors: STORAGE_NOT_AVAILABLE / ENTRY_NOT_FOUND / ENTRY_INCOMPLETE
     */
    virtual OperationStatus setProvisioningListEntry( uint32_t provisioningId,
                                                      const ProvisioningListEntryFields& fields ) = 0;

    /**
     * Removes entry with a given ID from the Provisioning List.
     *
     * @param provisioningId Unique ID of a Provisioning List entry.
     * @return Operation status:
     *           - success: SUCCESS
     *           - errors: STORAGE_NOT_AVAILABLE / ENTRY_NOT_FOUND
     */
    virtual OperationStatus removeProvisioningListEntry( uint32_t provisioningId ) = 0;

    /**
     * Subsequently calls a given callback for each entry in the Provisioning List until the callback returns true.
     * In case if the callback doesn't return true for any record, ENTRY_NOT_FOUND status is returned.
     *
     * @param callback A function subsequently called for each entry.
     *                 Must return true to stop iteration with SUCCESS or false to continue until
     *                 ENTRY_NOT_FOUND.
     * @return Operation status:
     *           - success: SUCCESS
     *           - errors: STORAGE_NOT_AVAILABLE / ENTRY_NOT_FOUND
     */
    virtual OperationStatus findProvisioningListEntry( const ProcessProvisioningCallback& callback ) const = 0;
};

}  // namespace api
}  // namespace znet
