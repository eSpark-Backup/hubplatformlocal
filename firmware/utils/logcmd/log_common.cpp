#include "log_common.h"

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <thread>

#include "common_utils/firmware_info_helper.h"
#include "common_utils/hub_info_helper.h"
#include "log_utils/ezlo_log.h"
#include <tiny/tiny_consolecolors.h>

#include "logcmd/filesystem/filesystem.h"
#include "logcmd/log_exceptions.h"

namespace HubPlatform {
namespace LogCmd {

std::string getHubSerial() {
    return HubInfoHelper::getSerial();
}

std::string generateTimeString( std::time_t timestamp ) {
    std::time_t time = timestamp != 0 ? timestamp : std::time( nullptr );
    std::tm* tm = std::localtime( &time );
    if( tm ) {
        std::vector< char > buffer( 30, 0 );
        strftime( buffer.data(), buffer.size(), "%Y.%m.%d_%H.%M", tm );
        return std::string( buffer.data() );
    }
    return {};
}

std::string generateArchiveName( const std::string userString, std::time_t timestamp ) {
    auto deviceSerial = getHubSerial();
    auto firmwareVersion = FirmwareInfoHelper::getFirmwareVersionString();

    std::stringstream ss;
    ss << deviceSerial << "_ver-" << firmwareVersion;

    auto date = generateTimeString( timestamp );
    if( !date.empty() )
        ss << "_date-" << date;

    if( !userString.empty() )
        ss << "_" << userString;

    ss << "_logs.tgz";
    return ss.str();
}

std::array< std::string, 5 > generateCronSchedule( size_t interval, size_t seed ) {
    const size_t minutesInHour = 60;
    const size_t hoursInDay = 24;

    std::string minutes = std::to_string( seed % minutesInHour );

    std::string hours;
    if( interval <= 1 ) {
        hours = "*/1";
    }
    else if( interval > 12 ) {
        LOGGER_WARN_RAW( cc::warn( "Intervals > 12 will be triggered once per day at fixed time" ) );
        hours = std::to_string( ( interval + seed ) % hoursInDay );
    }
    else if( interval > 1 ) {
        const size_t intervalsInDay = hoursInDay / interval;

        std::vector< size_t > hoursList;
        hoursList.reserve( intervalsInDay );

        for( size_t i = 0; i < intervalsInDay; ++i ) {
            size_t intervalTime = i * interval;
            intervalTime = ( intervalTime + seed ) % hoursInDay;
            hoursList.emplace_back( intervalTime );
        }

        std::sort( hoursList.begin(), hoursList.end() );
        for( auto& time : hoursList )
            hours += std::to_string( time ) + ",";
        hours.pop_back();
    }

    std::array< std::string, 5 > schedule;
    schedule.fill( "*" );
    schedule[ 0 ] = minutes;
    schedule[ 1 ] = hours;
    return schedule;
}

void updateSystemLogs( const std::string& logDirectory ) {
    const std::string headerCmd = "echo \"==================== $(date +%Y.%m.%d_%H.%M) ====================\"";

    Filesystem filesystem;
    if( !filesystem.isDirectoryExist( logDirectory ) )
        throw LogDirectoryDoesNotExistError( "Log directory doesn't exist" );

    auto fullPath = [&logDirectory]( const std::string& logName ) {
        const auto auxSlash = ( logDirectory.back() != '/' ) ? "/" : "";
        return logDirectory + auxSlash + logName;
    };

    auto updateLog = [&fullPath, &headerCmd]( const std::string& logName, const std::string& cmd ) {
        auto logPath = fullPath( logName );
        std::system( ( headerCmd + " > " + logPath ).c_str() );
        std::system( ( cmd + " >> " + logPath ).c_str() );
    };

    updateLog( "logread.log", "logread" );
    updateLog( "storage.log", "df -h" );
    updateLog( "top.log", "top -b -n 1" );

    updateLog( "ifconfig.log", "ifconfig" );
    updateLog( "route.log", "echo -e \"$(route -A inet)\\n\\n\\n$(route -A inet6)\"" );
    updateLog( "network_config.log", "cat /etc/config/network" );
}

bool waitForCondition( std::function< bool() > condition,
                       const std::chrono::milliseconds& timeout,
                       const std::chrono::milliseconds& checkInterval ) {
    const auto start = std::chrono::steady_clock::now();
    while( std::chrono::steady_clock::now() - start < timeout ) {
        if( condition() )
            return true;
        std::this_thread::sleep_for( checkInterval );
    }
    return false;
}

}  // namespace LogCmd
}  // namespace HubPlatform
