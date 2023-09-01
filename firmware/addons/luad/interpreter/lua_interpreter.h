#pragma once

#include "luad/interpreter/module_loader/lua_module_loader.h"
#include "luad/interpreter/lua_script_params_holder.h"

#include "json_utils/json_fwd.h"

#include "base_utils/script_result.h"

extern "C" {
#include <lua.h>
}

#include <string>
#include <memory>
#include <stdexcept>
#include <atomic>
#include <chrono>

namespace HubPlatform {
namespace Addons {
class Addon;
}  // namespace Addons
extern const char* const TimeoutErrorMessage;
namespace Luad {
namespace UT {
class LuaTestSuite;
}  // namespace UT
}  // namespace Luad

class LuaInterpreterError : public std::runtime_error {
public:
    LuaInterpreterError( const std::string& errDescription )
    : runtime_error( "LuaInterpreter: " + errDescription ) {}
};

class LuaPluginResourcesProvider;

class LuaInterpreter final {
    friend class Luad::UT::LuaTestSuite;

public:
    LuaInterpreter( Addons::Addon& addon,
                    LuaPluginResourcesProvider& plugProvider,
                    std::chrono::steady_clock::duration timeout = std::chrono::steady_clock::duration::zero() );
    ~LuaInterpreter();

    LuaInterpreter( const LuaInterpreter& rhs ) = delete;
    LuaInterpreter& operator=( const LuaInterpreter& rhs ) = delete;

    LuaInterpreter( LuaInterpreter&& rhs );
    LuaInterpreter& operator=( LuaInterpreter&& rhs );

    /*
     * @brief compiles and loads a lua chunk using a file with Lua script
     *
     * @param [in] luaFilePath - a full path to a file with Lua script to be compiled and
     * loaded(pushed on the Lua stack)
     *
     * @note - throws an exception if the operation failed
     */
    void loadLuaFile( const std::string& luaFilePath );

    /*
     * @brief compiles and loads a lua chunk using a null-terminated C-like string
     *
     * @param [in] luaString - a string to be compiled and loaded(pushed on the Lua stack)
     *
     * @note - throws an exception if the operation failed
     */
    void loadLuaString( const std::string& luaString );

    /*
     * @brief compiles and loads a lua chunk using a plugin's script as a source
     *
     * @param [in] script - a script to be compiled and loaded(pushed on the Lua stack)
     *
     * @note - throws an exception if the operation failed
     */
    void loadScript( const std::string& script );

    /*
     * @brief run a previously loaded Lua chunk
     *
     * @param [in] params - params are the table of external arguments for script.
     *
     * @return ScriptResult
     */
    ScriptResult runLoadedCode( const Json* params = nullptr );
    ScriptResult runLoadedCode( LuaScriptParamsHolderPtr paramsHolder );

    /*
     * @brief get a state of previously run Lua chunk
     *
     * @return string of error message
     */
    std::string getExecutionState();

    void setTimeout( int timeout );

private:
    void forceStopScript();

private:
    /* each LuaInterpreter is represented by a unique lua_State */
    std::unique_ptr< lua_State, void ( * )( lua_State* ) > m_luaState;

    std::unique_ptr< LuaModuleLoader > m_moduleLoader;

    std::atomic_bool m_isRunning;

    std::chrono::steady_clock::duration m_timeout;
};

}  // namespace HubPlatform
