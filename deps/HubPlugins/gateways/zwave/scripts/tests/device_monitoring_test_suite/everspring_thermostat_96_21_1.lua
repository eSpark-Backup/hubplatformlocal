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
zwave.get_thermostat_mode = tests.create_mock()
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

local function check_request_mode_called(mode_name)
    tests.assert_called(zwave.request_value, "check request mode value", {
        { 9000, 64, 0 }
    })
end

local function check_request_mode_not_called()
    tests.assert_called(zwave.request_value, "check request mode not called", nil, 0)
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
            id = 94,
            version = 2
        },
        {
            association_groups = {},
            association_groups_count = 1,
            id = 133,
            version = 2
        },
        {
            id = 89,
            version = 1
        },
        {
            id = 134,
            version = 3
        },
        {
            id = 114,
            version = 2
        },
        {
            id = 128,
            version = 1
        },
        {
            id = 122,
            version = 4
        },
        {
            id = 113,
            notifications = {
                {
                    event_types = {
                        "power_has_been_applied"
                    },
                    type = "power_management",
                    unknown_event_types = {}
                }
            },
            v1_supported = false,
            version = 8
        },
        {
            id = 112,
            version = 1
        },
        {
            id = 49,
            sensors = {
                {
                    scales = {
                        "celsius"
                    },
                    sensor_type = "air_temperature"
                }
            },
            version = 5
        },
        {
            id = class_ids.thermostat_mode,
            last_mode = 254,
            mode_activity = false,
            modes = {
                mode_enum.token_to_value["off"],
                mode_enum.token_to_value["heat"],
                mode_enum.token_to_value["energy_saving_heat"],
            },
            version = 3
        },
        {
            id = class_ids.thermostat_setpoint,
            setpoints = {
                {
                    max_scale = 0,
                    max_value = 35,
                    min_scale = 0,
                    min_value = 5,
                    type = 1
                },
                {
                    max_scale = 0,
                    max_value = 35,
                    min_scale = 0,
                    min_value = 5,
                    type = 11
                }
            },
            version = 3
        },
        {
            id = 129,
            version = 1
        }
    }
}

local node = {
    id = 9000,
    manufacturer_id = 96,
    product_id = 1,
    product_type = 21,
    is_battery_powered = true,
    command_span = 1000,
    generic_type = "GENERIC_TYPE_THERMOSTAT",
    icon_type = "ICON_TYPE_GENERIC_THERMOSTAT",

    is_reachable = false,
    is_smart_include = false,

    security = {
        "authenticated"
    },
    security_class_started = {
        "authenticated"
    },
    specific_type = "SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2",
    
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
local thermostat_setpoint_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint)
local thermostat_setpoint_heating_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_heating)

-- Check set

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

check_set_mode_called(target_mode)
check_set_setpoint_not_called()

check_request_mode_not_called(target_mode)
timer.add_time(2000)

check_request_mode_called(target_mode)

simulate_mode_is_updated(target_mode)
timer.add_time(5000)

local target_setpoint_name = "heating"
check_request_setpoint_value_called(target_setpoint_name)
