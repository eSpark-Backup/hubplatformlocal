local logger = require("HUB:siren/scripts/utils/log")
logger.trace("siren teardown")

local storage = require("storage")
local storage_keys_getter = loadfile("HUB:siren/scripts/utils/storage_keys_getter")()
local switch_enum = require('HUB:siren/scripts/items/switch_enum')
local item_names = loadfile("HUB:siren/scripts/items/names")()

local function teardown_main()
    -- TODO: stop playback
    local item_key = storage_keys_getter.get_item_key(item_names.switch)
    local stored_value = storage.get_string(item_key)
    logger.trace("Stored item_id value: ", item_key, ": ", stored_value)

    loadfile("HUB:siren/scripts/set_item_value")(
        {
            item_id = stored_value,
            value = switch_enum.stop
        }
    )

    logger.info("FINISH: siren/teardown.lua")
end

local params = ...
local status, value = pcall(teardown_main, params)
if not status then
    logger.error("Error calling teardown_main: " .. value)
    return false
else
    return true
end
