#pragma once

#include <string>
#include <deque>
#include <memory>

namespace HubPlatform {
namespace LogCmd {

class LogFile;

using LogFiles = std::deque< std::shared_ptr< LogFile > >;

class LogFile {
public:
    enum class LogType { Regular, Rotated, Archive };

    static std::shared_ptr< LogFile > makeLogFile( std::string dirName, std::string fileName );

    LogType type() const;
    std::string path() const;
    const std::string& dirName() const;
    const std::string& fileName() const;
    const std::string& logName() const;
    size_t partNum() const;

private:
    LogType m_type = LogType::Regular;
    std::string m_dirName;
    std::string m_fileName;
    std::string m_logName;
    size_t m_partNum = 0;
};

}  // namespace LogCmd
}  // namespace HubPlatform
