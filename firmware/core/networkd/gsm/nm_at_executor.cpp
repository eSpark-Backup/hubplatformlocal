#include "networkd/gsm/nm_at_executor.h"
#include "networkd/gsm/nm_gsm_exceptions.h"
#include "networkd/utils/nm_utils.h"

#include "system_utils/filesystem.h"
#include "system_utils/system.h"
#include "log_utils/ezlo_log.h"
#include "tiny/tiny_consolecolors.h"

#include <utility>
#include <vector>
#include <thread>
#include <cctype>

namespace HubPlatform {
namespace Network {
namespace GSM {

IATExecutor::~IATExecutor() = default;

ATExecutor::ATExecutor( SerialSettings serialSettings )
: m_serialSettings( std::move( serialSettings ) )
, m_isInitialized( true ) {}

ATExecutor::~ATExecutor() {
    try {
        auto& tty = m_serialSettings.controlTTY;
        if( m_isInitialized && filesystem::isExist( tty ) && !filesystem::isCharacterDeviceExist( tty ) )
            filesystem::removeFile( tty.c_str() );
    }
    catch( ... ) {
    }
}

void ATExecutor::setSerialSettings( SerialSettings serialSettings ) {
    m_serialSettings = std::move( serialSettings );
}

bool ATExecutor::lock() {
    std::lock_guard< std::timed_mutex > lock( m_mutex );
    if( !m_locked ) {
        m_locked = true;
        return true;
    }

    return false;
}

void ATExecutor::unlock() {
    std::lock_guard< std::timed_mutex > lock( m_mutex );
    m_locked = false;
}

bool ATExecutor::isAccessible() const {
    std::unique_lock< std::timed_mutex > lock( m_mutex, std::defer_lock );
    if( lock.try_lock_for( m_maxChannelLockDuration ) ) {
        auto checkChannelCommand = "chat -e -t 3 'ABORT' 'ERROR' '' 'at' 'OK' 1> " + m_serialSettings.controlTTY +
                                   " < " + m_serialSettings.controlTTY;
        int result;

        if(::access( m_serialSettings.controlTTY.c_str(), F_OK ) ) {
            return false;
        }

        std::tie( std::ignore, result ) = System::getCommandOutput( checkChannelCommand.c_str() );
        if( result != 0 ) {
            LOGGER_DEBUG_RAW( cc::debug( "Channel " ) + cc::note( m_serialSettings.controlTTY ) +
                              cc::debug( " ins't accessible, reason: " ) + cc::note( std::to_string( result ) ) );
        }

        return result == 0;
    }

    return false;
}

// FIXME: EFWR-3770: Can return response to multiple commands, not only the latest one
std::string ATExecutor::execute( const std::string& command, const std::chrono::milliseconds& waitForAnswer ) {
    std::vector< char > buff( 1024 );

    LOGGER_TRACE_RAW( cc::trace( "Execute " ) + cc::note( Utils::purgeCRSymbol( command ) ) );
    std::lock_guard< std::timed_mutex > lock( m_mutex );
    if( m_locked ) {
        return "ERROR";
    }

    LOGGER_TRACE_RAW( cc::trace( "Open a serial device" ) );
    Uart uart( m_serialSettings.controlTTY, true, m_serialSettings.hwFlowControl, true );

    LOGGER_TRACE_RAW( cc::trace( "Write to a serial device..." ) );
    if( uart.write( command.c_str(), command.size() ) < 0 ) {
        throw ATExecutorException( "Failed to send a command: " + Utils::purgeCRSymbol( command ) );
    }

    if( waitForAnswer == std::chrono::milliseconds( 0 ) ) {
        return "";
    }

    /* given this delay we can read the whole answer even if we use a non-blocking read */
    std::this_thread::sleep_for( waitForAnswer );

    LOGGER_TRACE_RAW( cc::trace( "Read from a serial device..." ) );
    auto readCount = uart.read( &buff[ 0 ], buff.size() );
    if( readCount < 0 ) {
        throw ATExecutorException( "Failed to read an answer for a command: " + Utils::purgeCRSymbol( command ) );
    }

    return std::string( buff.begin(), buff.begin() + readCount );
}

std::string ATExecutor::consume() {
    std::vector< char > buff( 1024 );

    std::lock_guard< std::timed_mutex > lock( m_mutex );
    Uart uart( m_serialSettings.controlTTY, true, m_serialSettings.hwFlowControl, true );
    auto readCount = uart.read( &buff[ 0 ], buff.size() );
    if( readCount < 0 && errno != EWOULDBLOCK && errno != EAGAIN ) {
        throw ATExecutorException( "Failed to consume result from at port" );
    }

    if( readCount < 0 ) {
        return "";
    }

    return std::string( buff.begin(), buff.begin() + readCount );
}

bool ATExecutor::isInitialized() const {
    return m_isInitialized;
}

namespace ATUtils {

std::string strip( const std::string& str ) {
    if( str.empty() )
        return {};

    size_t posBeg = 0;
    size_t posEnd = str.size();

    for( auto it = str.begin(); it != str.end() && *it <= 0x20; ++it ) {
        ++posBeg;
    }

    for( auto it = str.rbegin(); it != str.rend() && *it <= 0x20; ++it ) {
        --posEnd;
    }

    if( posBeg >= posEnd )
        return {};

    return str.substr( posBeg, posEnd - posBeg );
}

std::vector< std::string > split( const std::string& str, const std::string& delim ) {
    if( str.empty() )
        return {};

    if( delim.empty() )
        return { str };

    std::vector< std::string > result;
    result.reserve( 2 );
    size_t posBeg = 0;
    size_t posEnd = str.find( delim, posBeg );

    while( posEnd != std::string::npos ) {
        result.emplace_back( str.substr( posBeg, posEnd - posBeg ) );
        posBeg = posEnd + delim.size();
        posEnd = str.find( delim, posBeg );
    }
    result.emplace_back( str.substr( posBeg ) );
    return result;
}

std::vector< std::string > splitSequence( const std::string& str, const std::vector< std::string >& delims ) {
    if( str.empty() || delims.empty() )
        return {};

    std::vector< std::string > result;
    result.reserve( delims.size() + 1 );

    size_t posBeg = 0;
    size_t posEnd = 0;
    for( auto& delim : delims ) {
        if( delim.empty() )
            continue;

        posEnd = str.find( delim, posBeg );
        if( posEnd == std::string::npos )
            return {};

        result.emplace_back( str.substr( posBeg, posEnd - posBeg ) );
        posBeg = posEnd + delim.size();
    }
    result.emplace_back( str.substr( posBeg ) );
    return result;
}

std::vector< std::string > toStrings( const std::string& str ) {
    std::vector< std::string > result;

    size_t posBeg = 0;
    size_t posEnd = 0;

    while( posEnd != std::string::npos ) {
        posEnd = str.find( '\n', posBeg );
        auto substr = strip( str.substr( posBeg, posEnd - posBeg ) );
        if( !substr.empty() )
            result.emplace_back( std::move( substr ) );
        posBeg = posEnd + 1;
    }
    return result;
}

bool isOK( const std::vector< std::string >& answer, size_t expectedLineNum ) {
    if( expectedLineNum == 0 || answer.size() != expectedLineNum )
        return false;
    return answer.back() == "OK";
}

bool isOK( size_t expectedLineNum, const std::vector< std::string >& answer ) {
    if( answer.size() <= expectedLineNum )
        return false;
    return answer.at( expectedLineNum ) == "OK";
}

bool isValidCCID( const std::string& value ) {
    const char* const SIM_CCID_PREFIX = "89";
    bool lengthCheck = value.size() >= 2 && value.find( SIM_CCID_PREFIX ) == 0;
    if( lengthCheck ) {
        auto xDigitsNumber =
            std::count_if( value.begin(), value.end(), []( unsigned char c ) { return std::isxdigit( c ); } );

        return xDigitsNumber == value.size();
    }
    return false;
}

std::pair< std::string, bool > AtResponse::extractATParameter( const std::vector< std::string >& atAnswer,
                                                               const std::string& key ) {
    if( atAnswer.empty() || key.empty() )
        return { "", false };

    if( !ATUtils::isOK( atAnswer, atAnswer.size() ) )
        return { std::string{}, false };

    // Format of a parameter in AT command response is: "+<Key>"
    std::string searchKey = ( key.at( 0 ) != '+' ) ? "+" + key : key;

    for( const auto& one : atAnswer ) {
        if( one.find( searchKey ) == 0 ) {
            auto keyValue = ATUtils::split( one, " " );
            if( keyValue.size() >= 2 ) {
                return { keyValue[ 1 ], true };
            }
        }
    }
    return { "", false };
}

std::pair< std::string, bool > AtResponse::extractATParameter( const std::string& atAnswer, const std::string& key ) {
    if( atAnswer.empty() || key.empty() )
        return { "", false };

    auto strings = ATUtils::toStrings( atAnswer );
    return extractATParameter( strings, key );
}
}  // namespace ATUtils

}  // namespace GSM
}  // namespace Network
}  // namespace HubPlatform
