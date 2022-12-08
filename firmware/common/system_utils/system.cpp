#include "system_utils/system.h"
#include "system_utils/logger.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include <functional>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <unistd.h>

extern "C" {
#include <sys/sysinfo.h>
#include <sys/mount.h>
}

namespace HubPlatform {
namespace System {

PopenFailed::PopenFailed( const std::string& message )
: SystemError( message ) {}

UntarFailed::UntarFailed( const std::string& message )
: SystemError( message ) {}

std::pair< std::string, int > getCommandOutput( const char* command, bool catchStderr, bool stderrToDevNull ) {
    std::array< char, 128 > buffer;
    std::stringstream ss;
    int result = 0;
    {
        std::string commandWithRedirect( command ? command : "" );
        const char* redirect = "";

        if( catchStderr ) {
            redirect = " 2>&1";
        }
        else if( stderrToDevNull ) {  // use it if you have no stderr catching in a command passed
            redirect = " 2>/dev/null";
        }

        commandWithRedirect += redirect;
        command = commandWithRedirect.c_str();

        std::unique_ptr< FILE, std::function< void( FILE* ) > > pipe( popen( command, "r" ),
                                                                      [&]( FILE* file ) { result = pclose( file ); } );
        if( !pipe ) {
            SystemUtils::logError() << "popen() failed, command: '" << command << "', error: '" << strerror( errno )
                                    << "'\n";
            throw PopenFailed( std::string{ "popen() failed, error: " } + strerror( errno ) );
        }
        while( fgets( buffer.data(), buffer.size(), pipe.get() ) != nullptr ) {
            ss << buffer.data();
        }
    }
    return { std::piecewise_construct, std::forward_as_tuple( ss.str() ), std::forward_as_tuple( result ) };
}

std::string getAwkOutput( const std::string& awkCommand ) {
    return getAwkOutput( awkCommand.c_str() );
}
std::string getAwkOutput( const char* command ) {
    std::string result;
    std::tie( result, std::ignore ) = System::getCommandOutput( command );
    if( result.size() > 1 /*\n*/ ) {
        result.resize( result.size() - 1 );
    }
    return result;
}

USBDescription getUSBDescription( const std::string& usbDevicePath ) {
    std::ifstream idVendor( usbDevicePath + "/idVendor" );
    std::ifstream idProduct( usbDevicePath + "/idProduct" );
    USBDescription usbDescription;

    if( !idVendor.is_open() || !idProduct.is_open() ) {
        return {};
    }

    idVendor >> usbDescription.idVendor;
    idProduct >> usbDescription.idProduct;

    return usbDescription;
}

std::string formatTimeoutCommand( const std::string& command, int timeoutSeconds ) {
    std::string duration = std::to_string( timeoutSeconds ) + "s";
    std::string options = "--preserve-status -s 9 " + duration;  // SIGKILL == 9
    std::string safeCommand = "timeout " + options;
    return safeCommand + " " + command;
}

void untarArchive( const std::string& archivePath, const std::string& directoryToExtractPath ) {
    std::string command;
    std::string commandOutput;
    int result;

    if( directoryToExtractPath.empty() ) {
        command = "tar -xzf " + archivePath;
    }
    else {
        command = "tar -C " + directoryToExtractPath + " -xzf " + archivePath;
    }

    std::tie( commandOutput, result ) = getCommandOutput( command.c_str(), true, false );
    if( result != 0 ) {
        throw UntarFailed( commandOutput );
    }
}

MemoryInfo getMemoryInfo() {
    struct sysinfo info;
    auto ret = sysinfo( &info );
    if( ret != 0 )
        throw SystemError( "Unable to get memory info: " + std::string( std::strerror( errno ) ) );

    const size_t units = info.mem_unit;
    MemoryInfo memoryInfo;
    memoryInfo.totalram = info.totalram * units;
    memoryInfo.freeram = info.freeram * units;
    memoryInfo.sharedram = info.sharedram * units;
    memoryInfo.bufferram = info.bufferram * units;

    return memoryInfo;
}

size_t getMemoryPageSize() {
    return sysconf( _SC_PAGESIZE );
}

void dropVirtualMemoryCaches() {
    System::getCommandOutput( "sysctl vm.drop_caches=1" );
}

}  // namespace System
}  // namespace HubPlatform
