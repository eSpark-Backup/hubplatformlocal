local self, node_id, channel_id, class_id, lock_info = ...

if channel_id ~= 0 then
    print("Cant update anti-theft state for channel " .. channel_id)
    return
end

local core = require "core"
local zwave = require "zwave"

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local password_setting_id = storage:call("find_setting_id", node_id, class_ids.anti_theft, "password")

if lock_info then
    storage:call("set_string", storage:call("get_anti_theft_hint_key", node_id), lock_info.hint)
else
    storage:call("remove_value", storage:call("get_anti_theft_hint_key", node_id))
end

if password_setting_id then
    local auto_unlock_setting_id = storage:call("find_setting_id", node_id, class_ids.anti_theft, "auto_unlock")

    local password_setting_timer = hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", password_setting_id)
    local auto_unlock_setting_timer = auto_unlock_setting_id and hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", auto_unlock_setting_id)

    local function update_status(status)
        if password_setting_timer:call("get_arguments") then
            core.update_setting_status(password_setting_id, status)
            password_setting_timer:call("stop")
        end

        if auto_unlock_setting_timer and auto_unlock_setting_timer:call("get_arguments") then
            core.update_setting_status(auto_unlock_setting_id, status)
            auto_unlock_setting_timer:call("stop")
        end
    end


    if lock_info then
        print("Unlocking is not successful")
        update_status("failed")
    else
        print("Unlocking is successful. Start rediscover.")

        update_status("synced")
        zwave.start_rediscover(node_id, "HUB:zwave/configs/device_setting.json")
    end
else
    local anti_theft_enabled_setting_id = storage:call("find_setting_id", node_id, class_ids.anti_theft, "anti_theft_enabled")

    core.set_setting_value(anti_theft_enabled_setting_id, lock_info ~= nil, "synced")

    local anti_theft_enabled_setting_timer = hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", anti_theft_enabled_setting_id)
    anti_theft_enabled_setting_timer:call("stop")
end
