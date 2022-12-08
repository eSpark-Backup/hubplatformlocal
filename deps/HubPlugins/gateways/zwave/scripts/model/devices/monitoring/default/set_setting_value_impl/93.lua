local self, setting_id, value, descriptor = ...

local zwave = require("zwave")
local core = require("core")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local anti_theft_unlock_cc = zwave.get_command_class(descriptor.node_id, 0, class_ids.anti_theft_unlock)
local unlocking_cc_id = anti_theft_unlock_cc and anti_theft_unlock_cc.id or descriptor.class_id

local function request_value(cc_id)
    local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", descriptor.node_id, 0, cc_id)
    request_value_timer:call("start")
    -- zwave.request_value(descriptor.node_id, cc_id, 0)
end

local function unlock_default()
    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
    local hint = storage:call("get_string", storage:call("get_anti_theft_hint_key", descriptor.node_id))

    if hint then
        zwave.unlock_anti_theft_default(descriptor.node_id, 0, unlocking_cc_id, hint)
    end

    request_value(unlocking_cc_id)
end

if descriptor.tag == "anti_theft_enabled" then
    core.set_setting_value(setting_id, value, "pending")

    if value then
        zwave.lock_anti_theft_default(descriptor.node_id, 0)
        request_value(descriptor.class_id)
    else
        unlock_default()
    end

elseif descriptor.tag == "auto_unlock" then
    core.update_setting_status(setting_id, "pending")
    unlock_default()

elseif descriptor.tag == "password" then

    if not pcall(zwave.unlock_anti_theft, descriptor.node_id, 0, unlocking_cc_id, value) then
        core.set_setting_value(setting_id, value, "failed")
        return
    end

    core.set_setting_value(setting_id, value, "pending")

    request_value(unlocking_cc_id)

elseif descriptor.tag == "lock_info" then
    error("Lock info is read only, don't set value to it!")
end

hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", setting_id):call("start", value)
