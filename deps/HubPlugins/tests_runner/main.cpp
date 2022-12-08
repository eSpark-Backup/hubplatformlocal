#include "lua_wrapper.h"
#include "utils.h"
#include "test_settings.h"
#include "time_meter.h"

#include <json.hpp>

#include <iostream>

#include <fstream>
#include <vector>
#include <unordered_map>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace {

using TestCase = std::string;

struct TestSuite {
    std::string name;
    std::string relDirectory;
    std::string setUpScript;
    std::vector< TestCase > testCases;
};

struct PluginTest {
    enum class Status
    {
        Ok,
        DirectoryAbsent,
        ConfigReadingError,
        SkippedByFilter
    };

    std::vector< TestSuite > testSuites;
    Status status = Status::Ok;
    std::string name;
    std::string path;
};

struct TestsRunningInfo {
    std::vector<std::string> failedTestCases;
};

std::vector< TestSuite > readTestConfig(bool isPluginFilterPassed, const TestSettings& testSettings, const std::string& fileName) {
    std::ifstream stream( fileName );

    if (!stream.good()) {
        throw std::runtime_error( std::string("Fail to open config ") + fileName );
    }

    const auto config = nlohmann::json::parse( stream );
    stream.close();

    std::vector< TestSuite > result;

    const auto& testSuitesJson = config.at( "test_suites" );
    result.reserve( testSuitesJson.size() );

    for( const auto& testSuiteJson : testSuitesJson ) {
        TestSuite testSuite;
        testSuite.name = testSuiteJson.at( "name" ).get< std::string >();

        const auto isTestSuiteFilterPassed = isPluginFilterPassed || testSettings.applyFilters(testSuite.name);

        auto directoryIt = testSuiteJson.find( "directory" );
        if ( directoryIt != testSuiteJson.end() )
            testSuite.relDirectory = directoryIt->get< std::string >() + "/";

        auto setUpScriptIt = testSuiteJson.find( "setup_script" );
        if ( setUpScriptIt != testSuiteJson.end() )
            testSuite.setUpScript = setUpScriptIt->get< std::string >();

        const auto& testCasesJson = testSuiteJson.at( "test_cases" );

        testSuite.testCases.reserve( testCasesJson.size() );

        for (std::string testCase : testCasesJson) {

            if (isTestSuiteFilterPassed || testSettings.applyFilters(testCase))
                testSuite.testCases.push_back(std::move(testCase));
        }

        if (isTestSuiteFilterPassed || !testSuite.testCases.empty())
            result.push_back(std::move(testSuite));
    }

    return result;
}

PluginTest getPluginTest(const TestSettings& testSettings, const fs::path& testPath, const std::string& pluginTestName) {
    PluginTest result;

    result.path = testPath;
    result.name = pluginTestName;
    const auto isPluginFilterPassed = testSettings.applyFilters(pluginTestName);

    if (fs::is_directory( testPath )) {
        try {
            result.testSuites = readTestConfig(isPluginFilterPassed, testSettings, result.path + "test_config.json");
            result.status = isPluginFilterPassed || !result.testSuites.empty() ?
                PluginTest::Status::Ok : PluginTest::Status::SkippedByFilter;
        } catch (const std::exception& ex) {
            std::cerr << "Config reading error: " << ex.what() << std::endl;
            result.status = PluginTest::Status::ConfigReadingError;
        }
    }
    else {
        result.status = isPluginFilterPassed ?
            PluginTest::Status::DirectoryAbsent : PluginTest::Status::SkippedByFilter;
    }

    return result;
} 

bool runTestCase(const std::string& setUpScript,
                 const TestSettings& settings,
                 const PluginTest& pluginTest,
                 const TestSuite& testSuite,
                 const TestCase& testCaseName,
                 TestsRunningInfo& testsRunningInfo) {
    const auto fullName = pluginTest.name + "." + testSuite.name + "." + testCaseName;
    std::cout << "[ RUN     ] " << fullName << std::endl;

    const TimeMeter timeMeter; 

    LuaWrapper lua(settings.getPlatform(), settings.getHardwareRevision(), settings.getLogLevel());

    bool result = setUpScript.empty() || lua.runScript(setUpScript.c_str());

    if (result)
        result = lua.runScript((pluginTest.path + testSuite.relDirectory + testCaseName + ".lua").c_str());
    else 
        std::cout << "setup_script failed" << std::endl;

    std::cout << (result ? "[      OK ] " : "[     FAIL ] ") << fullName << " (" << timeMeter.getElapsedTime().count() << " ms)" <<  std::endl;

    if (!result) {
        testsRunningInfo.failedTestCases.push_back(fullName);
    }

    return result;
}

bool runTestSuite(const TestSettings& settings, const PluginTest& pluginTest, const TestSuite& testSuite, TestsRunningInfo& testsRunningInfo) {
    size_t successCount = 0;
    const auto fullName = pluginTest.name + "." + testSuite.name;

    std::cout << "\n[ RUN TEST SUITE    ] " << fullName << std::endl;

    const TimeMeter timeMeter;

    std::string setUpScript;
    bool setUpScriptOk = true;

    if ( !testSuite.setUpScript.empty() ) {
        auto checkSetUpScript = [&setUpScript](std::string script) {
            if (!isFileExist(script.c_str()))
                return false;
            setUpScript = std::move(script);
            return true;
        };

        setUpScriptOk = checkSetUpScript(pluginTest.path + testSuite.relDirectory + testSuite.setUpScript + ".lua") ||
                        checkSetUpScript(pluginTest.path + testSuite.setUpScript + ".lua");

        if (!setUpScriptOk)
            std::cout << "setup_script " << testSuite.setUpScript << " not found" << std::endl;
    }

    if ( setUpScriptOk ) {
        for( const auto& testCase : testSuite.testCases ) {
            successCount += runTestCase(setUpScript, settings, pluginTest, testSuite, testCase, testsRunningInfo);
        }
    }

    std::cout << "[ FINISH TEST SUITE ] " << fullName
              << " SUCCEEDED: " << successCount
              << " FAILED: " << testSuite.testCases.size() - successCount
              << ". (" << timeMeter.getElapsedTime().count() << " ms total)" << std::endl;

    return successCount == testSuite.testCases.size();
}

bool runPluginTest(const TestSettings& settings, const PluginTest& pluginTest, TestsRunningInfo& testsRunningInfo) {
    std::cout << "[ RUN PLUGIN TESTS ] " << pluginTest.name << std::endl <<
                 "Config reading..." << std::endl;


    size_t successCount = 0;

    for( const auto& testSuite : pluginTest.testSuites ) {
        successCount += runTestSuite(settings, pluginTest, testSuite, testsRunningInfo);
    }

    std::cout << "[ FINISH PLUGIN TESTS ] " << pluginTest.name
              << " SUCCEEDED: " << successCount
              << " FAILED: " << pluginTest.testSuites.size() - successCount  << std::endl << std::endl;

    return successCount == pluginTest.testSuites.size();
}

bool parseParams(int argc, char *argv[], TestSettings& testSettings) {
    static const std::unordered_map<std::string, bool(TestSettings::*)(const char*)> paramsMap = {
        { "filter", &TestSettings::setFilter },
        { "regex_filter", &TestSettings::setRegexFilter },
        { "log_level", &TestSettings::setLogLevel },
        { "platform", &TestSettings::setPlatform },
        { "hardware_revision", &TestSettings::setHardwareRevision }
    };

    for (int i = 1; i < argc; ++i) {
        bool paramParsingResult = false;

        if (argv[i][0] == '-' && argv[i][1] == '-') {
            if ( auto separator = strchr( argv[i] + 2, '=' ) ) {
                const auto found = paramsMap.find( std::string(argv[i] + 2, separator) );
                if ( found != paramsMap.end() ) {
                    paramParsingResult = separator[1] == '\0' || (testSettings.*found->second)( separator + 1 );
                }
            }
        }

        if (!paramParsingResult) {
            std::cerr << "Cannot parse param " << argv[i] << std::endl;
            return false;
        }
    }

    return true;
}

}

int main(int argc, char *argv[])
{
    const auto gatewaysDir = getGatewaysDir();
    if (!fs::is_directory(gatewaysDir)) {
        std::cerr << "Gateway dir is not found. Test runner must be run from root of HubPlugins" << std::endl;
        return -1;
    }

    TestSettings testSettings;

    if (!parseParams(argc, argv, testSettings))
        return -1;

    if ( !LuaWrapper(testSettings.getPlatform(), testSettings.getHardwareRevision(), testSettings.getLogLevel()).runCustomLuaApiTest() ) {
        std::cerr << "Custom cpp test failed!" << std::endl;
        return -1;
    }

    const TimeMeter timeMeter;

    int failsCount = 0;
    TestsRunningInfo testsRunningInfo;

    for (const auto& path : fs::directory_iterator( getGatewaysDir() )) {
        const auto pluginTest = getPluginTest(testSettings, path / fs::path("scripts/tests/"), path.path().filename());

        switch (pluginTest.status) {
            case PluginTest::Status::Ok:
                failsCount += !runPluginTest(testSettings, pluginTest, testsRunningInfo);
                break;
            case PluginTest::Status::DirectoryAbsent:
                std::cout << "skip plugin " << pluginTest.name << std::endl;
                break;
            case PluginTest::Status::ConfigReadingError:
                ++failsCount;
                break;
            default:
                break;
        }
    }

    if (!testsRunningInfo.failedTestCases.empty()) {
        std::cout << "\nFailed test cases:" << std::endl;
        for ( const auto& name : testsRunningInfo.failedTestCases ) {
            std::cout << name << std::endl;
        }
    }

    std::cout << "\n" << ( failsCount ? "[    FAIL ] " : "[  SUCCESS ] " )
              << " (" << timeMeter.getElapsedTime().count() << " ms total)" << std::endl;

    return failsCount;
}
