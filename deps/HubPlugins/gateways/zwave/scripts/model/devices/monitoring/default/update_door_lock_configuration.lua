local self, node_id, channel_id, configuration = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local door_lock_cc_id = require("HUB:zwave/scripts/resources/class_ids").door_lock
local core = require "core"

local function find_door_lock_setting_id(tag)
    return storage:call("find_setting_id", node_id, door_lock_cc_id, tag)
end

local function set_setting_value(id, value)
    if id then
        hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", id):call("stop")
        core.set_setting_value(id, value, "synced")
    end
end

if configuration.lock_timeout ~= nil then
    local lock_timeout_setting_id = find_door_lock_setting_id("lock_timeout")
    set_setting_value(lock_timeout_setting_id, configuration.lock_timeout)
end

local timed_operations_enabled_setting_id = find_door_lock_setting_id("timed_operations_enabled")
set_setting_value(timed_operations_enabled_setting_id, configuration.lock_timeout ~= nil)

local function update_standard_configuration_value(config_field, value_getter)
    if configuration[config_field] ~= nil then
        local tag = config_field
        local setting_id = find_door_lock_setting_id(tag)
        set_setting_value(setting_id, configuration[config_field])
    end
end

local function update_handle_ids_value(config_field)
    if configuration[config_field] ~= nil then
        local tag = config_field
        local setting_id = find_door_lock_setting_id(tag)
        if setting_id then
            local value = { }
            for _, id in ipairs(configuration[config_field]) do
                table.insert(value, tostring(math.floor(id)))
            end
            set_setting_value(setting_id, value)
        end
    end
end

update_standard_configuration_value("auto_relock_timeout")
update_standard_configuration_value("hold_and_release_timeout")
update_standard_configuration_value("twist_assist_enabled")
update_standard_configuration_value("block_to_block_enabled")

update_handle_ids_value("enabled_inside_handle_ids")
update_handle_ids_value("enabled_outside_handle_ids")
