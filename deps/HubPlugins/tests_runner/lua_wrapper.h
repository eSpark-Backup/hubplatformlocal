#pragma once

#include "log_level.h"

#include <memory>

struct lua_State;

class LuaWrapper
{
public:
    LuaWrapper(const std::string& platform, unsigned int hardwareRevision, LogLevel logLevel);
    ~LuaWrapper();

    bool runCustomLuaApiTest();

    bool runScript(const char* fileName);

private:
    std::unique_ptr< lua_State, void (*)(lua_State*) > m_luaState;
};

