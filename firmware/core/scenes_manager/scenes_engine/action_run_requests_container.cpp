#include "scenes_manager/scenes_engine/action_run_requests_container.h"

namespace SceneEngine {

void ActionRunRequestsContainer::push( SceneActionExecutePtr executor, AbstractOperator::OperatorState state ) {
    std::unique_lock< std::mutex > lock( m_entriesMutex );
    m_entries.push_back( Entry{ std::move( executor ), std::string{}, state } );
}

void ActionRunRequestsContainer::push( SceneActionExecutePtr executor,
                                       std::string blockId,
                                       AbstractOperator::OperatorState state ) {
    std::unique_lock< std::mutex > lock( m_entriesMutex );
    m_entries.push_back( Entry{ std::move( executor ), std::move( blockId ), state } );
}

void ActionRunRequestsContainer::runActions() {
    std::vector< Entry > entries;
    {
        std::unique_lock< std::mutex > lock( m_entriesMutex );
        entries.swap( m_entries );
    }

    std::set< Entry > uniqueEntries;
    auto filteringPredicate = [&uniqueEntries]( const Entry& entry ) {
        auto insertResult = uniqueEntries.insert( entry );
        return !insertResult.second;
    };
    entries.erase( std::remove_if( entries.begin(), entries.end(), filteringPredicate ), entries.end() );

    for( auto& entry : entries ) {
        if( entry.blockId.empty() ) {
            if( entry.state == AbstractOperator::True )
                entry.executor->runThenActions();
            else if( entry.state == AbstractOperator::False )
                entry.executor->runElseActions();
        }
        else {
            if( entry.state == AbstractOperator::True )
                entry.executor->runThenActions( entry.blockId );
            else if( entry.state == AbstractOperator::False )
                entry.executor->runElseActions( entry.blockId );
        }
    }
}

bool ActionRunRequestsContainer::Entry::operator<( const Entry& other ) const {
    auto toTuple = []( const Entry& entry ) {
        return std::make_tuple( entry.executor->getSceneId(), entry.blockId, entry.state );
    };
    return toTuple( *this ) < toTuple( other );
}

}  // namespace SceneEngine
