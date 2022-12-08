#include "lua_wrapper.h"

#include <string>
#include <string.h>
#include <fstream>
#include <exception>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <cassert>

#include <lua.hpp>
#include <json.hpp>

#include "utils.h"

namespace  {

constexpr char OriginalLoadfile[] = "original_loadfile";
constexpr char OriginalRequire[] = "original_require";

constexpr char TestModulesDir[] = "tests_runner/scripts/modules/";

using LuaTablePointersSet = std::unordered_set<const void*>;

void dump(lua_State* L, LuaTablePointersSet& tables, int index, int firstLevel = 0, int level = 0) {
    for ( int i = 0; i < firstLevel; ++i )
        std::cout << "    ";

    const auto type = lua_type(L, index);
    switch (type) {
        case LUA_TNIL:
            std::cout << "nil";
            break;
        case LUA_TBOOLEAN:
            std::cout << (lua_toboolean(L, index) ? "true" : "false");
            break;
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
            std::cout << lua_touserdata(L, index) << " (user data)";
            break;
        case LUA_TNUMBER:
            std::cout << lua_tonumber(L, index);
            break;
        case LUA_TSTRING:
            std::cout << "\"" << lua_tostring(L, index) << "\"";
            break;
        case LUA_TTABLE:
        {
            const auto pointer = lua_topointer(L, index);
            const auto insertResult = tables.insert(pointer);
            std::cout << pointer << " (table)";
            if (!insertResult.second)
                std::cout << "(cycled)";

            if (level >= 0) {
                std::cout << "\n";
                if ( insertResult.second ) {
                    lua_pushnil( L );
                    while( lua_next( L, index - 1 ) ) {
                        dump( L, tables, -2, level + 1, -1 );
                        std::cout << " -> ";
                        dump( L, tables, -1, 0, level + 1);
                        lua_pop(L, 1);
                    }
                }
            }

            tables.erase(insertResult.first);
            return;
        }
        case LUA_TFUNCTION:
            std::cout << lua_tocfunction(L, index) << " (function)";
            break;
        case LUA_TTHREAD:
            std::cout << lua_tothread(L, index) << " (thread)";
            break;
        default:
            std::cout << "(unexpected type)";
            break;
    };

    if (level >= 0)
        std::cout << "\n";
}

int report(lua_State* L, int status) {
    if (status != LUA_OK) {
        const char *msg = lua_tostring(L, -1);
        std::cout << msg << std::endl;

        lua_pop(L, 1);
    }
    return status;
}

int meassageHandler(lua_State* L) {
    const char *msg = lua_tostring(L, 1);
    if (msg == nullptr) {  /* is error object not a string? */
        if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
            lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
                return 1;  /* that is the message */
        else
            msg = lua_pushfstring(L, "(error object is a %s value)",
                                  luaL_typename(L, 1));
    }

    luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
    return 1;  /* return the traceback */
}


int doCall(lua_State* L, int narg, int nres) {
    int base = lua_gettop(L) - narg;  /* function index */
    lua_pushcfunction(L, meassageHandler);  /* push message handler */
    lua_insert(L, base);  /* put it under function and args */

    const auto status = lua_pcall(L, narg, nres, base);

    lua_remove(L, base);  /* remove message handler from the stack */
    return status;
}


int doChunk(lua_State* L, int status) {
    if (status == LUA_OK)
        status = doCall(L, 0, 0);
    return report(L, status);
}


int doFile(lua_State* L, const char *name) {
    return doChunk(L, luaL_loadfile(L, name));
}

int customRequireOrLoadFile(lua_State* L, const char* method, const char* originalMethod ) {
    const auto luaStackSize = lua_gettop( L );
    const auto name = luaL_checkstring( L, 1 );

    lua_getglobal( L, originalMethod );
    if( !lua_isfunction( L, -1 ) ) {
        throw std::runtime_error( std::string("fail to load an original ") + method );
    }

    static const auto DirGetters = {
        std::make_pair( std::string( "HUB:" ) , &getGatewaysDir ),
        std::make_pair( std::string( "HUB:" ) , &getLibrariesDir ),
        std::make_pair( std::string( "TEST:" ), &getTestsRunnerDir ),
    };

    auto pathNormalizer = [] (const char * const name, const std::string& prefix, const std::string& dirPath) {
        return ( dirPath + ( name + prefix.size() ) + ".lua" );
    };

    auto predicate = [name, pathNormalizer]( decltype (*DirGetters.begin()) prefixToGetter ) {
        bool hasSubstring = strncmp(name, prefixToGetter.first.c_str(), prefixToGetter.first.size() ) == 0;
        if( hasSubstring ) {
            return isFileExist(pathNormalizer(name, prefixToGetter.first, prefixToGetter.second()).c_str());
        }

        return false;
    };

    auto found = name ? std::find_if( DirGetters.begin(), DirGetters.end(), predicate ) : DirGetters.end();

    lua_pushstring( L, found != DirGetters.end() ?
                    pathNormalizer(name, found->first, found->second()).c_str() :
                    name );

    if ( lua_pcall( L, 1, LUA_MULTRET, luaStackSize ) != LUA_OK ) {
        lua_pop( L, 1 );
    }

    return lua_gettop( L ) - luaStackSize;
}

void overrideFunction(lua_State* L,
                      const char* functionName,
                      const char* savedFunctionName,
                      lua_CFunction customFunction) {
    if (savedFunctionName) {
        lua_getglobal( L, functionName );
        lua_setglobal( L, savedFunctionName );
    }

    lua_pushcfunction( L, customFunction );
    lua_setglobal( L, functionName );
}

void overrideOriginalRequire( lua_State* L ) {
    overrideFunction( L, "require", OriginalRequire, []( lua_State* L ) {
        return customRequireOrLoadFile( L, "require", OriginalRequire );
    });
}

void overrideOriginalLoadfile( lua_State* L ) {
    overrideFunction( L, "loadfile", OriginalLoadfile, []( lua_State* L ) {
        return customRequireOrLoadFile( L, "loadfile", OriginalLoadfile );
    });
}

int preloadSearcher(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
    if (lua_getfield(L, -1, name) == LUA_TNIL)  /* not found? */
        lua_pushfstring(L, "\n\tno field package.preload['%s']", name);
    return 1;
}

int luaSearcher (lua_State* L) {
    const char *name = luaL_checkstring(L, 1);

    if (name == nullptr || *name == '\0')
        return 1;  /* module not found in this path */

    auto load = [L, name](const char* filename)
    {
        if (luaL_loadfile(L, filename) == LUA_OK) {  /* module loaded successfully? */
            lua_pushstring(L, name);  /* will be 2nd argument to module */
            return 2;  /* return open function and file name */
        }
        else {
            return luaL_error(L, "error loading module '%s' from file '%s':\n\t%s",
                              lua_tostring(L, 1), filename, lua_tostring(L, -1));
        }
    };

    // Check can it be module name. We don't use module names with '.' or '/'. This rule can be changed.
    const auto isModule = strchr(name, '/') == nullptr && strrchr(name, '.') == nullptr;

    if( !isModule ) {
        if ( isFileExist( name ) ) {
            return load( name );
        }
        else {
            return 1;
        }
    }

    auto path = getBaseDir() + TestModulesDir + name + ".lua";
    if ( isFileExist( path.c_str() ) ) {
        return load( path.c_str() );
    }
    else {
        // Check if library plugin is called
        auto libraryFolderPath = getLibrariesDir() + name;
        if( isFolderExist(libraryFolderPath.c_str()) ) {
            auto configFilePath = libraryFolderPath + "/" + "config.json";

            if ( isFileExist( configFilePath.c_str() ) ) {
                std::ifstream stream( configFilePath );

                if (!stream.good()) {
                    throw std::runtime_error( std::string("Fail to open config ") + configFilePath );
                }

                const auto config = nlohmann::json::parse( stream );
                stream.close();

                if( config.count( "library" ) && config[ "library" ].count( "entryPoint" ) ) {
                    auto entryPointFilePath = config[ "library" ][ "entryPoint" ].get<std::string>();
                    entryPointFilePath = getLibrariesDir() + ( entryPointFilePath.c_str() + 4 ) + ".lua";
                    if( isFileExist( entryPointFilePath.c_str() ) ) {
                        return load( entryPointFilePath.c_str() );
                    }
                }
            }
        }
    }

    return 1;
}

void overrideModuleSearchers(lua_State* L) {
    static const lua_CFunction searchers[] = { preloadSearcher, luaSearcher };

    const int searchersCount = sizeof(searchers) / sizeof(searchers[0]);

    /* create 'searchers' table */
    lua_createtable(L, searchersCount, 0);
    /* fill it with predefined searchers */
    for (int i = 0; i < searchersCount; ++i) {
        lua_pushvalue(L, -2);  /* set 'package' as upvalue for all searchers */
        lua_pushcclosure(L, searchers[i], 1);
        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "searchers");  /* put it in field 'searchers' */
}

int fakefunction(lua_State* L) {
    return 0;
}

std::string logLevel2String(LogLevel logLevel) {
    switch (logLevel) {
        case LogLevel::Trace: return "trace";
        case LogLevel::Info: return "info";
        case LogLevel::Error: return "error";
        case LogLevel::Debug: return "debug";
        case LogLevel::Warning: return "warning";
        default: assert("Unknown value of LogLevel");
    }
}

void prepareLogFunctions(lua_State* L, LogLevel logLevel) {
    {
        const auto severity = logLevel2String(logLevel);
        lua_pushlstring(L, severity.data(), severity.size());
        lua_setglobal( L, "_TESTS_LOG_LEVEL" );
    }

    {
        lua_getglobal( L, "print" );
        lua_setglobal( L, "_test_print" );
    }

    if (logLevel > LogLevel::Info) {
        overrideFunction( L, "print", nullptr, fakefunction );
        overrideFunction( L, "write", nullptr, fakefunction );
    }
}

void preparePlatformAndHardwareRevision(lua_State* L, const std::string& platform, unsigned int hardwareRevision) {
    if (!platform.empty()) {
        lua_pushfstring( L, platform.c_str() );
        lua_setglobal( L, "_PLATFORM" );
    }

    if (hardwareRevision) {
        lua_pushinteger( L, hardwareRevision );
        lua_setglobal( L, "_HARDWARE_REVISION" );
    }
}

}

LuaWrapper::LuaWrapper(const std::string& platform, unsigned int hardwareRevision, LogLevel logLevel) :
    m_luaState( luaL_newstate(), &lua_close )
{
    lua_pushboolean(m_luaState.get(), 1);  /* signal for libraries to ignore env. vars. */
    lua_setfield(m_luaState.get(), LUA_REGISTRYINDEX, "LUA_NOENV");

    luaL_openlibs(m_luaState.get());

    lua_getglobal(m_luaState.get(), "package");
    overrideModuleSearchers(m_luaState.get());

    lua_pop(m_luaState.get(), 1);

    overrideOriginalRequire(m_luaState.get());
    overrideOriginalLoadfile(m_luaState.get());

    prepareLogFunctions(m_luaState.get(), logLevel);

    preparePlatformAndHardwareRevision(m_luaState.get(), platform, hardwareRevision);
}

LuaWrapper::~LuaWrapper() = default;

bool LuaWrapper::runScript(const char* fileName) {
    return doFile(m_luaState.get(), fileName) == LUA_OK;
}

int customLuaApiTest(lua_State* L);

bool LuaWrapper::runCustomLuaApiTest() {
    const auto top = lua_gettop(m_luaState.get());
    const int result = customLuaApiTest(m_luaState.get());
    const auto argsCount = lua_gettop(m_luaState.get()) - top;

    if ( argsCount < 0 ) {
        std::cerr << "Why did you pop " << -argsCount << " elements from stack " << std::endl;
        return false;
    }


    if ( argsCount != result ) {
        std::cerr << "Function result is " << result << " but you pushed " << argsCount << " params to stack" << std::endl;
        lua_pop( m_luaState.get(), argsCount );
        return false;
    }

    for ( int i = 0; i < argsCount; ++i ) {
        std::cout << "dump param " << i + 1 << ":\n";
        LuaTablePointersSet tables;
        dump( m_luaState.get(), tables, -argsCount + i, 1, 1 );
        std::cout << std::endl;
    }

    return true;
}

