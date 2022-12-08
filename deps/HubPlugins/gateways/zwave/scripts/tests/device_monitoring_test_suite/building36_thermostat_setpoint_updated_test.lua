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

core.update_item_value_with_min_max = tests.create_mock()

local function set_item_value(type, value, scale)
    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = setpoint_enum.token_to_value[type],
        value = value,
        scale = scale
    })
end

local function get_item_id(class_id, item_name)
    local item_id = storage:call("find_item_id", item_name, "9000", class_id, 0)
    tests.assert_ne(item_id, nil, item_name .. " not found")

    return item_id
end

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = false,
                    version = 1
                },
                {
                    id = 112,
                    parameters = {},
                    version = 1
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            },
                            group_nodes_max_count = 4
                        }
                    },
                    association_groups_count = 1,
                    id = 133,
                    version = 2
                },
                {
                    id = 128,
                    version = 1
                },
                {
                    id = 129,
                    version = 1
                },
                {
                    firmware_ids = {
                        0,
                        1
                    },
                    id = 122,
                    version = 3
                },
                {
                    id = 114,
                    version = 1
                },
                {
                    id = 49,
                    sensors = {
                        {
                            scales = {
                                "celsius",
                                "fahrenheit"
                            },
                            sensor_type = "air_temperature"
                        },
                        {
                            scales = {
                                "percent"
                            },
                            sensor_type = "humidity"
                        }
                    },
                    version = 6
                },
                {
                    id = 68,
                    modes = {
                        0,
                        1,
                        6
                    },
                    version = 3
                },
                {
                    id = 69,
                    version = 1
                },
                {
                    id = 64,
                    modes = {
                        0,
                        1,
                        2,
                        3
                    },
                    version = 3
                },
                {
                    id = 66,
                    version = 2
                },
                {
                    id = 67,
                    setpoints = {
                        {
                            max_scale = 3,
                            max_value = 0.02,
                            min_scale = 1,
                            min_value = 900.2,
                            type = 1
                        },
                        {
                            max_scale = 3,
                            max_value = 0.02,
                            min_scale = 1,
                            min_value = 1284.2,
                            type = 2
                        }
                    },
                    version = 3
                },
                {
                    firmware_versions = {
                        "1.10",
                        "1.50"
                    },
                    hardware_version = 2,
                    id = 134,
                    library_type = "enhanced_slave",
                    version = 2,
                    zwave_protocol_version = "3.95"
                },
                {
                    association_groups = {
                        {
                            commands = {
                                {
                                    class_id = 64,
                                    command_id = 3,
                                    name = "THERMOSTAT_MODE_REPORT"
                                },
                                {
                                    class_id = 64,
                                    command_id = 5
                                },
                                {
                                    class_id = 67,
                                    command_id = 3,
                                    name = "THERMOSTAT_SETPOINT_REPORT"
                                },
                                {
                                    class_id = 66,
                                    command_id = 3,
                                    name = "THERMOSTAT_OPERATING_STATE_REPORT"
                                },
                                {
                                    class_id = 68,
                                    command_id = 3,
                                    name = "THERMOSTAT_FAN_MODE_REPORT"
                                },
                                {
                                    class_id = 69,
                                    command_id = 3,
                                    name = "THERMOSTAT_FAN_STATE_REPORT"
                                },
                                {
                                    class_id = 49,
                                    command_id = 5,
                                    name = "SENSOR_MULTILEVEL_REPORT"
                                },
                                {
                                    class_id = 128,
                                    command_id = 3,
                                    name = "BATTERY_REPORT"
                                },
                                {
                                    class_id = 90,
                                    command_id = 1,
                                    name = "DEVICE_RESET_LOCALLY_NOTIFICATION"
                                }
                            },
                            group_id = 1,
                            group_name = "Lifeline",
                            profile = {
                                name = "General:Lifeline",
                                profile_lsb = 1,
                                profile_msb = 0
                            }
                        }
                    },
                    association_groups_count = 1,
                    id = 89,
                    version = 1
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_THERMOSTAT",
    icon_type = "ICON_TYPE_GENERIC_THERMOSTAT",
    id = "9000",
    is_battery_powered = true,
    is_reachable = false,
    is_rediscover_required = false,
    is_smart_include = false,
    manufacturer_id = 400,
    product_id = 1,
    product_type = 1,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2",
    command_span = 0
}

-- prepare zwave
zwave.add_node(node)

-- add node
events_handler({
    event = "node_added",
    node_id = 9000
})

local thermostat_mode_item_id = get_item_id(class_ids.thermostat_mode, item_names.thermostat_mode)
--local thermostat_setpoint_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint)
local thermostat_setpoint_heating_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_heating)
local thermostat_setpoint_cooling_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_cooling)

local function activate_mode(mode)
    core.update_item_value(thermostat_mode_item_id, mode)
end

local function check_items_of_setpoints_in_mode(mode, setpoint_items)
    activate_mode(mode)

    for _, setpoint_item in ipairs(setpoint_items) do
        local value = setpoint_item.value
        local scale = temperature_enum.token_to_value["celsius"]

        set_item_value(setpoint_item.setpoint_name, value, scale)

        tests.assert_called(
            core.update_item_value_with_min_max,
            "check setpoint updated",
            {   
                { 
                    setpoint_item.item_id, 
                    { value = setpoint_item.value, scale = "celsius" }, 
                    { value = setpoint_item.min_value, scale = "celsius" }, 
                    { value = setpoint_item.max_value, scale = "celsius" } 
                } 
            }
        )
    end
end

do
    check_items_of_setpoints_in_mode("off", {
    })

    check_items_of_setpoints_in_mode("heat", {
        {
            setpoint_name = "heating",
            item_id = thermostat_setpoint_heating_item_id,
            value = 25,
            min_value = 0,
            max_value = 33
        }
    })

    check_items_of_setpoints_in_mode("cool", {
        {
            setpoint_name = "cooling",
            item_id = thermostat_setpoint_cooling_item_id,
            value = 30,
            min_value = 15,
            max_value = 45
        }
    })

    check_items_of_setpoints_in_mode("auto", {
        {
            setpoint_name = "heating",
            item_id = thermostat_setpoint_heating_item_id,
            value = 25,
            min_value = 0,
            max_value = 33
        },
        {
            setpoint_name = "cooling",
            item_id = thermostat_setpoint_cooling_item_id,
            value = 30,
            min_value = 15,
            max_value = 45
        }
    })
end
