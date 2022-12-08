#include "cored/ram_usage.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>

#include <errno.h>
#include <stddef.h>

#include "log_utils/ezlo_log.h"
#include <tiny/tiny_consolecolors.h>

#include <fcntl.h>
#include <fnmatch.h>

#include "linux/limits.h"

//#include <mntent.h>	/* for getmntent(), et al. */

namespace HubPlatform {
namespace CoreD {

#include <mntent.h> /* for getmntent(), et al. */
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <dirent.h>

long getEntry( const char* name, const char* buf ) {
    const char* hit;
    hit = strstr( buf, name );
    if( hit == NULL ) {
        return -1;
    }

    errno = 0;
    long val = strtol( hit + strlen( name ), NULL, 10 );
    if( errno != 0 ) {
        Log::error( cc::error( "get_entry: strtol() failed" ) );
        return -1;
    }
    return val;
}

/* Like get_entry(), but exit if the value cannot be found */
long getEntryFatal( const char* name, const char* buf ) {
    long val = getEntry( name, buf );
    if( val == -1 ) {
        Log::error( cc::error( "could not find entry '%s' in /proc/meminfo" ) );
    }
    return val;
}

long calculateAvailableGuesstimate( const char* buf ) {
    long cached = getEntryFatal( "Cached:", buf );
    long memFree = getEntryFatal( "MemFree:", buf );
    long buffers = getEntryFatal( "Buffers:", buf );
    long shmem = getEntryFatal( "Shmem:", buf );

    return ( memFree + cached + buffers - shmem );
}

int MemoryInfo::parseMeminfo() {
    static FILE* fd;
    static char buf[ 8192 ];
    static int guesstimateWarned = 0;
    if( fd == NULL )
        fd = fopen( "/proc/meminfo", "r" );
    if( fd == NULL ) {
        Log::error( cc::error( "could not open /proc/meminfo: %s" ) );
        fclose( fd );
        return 0;
    }
    rewind( fd );

    size_t len = fread( buf, 1, sizeof( buf ) - 1, fd );
    if( len == 0 ) {
        Log::error( cc::error( "could not read /proc/meminfo: %s" ) );
        return 0;
    }
    buf[ len ] = 0;  // Make sure buf is zero-terminated

    this->memTotalKib = getEntryFatal( "MemTotal:", buf );

    long memAvailable = getEntry( "MemAvailable:", buf );
    if( memAvailable == -1 ) {
        memAvailable = calculateAvailableGuesstimate( buf );
        if( guesstimateWarned == 0 ) {
            Log::error( cc::error( "Warning: Your kernel does not provide MemAvailable data (needs 3.14+)" ) );
            guesstimateWarned = 1;
        }
    }

    // Calculate percentages
    this->memAvailablePercent = memAvailable * 100 / this->memTotalKib;

    // Convert kiB to MiB
    this->memTotalMib = this->memTotalKib / 1024;
    this->memAvailableMib = memAvailable / 1024;

    fclose( fd );
    return 1;
}

bool MemoryInfo::hasFlashInfo() {
    constexpr char filename[] = "/etc/mtab";
    struct mntent* fs;

    FILE* fp = setmntent( filename, "r" ); /* read only */
    if( fp == NULL ) {
        Log::error( cc::error( "Could not open file for flash memory cheking" ) );
        endmntent( fp );
        return false;
    }

    while( ( fs = getmntent( fp ) ) != NULL )
        calculateVolumeSize( fs );

    endmntent( fp );
    return true;
}

/* calculateVolumeSize --- Use statvfs and fill info */

void MemoryInfo::calculateVolumeSize( const struct mntent* fs ) {
    struct statvfs vfs;

    if( fs->mnt_fsname[ 0 ] != '/' ) /* skip nonreal filesystems */
        return;

    if( statvfs( fs->mnt_dir, &vfs ) != 0 ) {
        Log::error( cc::error( "statvfs failed" ) );
        return;
    }

    unsigned long totalFlash = vfs.f_blocks * vfs.f_frsize / 1024;
    unsigned long availableFlash = vfs.f_bavail * vfs.f_frsize / 1024;
    unsigned long freeFlash = vfs.f_bfree * vfs.f_frsize / 1024;
    unsigned long usedFlash = ( totalFlash ) - ( freeFlash );

    // Calculate % used based on f_bavail not f_bfree.
    unsigned long availableFlashPercent = ( ( vfs.f_blocks - vfs.f_bavail ) / (double) ( vfs.f_blocks ) * 100.0 );

    flashUsage.push_back( totalFlash );
    flashUsage.push_back( availableFlash );
    flashUsage.push_back( freeFlash );
    flashUsage.push_back( usedFlash );
    flashUsage.push_back( availableFlashPercent );

    flashDirectories.insert(
        std::pair< std::string, std::vector< unsigned long > >( std::string( fs->mnt_fsname ), flashUsage ) );
}

bool MemoryInfo::getProcessMemoryUsage( const char* d_name ) {
    int fd = 0, data = 0, stack = 0;
    char buf[ 4096 ], status_child[ NAME_MAX ];
    char nameproc[ 100 ];
    char* vm;

    sprintf( status_child, "/proc/%s/status", d_name );
    if( ( fd = open( status_child, O_RDONLY ) ) < 0 )
        return false;

    read( fd, buf, 4095 );
    buf[ 4095 ] = '\0';
    close( fd );

    data = stack = 0;

    vm = strstr( buf, "Name:" );
    if( vm ) {
        sscanf( vm, "%*s %s", nameproc );
    }
    vm = strstr( buf, "VmData:" );
    if( vm ) {
        sscanf( vm, "%*s %d", &data );
    }
    vm = strstr( buf, "VmStk:" );
    if( vm ) {
        sscanf( vm, "%*s %d", &stack );
    }

    this->processList.insert( std::pair< std::string, int >( std::string( nameproc ), ( data + stack ) ) );
    return true;
}

void MemoryInfo::processDir( const struct dirent* dir ) {
    puts( dir->d_name );
    getProcessMemoryUsage( dir->d_name );
}

int filter( const struct dirent* dir ) {
    uid_t user;
    struct stat dirinfo;
    int len = strlen( dir->d_name ) + 7;
    char path[ len ];

    strcpy( path, "/proc/" );
    strcat( path, dir->d_name );
    user = getuid();
    if( stat( path, &dirinfo ) < 0 ) {
        Log::error( cc::error( "processdir() ==> stat()" ) );
        return 0;
    }
    return !fnmatch( "[1-9]*", dir->d_name, 0 ) && user == dirinfo.st_uid;
}

bool MemoryInfo::hasProcessInfo() {
    struct dirent** namelist;
    int n = 0;

    n = scandir( "/proc", &namelist, filter, 0 );
    if( n < 0 ) {
        Log::error( cc::error( "Not enough memory." ) );
        return false;
    }
    else {
        while( n-- ) {
            processDir( namelist[ n ] );
            free( namelist[ n ] );
        }
        free( namelist );
    }
    return true;
}

}  // namespace CoreD
}  // namespace HubPlatform
