#include <lua.hpp>
#include <iostream>

int customLuaApiTest(lua_State* L) {
    // Don't commit any code here. Use this function only locallys
    return 0;
}

// Example 1:
//
//int customLuaApiTest(lua_State* L) {
//    lua_createtable(L, 0, 1);
//    {
//        lua_pushstring(L, "info");
//        lua_createtable(L, 0, 2);
//        lua_pushstring(L, "user@user");
//        lua_setfield(L, -2, "account");
//        lua_pushstring(L, "xxx");
//        lua_setfield(L, -2, "vendor");
//        lua_settable(L, -3);
//    }
//
//    {
//        lua_pushstring(L, "capabilities");
//        lua_createtable(L, 2, 0);
//        lua_pushstring(L, "first_capability");
//        lua_seti(L, -2, 1);
//        lua_pushstring(L, "second_capability");
//        lua_seti(L, -2, 2);
//        lua_settable(L, -3);
//    }
//
//    lua_pushstring(L, "Some string");
//
//    return 2;
//}
//
// run:
// make TESTS_FILTER=~ prepare_and_run_tests
//
// Expected output:
//
//dump param 1:
//    0x7fd283c04f00 (table)
//        "capabilities" -> 0x7fd283c050c0 (table)
//            1 -> "first_capability"
//            2 -> "second_capability"
//        "info" -> 0x7fd283c04f80 (table)
//            "vendor" -> "xxx"
//            "account" -> "user@user"
//
//dump param 2:
//    "Some string"


// Example 2:
//int customLuaApiTest(lua_State* L) {
//    luaL_loadstring(L, 
//R"(
//    local value, description = ...
//    print(("Lua api test: script has been called with value '%i' and description '%s'"):format(value, description))
//)");
//    lua_pushinteger(L, 100500);
//    lua_pushstring(L, "sto_pitsot");
//
//    lua_pcall(L, 2, 0, 0);
//
//    return 0;
//}
//
// run:
// make TESTS_FILTER=~ prepare_and_run_tests
//
// Expected output:
//
// Lua api test: script has been called with value '100500' and description 'sto_pitsot'

