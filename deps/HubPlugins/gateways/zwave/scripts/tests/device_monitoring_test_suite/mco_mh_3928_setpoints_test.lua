local zwave = require("zwave")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local core = require("core")
local timer = require("timer")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

zwave.set_value = tests.create_mock()
zwave.request_value = tests.create_mock()

local node = {
    basic_type = 4.0,
    channels = {
        {
            classes = {
                {
                    id = 94.0,
                    version = 2.0
                },
                {
                    firmware_versions = {
                        "1.5"
                    },
                    hardware_version = 255.0,
                    id = 134.0,
                    library_type = "enhanced_slave",
                    version = 2.0,
                    zwave_protocol_version = "6.4"
                },
                {
                    id = 114.0,
                    version = 2.0
                },
                {
                    association_groups = {
                        {
                            group_id = 1.0,
                            group_nodes = {
                                1.0
                            }
                        }
                    },
                    association_groups_count = 1.0,
                    id = 133.0,
                    version = 2.0
                },
                {
                    id = 89.0,
                    version = 1.0
                },
                {
                    id = 49.0,
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
                    version = 5.0
                },
                {
                    id = 67.0,
                    setpoints = {
                        {
                            type = 1.0
                        },
                        {
                            type = 2.0
                        }
                    },
                    version = 2.0
                },
                {
                    id = 64.0,
                    last_mode = 254.0,
                    mode_activity = false,
                    modes = {
                        0.0,
                        1.0,
                        2.0,
                        3.0
                    },
                    version = 3.0
                },
                {
                    id = 66.0,
                    version = 1.0
                },
                {
                    id = 68.0,
                    modes = {
                        0.0,
                        1.0
                    },
                    version = 3.0
                },
                {
                    id = 69.0,
                    version = 2.0
                },
                {
                    id = 112.0,
                    parameters = {},
                    version = 1.0
                },
                {
                    firmware_ids = {
                        30978.0
                    },
                    id = 122.0,
                    version = 4.0
                }
            },
            id = 0.0
        }
    },
    generic_type = "GENERIC_TYPE_THERMOSTAT",
    icon_type = "ICON_TYPE_GENERIC_THERMOSTAT",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_smart_include = false,
    manufacturer_id = 351.0,
    product_id = 20738.0,
    product_type = 14632.0,
    security = {
        "unauthenticated"
    },
    security_class_started = {
        "unauthenticated"
    },
    specific_type = "SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2"
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- prepare functions

local function get_item_id(class_id, item_name)
    local item_id = storage:call("find_item_id", item_name, "9000", class_id, 0)
    tests.assert_ne(item_id, nil, item_name .. " not found")

    return item_id
end

local thermostat_mode_item_id = get_item_id(class_ids.thermostat_mode, item_names.thermostat_mode)
local thermostat_setpoint_heating_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_heating)
local thermostat_setpoint_cooling_item_id = get_item_id(class_ids.thermostat_setpoint, item_names.thermostat_setpoint_cooling)


core.update_item_value(thermostat_mode_item_id, "auto")

-- check heating setpoint
do
    core.update_item_value_with_min_max = tests.create_mock()

    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = 1,
        value = 15,
        scale = 0
    })

    tests.assert_called(
        core.update_item_value_with_min_max,
        "check min max values",
        { {thermostat_setpoint_heating_item_id, {value = 15, scale = "celsius"}, {value = 5, scale = "celsius"}, {value = 36.6, scale = "celsius"}} }
    )
end

-- check cooling setpoint
do
    core.update_item_value_with_min_max = tests.create_mock()

    events_handler({
        event = "thermostat_setpoint_value_updated",
        node_id = 9000,
        channel_id = 0,
        class_id = class_ids.thermostat_setpoint,
        type = 2,
        value = 13,
        scale = 0
    })

    tests.assert_called(
        core.update_item_value_with_min_max,
        "check min max values",
        { {thermostat_setpoint_cooling_item_id, {value = 13, scale = "celsius"}, {value = 5, scale = "celsius"}, {value = 36.6, scale = "celsius"}} }
    )
end
