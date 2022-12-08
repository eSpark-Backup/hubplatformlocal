local self, item_descriptor, value = ...

local zwave = require("zwave")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local setpoint_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")
local setpoint_item_names = hubplugins.weak_require("HUB:zwave/scripts/resources/thermostat_setpoint_item_names")

local node_id = item_descriptor.node_id
local channel_id = item_descriptor.channel_id
local class_id = item_descriptor.class_id

local cc_setpoint = zwave.get_command_class(node_id, channel_id, class_id)
local setpoint_type_setting = self:call("find_setting", node_id, class_ids.thermostat_setpoint, "controlled_setpoint_type")

local thermostat_mode_item = self:call("find_item", item_names.thermostat_mode, node_id, channel_id, class_ids.thermostat_mode)
local mode = thermostat_mode_item.value

local setpoints = self:call("find_current_setpoints", cc_setpoint, mode, setpoint_type_setting and setpoint_type_setting.value)
assert(setpoints, "Not supported " .. item_descriptor.item_name)

for _, setpoint in ipairs(setpoints) do
    local target_item_name = setpoint_item_names[setpoint_enum.value_to_token[setpoint.type]]

    if target_item_name == item_descriptor.item_name then
        zwave.set_setpoint_value(node_id, class_id, channel_id, setpoint.type, value)

        return {
            node_id = node_id,
            channel_id = channel_id,
            setpoints = { setpoint },
            delay = 1000
        }
    end
end
