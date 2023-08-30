local handler, node, device_id, endpoint_id, cluster_name, attribute = ...

local helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/thermostat_helper")()

local zigbee = require("zigbee")
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local thermostat_mode_enum = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/thermostat_mode")
local thermostat_op_state_enum = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/thermostat_operating_state")

local function update_simple_enum_item(item_name, enum, value_getter)
    local new_value = value_getter(attribute.formatted_value)

    local item_id = handler:find_item_id(item_name, node.id, endpoint_id, cluster_name, attribute.name)
    if item_id and new_value then
        handler:update_value_in_core(item_id, new_value)
    else
        print("Failed to update " .. item_name .. " enum value: " .. attribute.formatted_value or "nil")
    end
end

local function get_setpoint_limits(attribute_name, item_name)
    local min_value, max_value = handler:calculate_min_max_value(
        node,
        cluster_name,
        item_name,
        function()
            local min_attr_value, max_attr_value
            local border_attr = handler:find_attribute(node, endpoint_id, cluster_name, helper.min_max_attributes[attribute_name].min)
            if border_attr and border_attr.formatted_value ~= "undefined" then
                min_attr_value = handler:to_temperature(border_attr.formatted_value)
            end

            border_attr = handler:find_attribute(node, endpoint_id, cluster_name, helper.min_max_attributes[attribute_name].max)
            if border_attr and border_attr.formatted_value ~= "undefined" then
                max_attr_value = handler:to_temperature(border_attr.formatted_value)
            end

            if min_attr_value ~= max_attr_value then
                return min_attr_value, max_attr_value
            end
        end
    )

    return { min = min_value, max = max_value }
end

local function update_setpoint_value(item_id, value, min_max_values)
    if min_max_values then
        handler:update_value_with_min_max_in_core(item_id, value, min_max_values.min, min_max_values.max)
    else
        handler:update_value_in_core(item_id, value)
    end
end

local function set_item_visibility(item_name, is_shown)
    local item_name_to_attribute = {
        [item_names.thermostat_setpoint_heating] = "OccupiedHeatingSetpoint",
        [item_names.thermostat_setpoint_cooling] = "OccupiedCoolingSetpoint"
    }

    local item = handler:find_item(item_name, node.id, endpoint_id, cluster_name, item_name_to_attribute[item_name])

    item.enum = nil -- Todo: why is this present?
    item.show = is_shown

    handler:modify_item_in_core(item)
end


-- main


if attribute.name == "SystemMode" then
    update_simple_enum_item(item_names.thermostat_mode, thermostat_mode_enum, helper.mode_to_item_value)

    -- request setpoints values
    if attribute.formatted_value == "off" then
        set_item_visibility(item_names.thermostat_setpoint_heating, false)
        set_item_visibility(item_names.thermostat_setpoint_cooling, false)
    elseif attribute.formatted_value == "cool" then
        set_item_visibility(item_names.thermostat_setpoint_heating, false)
        set_item_visibility(item_names.thermostat_setpoint_cooling, true)

        zigbee.request_attribute_value(node.id, endpoint_id, cluster_name, "OccupiedCoolingSetpoint")
    elseif attribute.formatted_value == "heat" then
        set_item_visibility(item_names.thermostat_setpoint_heating, true)
        set_item_visibility(item_names.thermostat_setpoint_cooling, false)

        zigbee.request_attribute_value(node.id, endpoint_id, cluster_name, "OccupiedHeatingSetpoint")
    else
        set_item_visibility(item_names.thermostat_setpoint_heating, true)
        set_item_visibility(item_names.thermostat_setpoint_cooling, true)

        zigbee.request_attribute_value(node.id, endpoint_id, cluster_name, "OccupiedCoolingSetpoint")
        zigbee.request_attribute_value(node.id, endpoint_id, cluster_name, "OccupiedHeatingSetpoint")
    end

elseif attribute.name == "ThermostatRunningState" then
    update_simple_enum_item(item_names.thermostat_operating_state, thermostat_op_state_enum, helper.op_state_to_item_value)

elseif attribute.name == "LocalTemperature" then
    if attribute.formatted_value ~= "undefined" then
        local item_id = handler:find_item_id(item_names.temp, node.id, endpoint_id, cluster_name, attribute.name)
        if item_id then
            handler:update_value_in_core(item_id, handler:to_temperature(attribute.formatted_value))
        end
    end

elseif attribute.name == "OccupiedCoolingSetpoint" or attribute.name == "OccupiedHeatingSetpoint" then
    -- update value of specific setpoint
    local item_name = attribute.name == "OccupiedCoolingSetpoint" and
        item_names.thermostat_setpoint_cooling or
        item_names.thermostat_setpoint_heating
    local item_id = handler:find_item_id(item_name, node.id, endpoint_id, cluster_name, attribute.name)
    if not item_id then
        print("Can not find setpoint item")
        return
    end

    local new_value = handler:to_temperature(attribute.formatted_value)

    local min_max_values = get_setpoint_limits(attribute.name, item_name)
    update_setpoint_value(item_id, new_value, min_max_values)

elseif attribute.name == "LocalTemperatureCalibration" then
    local setting = handler:find_setting("thermostat_temperature_calibration", node.id, endpoint_id, cluster_name, attribute.name)
    if setting then
        handler:update_setting_value_in_core(setting.id, handler:to_temperature_with_divider(attribute.formatted_value, 10))
        zigbee.request_attribute_value(node.id, endpoint_id, cluster_name, "LocalTemperature")
    end

else
    print("No monitoring functionality for " .. attribute.name .. "attribute")
end
