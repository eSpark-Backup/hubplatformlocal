local self, setting_id, value, descriptor = ...

local zwave = require("zwave")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local settings_fields = {
    "enabled_inside_handle_ids",
    "enabled_outside_handle_ids",
    "lock_timeout",
    "timed_operations_enabled",
    "auto_relock_timeout",
    "hold_and_release_timeout",
    "twist_assist_enabled",
    "block_to_block_enabled"
}

-- prepare configuration
local configuration = { }

for _, field in pairs(settings_fields) do
    if field ~= descriptor.tag then
        local neighbor_setting_id = storage:call("find_setting_id", descriptor.node_id, descriptor.class_id, field)
        
        if neighbor_setting_id then
            local neighbor_timer = hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", neighbor_setting_id)
            local neighbor_timer_args = neighbor_timer:call("get_arguments")
            
            if neighbor_timer_args and neighbor_timer_args.value ~= nil then
                configuration[ field ] = neighbor_timer_args.value
            else
                local neighbor_setting = core.get_setting(neighbor_setting_id)
                configuration[ field ] = neighbor_setting.value
            end

        end
    else
        configuration[ field ] = value
    end
end

-- set configuration

local settings_fail_timer = hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", setting_id)

if descriptor.tag == "lock_timeout" and not configuration.timed_operations_enabled then
    settings_fail_timer:call("stop") -- TODO: start with value and some additional flag
    --
    core.set_setting_value(setting_id, value, "synced")
else
    -- save configuration

    settings_fail_timer:call("start", value)
    core.set_setting_value(setting_id, value, "pending")

    -- fix configuration
    local default_enabled_handle_ids = { }

    for _, field in ipairs({ "enabled_inside_handle_ids", "enabled_outside_handle_ids" }) do
        local ids = configuration[field]
        if ids then
            for index, id in ipairs(ids) do
                ids[index] = math.floor(tonumber(id))
            end
            table.sort(ids)
        else
            configuration[field] = default_enabled_handle_ids
        end
    end

    configuration.lock_timeout = configuration.timed_operations_enabled and configuration.lock_timeout or nil
    configuration.timed_operations_enabled = nil

    local default_extended_config = {
        auto_relock_timeout = 0,
        hold_and_release_timeout = 0,
        twist_assist_enabled = false,
        block_to_block_enabled = false
    }

    for field, default_value in pairs(default_extended_config) do
        if configuration[field] == nil then
            configuration[field] = default_value
        end
    end

    -- zwave set/request

    zwave.set_door_lock_configuration(descriptor.node_id, 0, configuration)
    zwave.request_door_lock_configuration(descriptor.node_id, 0)
end
