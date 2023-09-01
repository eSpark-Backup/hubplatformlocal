local zwave = require("zwave")
local core = require("core")
local timer = require("timer")
local tests = require("tests")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local external_setter = hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")
local setpoint_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")
local temperature_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/temperature")

timer.set_timeout_with_id = tests.create_mock()
timer.cancel = tests.create_mock()
zwave.request_node_values = tests.create_mock()
zwave.set_setpoint_value = tests.create_mock()
zwave.request_setpoint_value = tests.create_mock()

local function get_item_id(class_id, item_name)
    local item_id = storage:call("find_item_id", item_name, "9000", class_id, 0)
    tests.assert_ne(item_id, nil, item_name .. " not found")

    return item_id
end

local function set_setpoint_value(item_id, value)
    external_setter({
        item_id = item_id,
        value = {
            value = value,
            scale = "celsius"
        }
    })
end

local function on_setpoint_value_report(type, value)
    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = setpoint_enum.token_to_value[type],
        value = value,
        scale = temperature_enum.token_to_value.celsius
    })
end

local function check_set_setpoint_called(setpoint_name, value)
    tests.assert_called(zwave.set_setpoint_value, "set setpoint value", {
        { "9000", class_ids.thermostat_setpoint, 0, setpoint_enum.token_to_value[setpoint_name], { value = value, scale = "celsius" } }
    })
end

local function check_request_setpoint_value_initiated(setpoint_name)
    local setpoint_type = setpoint_enum.token_to_value[setpoint_name]

    tests.assert_called(timer.set_timeout_with_id, "initiate request setpoint value", {
        {
            tests.skipped_param,
            "pending_operation_9000_0_request_setpoint_value_" .. setpoint_type,
            "HUB:zwave/scripts/timer_handlers/timeout/request_setpoint_value",
            {
                node_id = "9000",
                channel_id = 0,
                type = setpoint_type
            }
        }
    })
end

local function check_request_setpoint_value_cancelled(setpoint_name)
    local setpoint_type = setpoint_enum.token_to_value[setpoint_name]

    tests.assert_called(timer.cancel, "initiate request setpoint value", {
        { "pending_operation_9000_0_request_setpoint_value_" .. setpoint_type }
    })
end

local function make_setpoint(name, min_value, max_value)
    return {
        type = setpoint_enum.token_to_value[name],
        min_value = min_value,
        min_scale = temperature_enum.token_to_value["celsius"],
        max_value = max_value,
        min_scale = temperature_enum.token_to_value["celsius"],
    }
end

local channel_0 = {
    id = 0,
    classes = {
        {
            id = class_ids.thermostat_mode,
            modes = {
                mode_enum.token_to_value["off"],
                mode_enum.token_to_value["heat"],
                mode_enum.token_to_value["cool"],
                mode_enum.token_to_value["auto"]
            }
        },
        {
            id = class_ids.thermostat_setpoint,
            version = 3,
            setpoints = {
                make_setpoint("heating", 10, 50),
                make_setpoint("cooling", 20, 60)
            }
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node(node)

-- add node
events_handler({
    event = "node_added",
    node_id = 9000
})

local thermostat_mode_item_id = get_item_id(class_ids.thermostat_mode, item_names.thermostat_mode)
local thermostat_setpoint_heating_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_heating)

core.update_item_value(thermostat_mode_item_id, "auto")

set_setpoint_value(thermostat_setpoint_heating_item_id, 123)
check_set_setpoint_called("heating", 123)
check_request_setpoint_value_initiated("heating")

on_setpoint_value_report("heating", 123)
check_request_setpoint_value_cancelled("heating")
