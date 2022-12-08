#include "cc/Association.h"
#include <assert.h>
#include <iostream>

Association::Association()
: CommandClass( CommandClass::CommandClassId::association, 1 ) {}

bool Association::run( std::vector<std::vector<uint8_t>>& rev,
                       std::vector<uint8_t>& ret,
                       std::vector<uint8_t> const& data ) {
    std::cout << "Association\n";

    switch( data.at( 1 ) ) {
    case Command::set:
        return cset( ret, data );
        break;

    case Command::get:
        return cget( ret, data );
        break;

    default:
        assert( !"Error Command" );
    }

    return false;
}

bool Association::cset( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data ) {
    uint8_t groupingIdentifier = data.at( 2 );
    assert( groupingIdentifier > 0 );
    uint8_t nodeNumber = data.size() - 1 /*Command Class*/ - 1 /*Command*/ - 1 /*Grouping Identifier*/;
    assert( nodeNumber > 0 );

    if( m_group.count( groupingIdentifier ) > 0 ) {
        m_group.erase( groupingIdentifier );
    }
    std::vector<uint8_t> nodes( data.begin() + 1 /*Command Class*/ + 1 /*Command*/ + 1 /*Grouping Identifier*/,
                                data.end() );
    m_group[groupingIdentifier] = nodes;

    return true;
}

bool Association::cget( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data ) {
    uint8_t groupingIdentifier = data.at( 2 );
    assert( groupingIdentifier > 0 );
    if( m_group.count( groupingIdentifier ) == 0 ) {
        return false;
    }

    uint8_t nodeNumber = m_group[groupingIdentifier].size();
    ret = { get_id(), Command::report, groupingIdentifier, m_maxNodesSupported, nodeNumber };
    for( auto x : m_group[groupingIdentifier] ) {
        ret.push_back( x );
    }

    return true;
}
