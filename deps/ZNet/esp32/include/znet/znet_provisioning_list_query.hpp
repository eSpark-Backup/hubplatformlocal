#pragma once

extern "C" {
#include <byte_ops.h>
}

#include <znet/znet_provisioning_types.hpp>
#include <znet/znet_provisioning_list_query_filters.hpp>
#include <znet/znet_provisioning_list_query_executor.hpp>

namespace znet {
namespace api {

class ProvisioningListQueryBase {
public:
    explicit ProvisioningListQueryBase( IProvisioningListQueryDataSource& dataSource )
    : m_dataSource( dataSource ) {}

    bool execute(::znet::api::IProvisioningListController::OperationStatus& status ) {
        status =
            ProvisioningListQueryExecutor::execute( m_dataSource, getReadOnlyStatement(), getModifyingStatement() );
        return ::znet::api::IProvisioningListController::OperationStatus::SUCCESS == status;
    }

    bool execute() {
        ::znet::api::IProvisioningListController::OperationStatus status;
        return execute( status );
    }

private:
    virtual ModifyingStatement getModifyingStatement() const {
        return {};
    }
    virtual ReadOnlyStatement getReadOnlyStatement() const {
        return {};
    }

    IProvisioningListQueryDataSource& m_dataSource;
};

/**
 * Utility class that provides simple interface for regular modifying operations on entries in the Provisioning List.
 * It extends base functionality of ProvisioningListControllerDataSource with some domain-specific modifying query
 * methods.
 *
 * You must build your queries using ProvisioningListReadQuery (aka ProvisioningListQuery), because query must always
 * contain at least one read method.
 */
class ProvisioningListModifyingQuery : public ProvisioningListQueryBase {
public:
    explicit ProvisioningListModifyingQuery( IProvisioningListQueryDataSource& dataSource )
    : ProvisioningListQueryBase{ dataSource } {}

    template <typename FieldType, typename ValueType>
    ProvisioningListModifyingQuery& setField( FieldType(::znet::ProvisioningListEntryFields::*fieldPtr ),
                                              const ValueType& fieldValue ) {
        m_modifyingStatement.addClause(
            [fieldPtr, fieldValue](::znet::ProvisioningListEntry& entry ) { entry.m_fields.*fieldPtr = fieldValue; } );
        return *this;
    }

    ProvisioningListModifyingQuery& setDsk( const DSK& fullDsk ) {
        m_modifyingStatement.addClause( [fullDsk](::znet::ProvisioningListEntry& entry ) { entry.m_dsk = fullDsk; } );
        return *this;
    }

    ProvisioningListModifyingQuery& setDsk( const std::array<uint8_t, 2>& key ) {
        m_modifyingStatement.addClause(
            [key](::znet::ProvisioningListEntry& entry ) { entry.m_dsk[0] = bytes_to_int16( key.data() ); } );
        return *this;
    }

    ProvisioningListModifyingQuery& createIfNotExist() {
        m_modifyingStatement.enableForceUpdate();
        return *this;
    }

    ProvisioningListQueryBase& remove() {
        m_modifyingStatement.markForRemove();
        return *this;
    }

private:
    ModifyingStatement getModifyingStatement() const override {
        return m_modifyingStatement;
    }

    ModifyingStatement m_modifyingStatement;
};

/**
 * Utility class that provides simple interface for regular search operations in the Provisioning List.
 * It extends base functionality of ProvisioningListControllerDataSource with some domain-specific read-only query
 * methods (filters and getters).
 *
 * The interface also includes modifying operations inherited from ProvisioningListModifyingQuery.
 * Note that Read methods return ProvisioningListReadQuery, while Modifying methods return
 * ProvisioningListModifyingQuery. This affects method chaining mechanism: you cannot chain read methods after modifying
 * methods (it doesn't actually make any sense, and the limitation is intended to prevent occasional errors).
 *
 * Usage:
 *      ProvisioningListQuery{ executor }
 *          .filterByNodeId( nodeId )
 *          .getDskDerivedHomeId( outputBuffer )
 *          .setStatus( newStatus )
 *          .execute();
 */
class ProvisioningListReadQuery : public ProvisioningListModifyingQuery {
public:
    explicit ProvisioningListReadQuery( IProvisioningListQueryDataSource& dataSource )
    : ProvisioningListModifyingQuery{ dataSource } {}

    ProvisioningListReadQuery& filterBy( const filters::Filter& filter ) {
        m_readOnlyStatement.addClause( filter.m_clause );
        return *this;
    }

    ProvisioningListReadQuery& getDskDerivedHomeId( DskDerivedHomeId& dskDerivedHomeId ) {
        m_readOnlyStatement.addClause( [&dskDerivedHomeId]( const ::znet::ProvisioningListEntry& entry ) {
            int16_to_bytes( set_last_2_bits( entry.m_dsk[4] ), &dskDerivedHomeId[0] );
            int16_to_bytes( reset_first_bit( entry.m_dsk[5] ), &dskDerivedHomeId[2] );
            int16_to_bytes( set_last_2_bits( entry.m_dsk[6] ), &dskDerivedHomeId[4] );
            int16_to_bytes( reset_first_bit( entry.m_dsk[7] ), &dskDerivedHomeId[6] );
            return true;
        } );
        return *this;
    }

    ProvisioningListReadQuery& getPubKey( std::array<uint8_t, 2>& dsk ) {
        m_readOnlyStatement.addClause( [&dsk]( const ::znet::ProvisioningListEntry& entry ) {
            int16_to_bytes( entry.m_dsk[0], &dsk[0] );
            return true;
        } );
        return *this;
    }

    template <typename FieldType>
    ProvisioningListReadQuery& getField( FieldType(::znet::ProvisioningListEntryFields::*fieldPtr ),
                                         FieldType& fieldValue ) {
        m_readOnlyStatement.addClause( [fieldPtr, &fieldValue]( const ::znet::ProvisioningListEntry& entry ) {
            fieldValue = entry.m_fields.*fieldPtr;
            return true;
        } );
        return *this;
    }

    template <typename FieldType, typename ValueType>
    ProvisioningListReadQuery& getField( FieldType(::znet::ProvisioningListEntryFields::*fieldPtr ),
                                         ValueType& fieldValue,
                                         const ValueType& defaultValue ) {
        m_readOnlyStatement.addClause(
            [fieldPtr, &fieldValue, defaultValue]( const ::znet::ProvisioningListEntry& entry ) {
                if( entry.m_fields.*fieldPtr ) {
                    fieldValue = *( entry.m_fields.*fieldPtr );
                }
                else {
                    fieldValue = defaultValue;
                }
                return true;
            } );
        return *this;
    }

    ProvisioningListReadQuery& tap( const ReadOnlyStatement::function_type_t& function ) {
        m_readOnlyStatement.addClause( static_cast<ReadOnlyStatement::function_type_t>(
            [function]( const ::znet::ProvisioningListEntry& entry ) { return function( entry ); } ) );
        return *this;
    }

private:
    ReadOnlyStatement getReadOnlyStatement() const override {
        return m_readOnlyStatement;
    }

    ReadOnlyStatement m_readOnlyStatement;
};

using ProvisioningListQuery = ProvisioningListReadQuery;

}  // namespace api
}  // namespace znet
