local self, node_id, channel_id, class_id, setpoint_type, value, scale, is_requested = ...

local core = require("core")
local zwave = require("zwave")
local timer = require("timer")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")
local setpoint_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")
local setpoint_item_names = hubplugins.weak_require("HUB:zwave/scripts/resources/thermostat_setpoint_item_names")


local function is_active_setpoint_updated(thermostat_mode, cc_setpoint)
    local setpoint_type_setting = self:call("find_setting", node_id, class_ids.thermostat_setpoint, "controlled_setpoint_type")
    local setpoints = self:call("find_current_setpoints", cc_setpoint, thermostat_mode, setpoint_type_setting and setpoint_type_setting.value)
    if setpoints then
        for _, setpoint in ipairs(setpoints) do
            if setpoint.type == setpoint_type then
                return true
            end
        end
    end
    return false
end


local function get_item_id(item_name)
    local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)
    assert(item_id, "[handler.update_thermostat_setpoint_value]: " ..
            self:call("create_item_id_not_found_error_message", item_name, node_id, channel_id, class_id))

    return item_id
end


-- main

-- cancel any pending get requests for this setpoint
local timer_id = storage:call("create_pending_operation_key", node_id, channel_id, "request_setpoint_value", { math.floor(setpoint_type) })
timer.cancel(timer_id)

-- get current thermostat mode
local thermostat_mode_item = self:call("find_item", item_names.thermostat_mode, node_id, channel_id, class_ids.thermostat_mode)
local thermostat_mode = thermostat_mode_item.value

local cc_setpoint = zwave.get_command_class(node_id, channel_id, class_id)

-- check if active setpoint was updated
if not is_active_setpoint_updated(thermostat_mode, cc_setpoint) then
    return
end

local target_item_name = setpoint_item_names[setpoint_enum.value_to_token[setpoint_type]]
local item_id = get_item_id(target_item_name)

-- get scales
local capabilities = self:call("get_setpoint_capabilities", cc_setpoint, setpoint_type, target_item_name, item_id)
local scale_string
local min_scale
local max_scale

do
    local temperature_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/temperature")
    scale_string = temperature_enum.value_to_token[scale]

    min_scale = temperature_enum.value_to_token[capabilities.min_scale]
    max_scale = temperature_enum.value_to_token[capabilities.max_scale]
end

core.update_item_value_with_min_max(
    item_id,
    { value = value, scale = scale_string },
    { value = capabilities.min_value, scale = min_scale },
    { value = capabilities.max_value, scale = max_scale }
)

self:call("react_on_setpoint_updated", item_id, node_id, channel_id, class_id, value, scale_string, is_requested)
