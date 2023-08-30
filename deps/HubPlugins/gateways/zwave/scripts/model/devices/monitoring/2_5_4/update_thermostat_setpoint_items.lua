local self, node_id, channel_id, mode, setpoint_type_setting_value = ...

local zwave = require("zwave")
local core = require("core")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local setpoint_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")
local setpoint_item_names = hubplugins.weak_require("HUB:zwave/scripts/resources/thermostat_setpoint_item_names")

local cc_setpoint = zwave.get_command_class(node_id, channel_id, class_ids.thermostat_setpoint)
local setpoints = self:call("find_current_setpoints", cc_setpoint, mode, setpoint_type_setting_value)
if not setpoints then
    return
end

local active_items = {}
for _, setpoint in ipairs(setpoints) do
    local active_item_name = setpoint_item_names[setpoint_enum.value_to_token[setpoint.type]]
    if active_item_name ~= nil then
        active_items[active_item_name] = true
    end
end

local thermostat_setpoint_items = {
    self:call("find_item", item_names.thermostat_setpoint, node_id, channel_id, class_ids.thermostat_setpoint),
    self:call("find_item", item_names.thermostat_setpoint_heating, node_id, channel_id, class_ids.thermostat_setpoint),
}

for _, item in ipairs(thermostat_setpoint_items) do
    local show = active_items[item.name] ~= nil
    if show ~= item.show  then
        item.enum = nil -- TODO: check and remove
        item.show = show

        print("modify item " .. item.name .. " " .. tostring(show))
        core.modify_item(item.id, item) -- TODO: use separate binding
    end
end

self:call("request_setpoints_value", node_id, channel_id, setpoints)
