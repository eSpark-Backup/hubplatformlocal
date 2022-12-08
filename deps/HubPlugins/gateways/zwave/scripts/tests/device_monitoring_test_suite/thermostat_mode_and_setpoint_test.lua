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


zwave.request_node_values = tests.create_mock()
zwave.set_value = tests.create_mock()
zwave.set_thermostat_mode = tests.create_mock()
zwave.set_setpoint_value = tests.create_mock()
zwave.request_value = tests.create_mock()
zwave.request_setpoint_value = tests.create_mock()

local function get_item_id(class_id, item_name)
    local item_id = storage:call("find_item_id", item_name, "9000", class_id, 0)
    tests.assert_ne(item_id, nil, item_name .. " not found")

    return item_id
end

local function check_set_mode_called(mode_name)
    tests.assert_called(zwave.set_thermostat_mode, "check set mode value", {
        { 9000, 0, mode_enum.token_to_value[mode_name] }
    })
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

local function check_set_setpoint_called(setpoint_name, value)
    tests.assert_called(zwave.set_setpoint_value, "check set setpoint value", {
        { 9000, class_ids.thermostat_setpoint, 0, setpoint_enum.token_to_value[setpoint_name], { value = value, scale = "celsius" } }
    })
end

local function check_set_setpoint_not_called()
    tests.assert_called(zwave.set_setpoint_value, "check set setpoint not called", nil, 0)
end

local function check_request_setpoint_value_called(setpoint_name)
    local setpoint_type = setpoint_enum.token_to_value[setpoint_name]

    tests.assert_called(zwave.request_setpoint_value, "check request setpoint value", {
        { 9000, 0, setpoint_enum.token_to_value[setpoint_name]}
    })
end

local function check_request_setpoint_value_not_called()
    tests.assert_called(zwave.request_setpoint_value, "check request setpoint not called", nil)
end

local function make_setpoint(name, min_value, max_value)
    return {
        type = setpoint_enum.token_to_value[name],
        min_value = min_value,
        max_value = max_value,
        scale = temperature_enum.token_to_value["celsius"],
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
                mode_enum.token_to_value["auto"],
                mode_enum.token_to_value["aux_mode"],
                mode_enum.token_to_value["resume"],
                mode_enum.token_to_value["fan_only"],
                mode_enum.token_to_value["furnace"],
                mode_enum.token_to_value["dry_air"],
                mode_enum.token_to_value["moist_air"],
                mode_enum.token_to_value["auto_change_over"],
                mode_enum.token_to_value["energy_saving_heat"],
                mode_enum.token_to_value["energy_saving_cool"],
                mode_enum.token_to_value["away"],
                mode_enum.token_to_value["reserved"],
                mode_enum.token_to_value["full_power"],
            }
        },
        {
            id = class_ids.thermostat_setpoint,
            version = 3,
            setpoints = {
                make_setpoint("heating", 10, 50),
                make_setpoint("cooling", 20, 60),
                make_setpoint("furnace", 30, 70),
                make_setpoint("dry_air", 40, 80),
                make_setpoint("moist_air", 50, 90),
                make_setpoint("auto_changeover", 60, 100),
                make_setpoint("energy_save_heating", 70, 110),
                make_setpoint("energy_save_cooling", 80, 120),
                make_setpoint("away_heating", 90, 130),
                make_setpoint("away_cooling", 100, 140),
                make_setpoint("full_power", 110, 150),
            }
        }
    }
}

local node = {
    id = 9000,
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,
    command_span = 0,
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
local thermostat_setpoint_cooling_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_cooling)

local function set_mode(item_id, value)
    external_setter({
        item_id = item_id,
        value = value
    })
end

local function simulate_mode_is_updated(mode)
    events_handler({
        event = "thermostat_mode_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_mode,
        mode = mode_enum.token_to_value[mode]
    })
end

local function simulate_setpoint_is_updated(value)
    events_handler({
        event = "value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        value = value
    })
end

--  step 1. Turn on some mode (It is Off by default)

local target_mode = "heat"
set_mode(thermostat_mode_item_id, target_mode)
timer.add_time(2000)
check_set_mode_called(target_mode)
check_set_setpoint_not_called()

simulate_mode_is_updated(target_mode)
timer.add_time(5000)

local target_setpoint_name = "heating"
check_request_setpoint_value_called(target_setpoint_name)

local test_setpoint_value = 100

--  step 2. Shift mode and set setpoint

simulate_setpoint_is_updated(115)

-- begin simulate setting moda and setpoint at the same time
target_mode = "cool"
set_mode(thermostat_mode_item_id, target_mode)
check_set_mode_called(target_mode)

target_setpoint_name = "cooling"
set_setpoint_value(thermostat_setpoint_cooling_item_id, test_setpoint_value)
check_set_setpoint_not_called()
-- end simulate setting moda and setpoint at the same time

simulate_mode_is_updated(target_mode)
timer.add_time(5000)

--  step 2.

check_set_setpoint_called("cooling", test_setpoint_value)
check_request_setpoint_value_called(target_setpoint_name)

-- check update mode on unreachable device
target_mode = "heat"

set_mode(thermostat_mode_item_id, target_mode)
check_set_mode_called(target_mode)
timer.add_time(15000)
check_set_setpoint_not_called()
