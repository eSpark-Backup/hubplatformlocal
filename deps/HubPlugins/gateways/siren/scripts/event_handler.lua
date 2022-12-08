-- Handles Lua API for Siren Speaker events: speaker_playing_status
local logger = loadfile("HUB:siren/scripts/utils/log")()
local log = require('logging')

local function trace(...)
    logger.trace("event_handler.lua: ", ...)
end
trace("siren event_handler")

local switch_enum = require('HUB:siren/scripts/items/switch_enum')
local core = require("core")
local storage = require("storage")
local storage_keys_getter = loadfile("HUB:siren/scripts/utils/storage_keys_getter")()
local item_names = loadfile("HUB:siren/scripts/items/names")()

local function storage_fetch_key(key_name)
    local value = storage.get_string(key_name)
    return value
end

local function storage_remove_key(key_name)
    if storage.exists(key_name) then
        storage.delete(key_name)
    end
end

local function change_switch_status(item_name, switch_status)
    trace('change_switch_status(item_name, switch_status): ', item_name, switch_status,
        type(switch_status), type(switch_status) == "boolean")
    assert(type(item_name) == "string", "Wrong argument type: arg 1")
    assert(type(switch_status) == "boolean", "Wrong argument type: arg 2")

    local item_id = storage_fetch_key(storage_keys_getter.get_item_key(item_name, nil))
    if item_id ~= nil then
        core.update_item_value(item_id, switch_status)
    else
        logger.error("cannot find item id for item name " .. item_name)
    end
end

local function handle_speaker_events(params)
    trace('handle_speaker_events', "Event speaker.playing_status_event caught, id: " .. params.id)
    local status = assert(params.status, "Event status field access failed")


    if status == "finished" or status == "stopped" or status == "failed" then
        trace('handle_speaker_events', "Change playback item status to 'stop'")
        change_switch_status(item_names.switch, switch_enum.stop)

        -- remove itemId from storage
        local key_command_id = storage_keys_getter.get_speaker_command_id()
        local current_id = storage_fetch_key(key_command_id)

        if current_id == params.id then
            trace('handle_speaker_events', "Remove command_id from storage: ", current_id)
            storage_remove_key(key_command_id)
        else
            log.error('handle_speaker_events', "Skip removing command_id from storage: stored value != event.id",
                current_id, params.id)
            logger.error(params)
        end
    elseif status == "playing" then
        trace('handle_speaker_events', "Change playback item status to 'play'")
        change_switch_status(item_names.switch, switch_enum.play)
    else
        logger.error('handle_speaker_events', "Unknown status in event")
    end
end

local function handle_events(params)
    -- {id, status, user_data} fields are passed here for speaker events
    if  params.id ~= nil and params.status ~= nil then
        handle_speaker_events(params)
    end
end

local params = ... or {}
logger.trace_dump(params, "script_params")
assert(params, "Parameters are empty")
assert(type(params) == "table", "Invalid argument type, table required")

local status, value = pcall(handle_events, params)
if not status then
    logger.error("handle_events failed: ")
    logger.error(value)
end
