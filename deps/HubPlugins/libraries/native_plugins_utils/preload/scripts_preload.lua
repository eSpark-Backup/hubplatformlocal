--
-- @Brief
--
-- This script setups some glogal functionality with platform-specific implementation.
-- Most strong motivation to use platform-specific scripts is optimization for memory or performance depends on hardware capabilities.
-- Don't use this script for business logic customization.
--

local platform = _G._PLATFORM or "linux"
local hardware_revision = _G._HARDWARE_REVISION or 1

local method_to_file_name

if hubplugins ~= nil then
    print("preload script already executed")
    return
end

assert(hubplugins == nil, "preload script must be executed once")

--
-- hubplugins interface
--
-- weak_loadfile: Default loadfile loads and compiles script any way.
--                weak_loadfide provides not strong guarantee of loading.
--                So file can be taken from cache or loaded from disk depends on specific implementation.
--
-- weak_require: Default require loads, compiles, executes file and caches result.
--               weak_requie provides not strong guarantee of caching.
--               So file can be loaded and executed again depends on specific implementation.
--
-- object: Provides interface of object model. Call hubplugins.object(name, path, params...) for object creation.
--
-- try_collectgarbage: Calls garbarge collector inside if specific platform has lack of memory.
--                     Use it for memory cleanup instead of standard collectgarbage.
--


hubplugins = setmetatable({ }, {
        __index = function(t, k)
            t[k] = require(assert(method_to_file_name[k]))
            method_to_file_name[k] = nil
            return t[k]
        end
    })

if platform == "freertos" and hardware_revision == 1 then
    method_to_file_name = {
        weak_loadfile = "HUB:native_plugins_utils/preload/specific_file_loaders/weak_caching_loadfile",
        weak_require = "HUB:native_plugins_utils/preload/specific_file_loaders/weak_caching_require",
        object = "HUB:native_plugins_utils/preload/objects_realizations/non_caching_object",
        resource = "HUB:native_plugins_utils/preload/objects_realizations/non_caching_resource"
    }

    hubplugins.try_collectgarbage = function()
        collectgarbage()
        collectgarbage()
    end

    -- These features are disabled to save memory
    hubplugins.device_settings_enabled = false
    hubplugins.virtual_pin_codes_restrictions_enabled = false

    --hotfix. It is necessary to change the principle of dump because dump use a lot of memory
    hubplugins.dump = tostring
elseif platform == "linux" or platform == "freertos" then
    method_to_file_name = {
        weak_loadfile = "HUB:native_plugins_utils/preload/specific_file_loaders/caching_loadfile",
        weak_require = "HUB:native_plugins_utils/preload/specific_file_loaders/caching_require",
        object = "HUB:native_plugins_utils/preload/objects_realizations/vtable_caching_object",
        resource = "HUB:native_plugins_utils/preload/objects_realizations/caching_resource"
    }

    hubplugins.try_collectgarbage = function() end

    hubplugins.device_settings_enabled = true
    hubplugins.virtual_pin_codes_restrictions_enabled = true

    hubplugins.dump = function(...)
        return hubplugins.weak_loadfile("HUB:native_plugins_utils/preload/dump")(...)
    end
else
    error("Unknown platform " .. tostring(platform))
end

return hubplugins
