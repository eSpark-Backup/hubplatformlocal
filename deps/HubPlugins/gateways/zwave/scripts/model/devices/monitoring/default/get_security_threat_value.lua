local self, item, value = ...

assert(self._device_info)

local security_items = hubplugins.weak_loadfile("HUB:zwave/scripts/resources/security_items")()
local security_settings = hubplugins.weak_loadfile("HUB:zwave/scripts/resources/security_settings")()
local item_names = require("HUB:zwave/scripts/resources/item_names")

local function find_security_setting(setting_tag)
    local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
    local item_descriptor = storage:call("find_item_descriptor", item.id)
    if not item_descriptor then return nil end

    return self:call("find_setting", item_descriptor.node_id, item_descriptor.class_id, setting_tag)
end

-- try to find common security value
if security_items[item.name] then
    if security_settings[item.name] then
        local setting = find_security_setting(security_settings[item.name])
        if setting and setting.value == false then
            return false
        end
    end

    return security_items[item.name][value]
end

local function is_security_state(secutiry_states, value)
    for _, state in ipairs(secutiry_states) do
        if state == value then return true end
    end
    return false
end

-- try to find panic button value
if self._device_info.panic_triggers then
    for _, trigger in ipairs(self._device_info.panic_triggers) do
        if trigger.item_name and item.name == trigger.item_name then
            return is_security_state(trigger.panic, value)
        elseif trigger.button_number
           and item.name == item_names.button_state
           and item.value.button_number == trigger.button_number then
            return is_security_state(trigger.panic, value.button_state)
        end
    end
end
