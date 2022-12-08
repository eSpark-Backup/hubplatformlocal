#pragma once

#include "log_level.h"

#include <regex>

class TestSettings {
public:
    bool applyFilters(const std::string& str) const;

    LogLevel getLogLevel() const;

    const std::string& getPlatform() const;

    unsigned int getHardwareRevision() const;

    bool setFilter(const char* filter);

    bool setRegexFilter(const char* regexFilter);

    bool setLogLevel(const char* regexFilter);

    bool setPlatform(const char* platform);

    bool setHardwareRevision(const char* revision);

private:
    std::string m_filter;
    std::regex m_regexFilter;
    bool m_useRegexFilter = false;
    LogLevel m_logLevel = LogLevel::Default;
    std::string m_platform;
    unsigned int m_hardwareRevision = 0;
};
