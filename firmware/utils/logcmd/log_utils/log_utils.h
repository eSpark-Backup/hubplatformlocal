#pragma once

#include <map>
#include <vector>
#include <string>

#include "logcmd/log_file.h"
#include "logcmd/filesystem/filesystem_interface.h"

namespace HubPlatform {
namespace LogCmd {
namespace LogUtils {

using Filename = std::string;

LogFiles findLogs( IFilesystem& filesystem, const std::string& directoryPath );
LogFiles findRegularLogs( IFilesystem& filesystem, const std::string& directoryPath );
LogFiles findRotatedLogs( IFilesystem& filesystem, const std::string& directoryPath );
LogFiles findArchivedLogs( IFilesystem& filesystem, const std::string& directoryPath );
LogFiles findSelectedLogs( IFilesystem& filesystem,
                           const std::string& directoryPath,
                           const std::vector< std::string >& names );

void rotateArchivesByNum( IFilesystem& filesystem, const std::string directoryPath, size_t numLimit );
void rotateArchivesBySize( IFilesystem& filesystem, const std::string directoryPath, size_t sizeLimit );
void rotatePendingLogsBySize( IFilesystem& filesystem, const std::string directoryPath, size_t sizeLimit );

void deleteLogFiles( IFilesystem& filesystem, const LogFiles& logFiles );
size_t getLogsSize( IFilesystem& filesystem, const LogFiles& logFiles );

using PartitionMap = std::map< Filename, size_t >;
PartitionMap buildPartitionMap( const LogFiles& logFiles );
void partitionLogs( IFilesystem& filesystem,
                    const LogFiles& srcLogs,
                    const std::string& dstDir,
                    const LogFiles& existingLogs );
LogFiles joinLogFiles( const LogFiles& listA, const LogFiles& listB );

void createArchive( const std::string& name, const LogFiles& logFiles );

}  // namespace LogUtils
}  // namespace LogCmd
}  // namespace HubPlatform
