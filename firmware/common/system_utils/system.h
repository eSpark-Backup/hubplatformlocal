#pragma once

#include <string>
#include <stdexcept>

namespace HubPlatform {
namespace System {

class SystemError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class PopenFailed : public SystemError {
public:
    explicit PopenFailed( const std::string& message );
};

class UntarFailed : public SystemError {
public:
    explicit UntarFailed( const std::string& message );
};

struct USBDescription {
    std::string idVendor;
    std::string idProduct;
};

std::pair< std::string, int > getCommandOutput( const char* command,
                                                bool catchStderr = false,
                                                bool stderrToDevNull = true );
std::string getAwkOutput( const std::string& awkCommand );
std::string getAwkOutput( const char* awkCommand );

USBDescription getUSBDescription( const std::string& usbDevicePath );
std::string formatTimeoutCommand( const std::string& command, int timeoutSeconds );

void untarArchive( const std::string& archivePath, const std::string& directoryToExtractPath = "" );
bool tarArchive( const std::string& path, const std::string& archivePath );

// Memory info in bytes
struct MemoryInfo {
    uint64_t totalram = 0;
    uint64_t freeram = 0;
    uint64_t sharedram = 0;
    uint64_t bufferram = 0;
    size_t pagesize = 0;
};

MemoryInfo getMemoryInfo();
size_t getMemoryPageSize();

void dropVirtualMemoryCaches();

}  // namespace System
}  // namespace HubPlatform
