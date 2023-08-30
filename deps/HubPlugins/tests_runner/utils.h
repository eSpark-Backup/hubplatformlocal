#pragma once

#include <string>

void setHubPluginsDir(std::string pluginsDir);

std::string getBaseDir();
std::string getGatewaysDir();
std::string getLibrariesDir();
std::string getPluginDir(const char* plugin);
std::string getTestsRunnerDir();

bool isFileExist(const char* fileName);
bool isFolderExist(const char* path);

