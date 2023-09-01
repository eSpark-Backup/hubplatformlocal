#include "utils.h"

#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

static std::string PluginsDir = "./";

void setHubPluginsDir(std::string pluginsPath) {
    if (!pluginsPath.empty() && pluginsPath.back() != '/')
        pluginsPath += '/';
    PluginsDir = std::move(pluginsPath);
}

std::string getBaseDir() {
    return PluginsDir;
}

std::string getGatewaysDir() {
    return PluginsDir + "gateways/";
}

std::string getLibrariesDir() {
    return PluginsDir + "libraries/";
}

std::string getPluginDir(const char* plugin) {
    return getGatewaysDir() + plugin + "/";
}

std::string getTestsRunnerDir() {
    return getBaseDir() + "tests_runner/";
}

bool isFileExist(const char* fileName) {
    std::ifstream file(fileName);
    return file.good();
}

bool isFolderExist(const char* path) {
    struct stat info;
    auto result = stat( path, &info );
    return result == 0 && (info.st_mode & S_IFDIR);
}
