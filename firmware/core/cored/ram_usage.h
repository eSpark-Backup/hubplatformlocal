#pragma once

#include <map>
#include <vector>

namespace HubPlatform {
namespace CoreD {

struct MemoryInfo {
    MemoryInfo()
    : memTotalKib( 0 )
    , memTotalMib( 0 )
    , memAvailableMib( 0 )
    , memAvailablePercent( 0 ){};
    int parseMeminfo();
    bool hasFlashInfo();
    bool hasProcessInfo();

private:
    void calculateVolumeSize( const struct mntent* fs );
    bool getProcessMemoryUsage( const char* d_name );
    void processDir( const struct dirent* dir );

public:
    // Values from /proc/meminfo, in KiB or converted to MiB.
    long memTotalKib;
    int memTotalMib;
    int memAvailableMib;  // -1 means no data available

    // Calculated percentages
    int memAvailablePercent;  // percent of total memory that is available

    std::map< std::string, std::vector< unsigned long > > flashDirectories;
    std::vector< unsigned long > flashUsage;
    std::multimap< std::string, int > processList;
};

}  // namespace CoreD
}  // namespace HubPlatform
