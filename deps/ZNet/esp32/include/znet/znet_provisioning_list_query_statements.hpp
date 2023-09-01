#pragma once

#include <functional>

namespace znet {
struct ProvisioningListEntry;
namespace api {

/**
 * Defines the way read-only clauses are collected into a single statement.
 */
class ReadOnlyStatement {
public:
    using function_type_t = std::function<bool( const ::znet::ProvisioningListEntry& )>;

    ReadOnlyStatement() = default;

    explicit ReadOnlyStatement( function_type_t statement )
    : m_statement( std::move( statement ) ) {}

    /**
     * Adds the specified function to the end of the read-only chain by combining it with logical AND, i.e. an entry
     * proceeds to the next function within the read-only chain if only the previous function returns true.
     */
    void addClause( function_type_t nextClause ) {
        if( !m_statement ) {
            m_statement = std::move( nextClause );
        }
        else {
            // TODO: move clauses to lambda in c++14
            m_statement = std::bind(
                []( const function_type_t& currentClause,
                    const function_type_t& nextClause,
                    const ::znet::ProvisioningListEntry& entry ) {
                    return currentClause( entry ) && nextClause( entry );
                },
                std::move( m_statement ),
                std::move( nextClause ),
                std::placeholders::_1 );
        }
    }

    const function_type_t& asStdFunction() const {
        return m_statement;
    }

    bool empty() const {
        return !m_statement.operator bool();
    }

private:
    function_type_t m_statement;
};

/**
 * Defines the way modifying clauses are collected into a single statement.
 */
class ModifyingStatement {
public:
    using function_type_t = std::function<void(::znet::ProvisioningListEntry& )>;

    /**
     * Adds the specified function to the end of the modifying chain.
     */
    void addClause( function_type_t nextClause ) {
        if( !m_statement ) {
            m_statement = std::move( nextClause );
        }
        else {
            // TODO: move clauses to lambda in c++14
            m_statement = std::bind(
                []( const function_type_t& currentClause,
                    const function_type_t& nextClause,
                    ::znet::ProvisioningListEntry& entry ) {
                    currentClause( entry );
                    nextClause( entry );
                },
                std::move( m_statement ),
                std::move( nextClause ),
                std::placeholders::_1 );
        }
    }

    void enableForceUpdate() {
        m_forcedUpdate = true;
    }

    bool forceUpdate() const {
        return m_forcedUpdate;
    }

    void markForRemove() {
        m_remove = true;
    }

    bool isMarkedForRemove() const {
        return m_remove;
    }

    const function_type_t& asStdFunction() const {
        return m_statement;
    }

    bool empty() const {
        return !m_statement.operator bool() && !m_remove;
    }

private:
    function_type_t m_statement;
    bool m_forcedUpdate = false;
    bool m_remove = false;
};

}  // namespace api
}  // namespace znet
