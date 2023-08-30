#include "Device.h"
#include <assert.h>

void Device::inclusionSetReady() {
    m_inclusionReady = true;
}

void Device::inclusionClearReady() {
    m_inclusionReady = false;
}

bool Device::inclusionIsReady() {
    return m_inclusionReady;
}

void Device::addCC( std::initializer_list<std::shared_ptr<CommandClass>> CC ) {
    for( auto mx : m_CC ) {
        for( auto x : CC ) {
            if( mx.second->get_id() == x->get_id() ) {
                assert( !"Re-adding CommandClass" );
            }
        }
    }

    for( auto x : CC ) {
        m_CC[x->get_id()] = x;
    }
}

std::shared_ptr<CommandClass> Device::getCC( uint8_t n ) {
    assert( m_CC.count( n ) );
    return m_CC.at( n );
}
