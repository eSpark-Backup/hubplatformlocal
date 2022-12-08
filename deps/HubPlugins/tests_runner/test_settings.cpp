#include "test_settings.h"

#include <stdlib.h>
#include <iostream>

bool TestSettings::applyFilters(const std::string& str) const {
    return str.find(m_filter) != std::string::npos && 
           (!m_useRegexFilter || std::regex_match(str, m_regexFilter));
}

LogLevel TestSettings::getLogLevel() const {
    return m_logLevel;
}

const std::string& TestSettings::getPlatform() const {
    return m_platform;
}

unsigned int TestSettings::getHardwareRevision() const {
    return m_hardwareRevision;
}

bool TestSettings::setFilter(const char* filter) {
    m_filter = filter;
    return true;
}

bool TestSettings::setRegexFilter(const char* regexFilter) {
    try {
        m_regexFilter = std::regex(regexFilter);
        m_useRegexFilter = *regexFilter != '\0';
        return true;
    } catch (const std::regex_error&) {
        std::cerr << "wrong regex filter " << regexFilter << std::endl;
        return false;
    }
}

bool TestSettings::setLogLevel(const char* logLevel) {
    static const auto namesToEnums = {
        std::make_pair("trace", LogLevel::Trace),
        std::make_pair("info", LogLevel::Info),
        std::make_pair("debug", LogLevel::Debug),
        std::make_pair("warning", LogLevel::Warning)
    };

    const auto found = std::find_if(namesToEnums.begin(), namesToEnums.end(), [logLevel](const auto& nameToEnum)
    {
        return *nameToEnum.first == *logLevel && ( logLevel[1] == '\0' || strcmp(logLevel + 1, nameToEnum.first + 1) == 0 );
    });

    if ( found != namesToEnums.end() ) {
        m_logLevel = found->second;
        return true;
    }

    return *logLevel == '\0';
}

bool TestSettings::setPlatform(const char* platform) {
    m_platform = platform;
    return true;
}

bool TestSettings::setHardwareRevision(const char* revision) {
    char* end = nullptr;
    const auto hardwareRevision = strtoul(revision, &end, 0);

    const auto result = end != nullptr && *end == '\0';

    if (result)
        m_hardwareRevision = hardwareRevision;

    return result;
}