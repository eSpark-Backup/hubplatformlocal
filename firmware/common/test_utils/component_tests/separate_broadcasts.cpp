#include "separate_broadcasts.h"
#include <utility>

namespace HubPlatform {
namespace Tests {

SeparateBroadcasts::SeparateBroadcasts( CommonNmaTests& commonNma, std::string broadcastName )
: m_commonNma( commonNma )
, m_broadcastName( std::move( broadcastName ) ) {}

Json SeparateBroadcasts::waitForUiBroadcast( const std::string& message,
                                             const char* file,
                                             int line,
                                             std::chrono::seconds timeout ) {
    try {
        std::lock( m_progressBroadcastsMutex, m_uiBroadcastsMutex );
        std::unique_lock< std::mutex > lock1( m_progressBroadcastsMutex, std::adopt_lock );
        std::unique_lock< std::mutex > lock2( m_uiBroadcastsMutex, std::adopt_lock );
        if( !m_uiBroadcastMessages.empty() ) {
            auto tmp = m_uiBroadcastMessages.front();
            m_uiBroadcastMessages.pop_front();
            return tmp;
        }
        else {
            while( true ) {
                Json broadcast = m_commonNma.waitForBroadcast( file, line, timeout );
                if( json_get_str( broadcast, "msg_subclass" ) != m_broadcastName ) {
                    return broadcast;
                }
                else {
                    m_progressBroadcastMessages.push_back( std::move( broadcast ) );
                }
            }
        }
    }
    catch( const std::exception& ex ) {
        throw std::runtime_error( "could not get ui broadcast: " + message + ", reason: " + ex.what() );
    }
}

Json SeparateBroadcasts::waitForProgressBroadcast( const std::string& message,
                                                   const char* file,
                                                   int line,
                                                   std::chrono::seconds timeout ) {
    try {
        std::lock( m_progressBroadcastsMutex, m_uiBroadcastsMutex );
        std::unique_lock< std::mutex > lock1( m_progressBroadcastsMutex, std::adopt_lock );
        std::unique_lock< std::mutex > lock2( m_uiBroadcastsMutex, std::adopt_lock );
        if( !m_progressBroadcastMessages.empty() ) {
            auto tmp = m_progressBroadcastMessages.front();
            m_progressBroadcastMessages.pop_front();
            return tmp;
        }
        else {
            while( true ) {
                Json broadcast = m_commonNma.waitForBroadcast( file, line, timeout );
                if( json_get_str( broadcast, "msg_subclass" ) == m_broadcastName ) {
                    return broadcast;
                }
                else {
                    m_uiBroadcastMessages.push_back( std::move( broadcast ) );
                }
            }
        }
    }
    catch( const std::exception& ex ) {
        throw std::runtime_error( "could not get progress broadcast: " + message + ", reason: " + ex.what() );
    }
}

}  // namespace Tests
}  // namespace HubPlatform