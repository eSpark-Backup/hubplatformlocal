local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require "tests"

zwave.request_node_values = tests.create_mock()

local cc_switch = { 
    id = class_ids.switch_binary,
}

local cc_sensor_multilevel = {
    id = class_ids.multilevel_sensor,
    sensors = {
        {scales = "watt", sensor_type = "power"}
    }
}

local channel_0 = { 
    id = 0,
    classes = { cc_switch, cc_sensor_multilevel }
}

local channel_1 = { 
    id = 1,
    classes = { cc_switch }
}


local channel_2 = { 
    id = 2,
    classes = { cc_switch }
}

local node = {
    id = "9000",
    manufacturer_id = 271,
    product_type = 0,
    product_id = 0,
    is_battery_powered = false,

    specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
    generic_type = "GENERIC_TYPE_SWITCH_BINARY",

    security = { "s0" },
    security_class_started = { "s0" },

    channels = { channel_0, channel_1, channel_2 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )

-- check adding result
local expected_devices = {
    {
        name = "Fibaro In Wall Switch",
        category = "switch",
        subcategory = "in_wall",
        items = {
            {
                name = "switch",
                cc_id = class_ids.switch_binary,
                channel_id = 0
            },
            {
                name = "electric_meter_watt",
                cc_id = class_ids.multilevel_sensor,
                channel_id = 0
            }
        },
        node_id = "9000",
        battery_powered = false,
        security = "middle",

        subdevices = {
            {
                name = "In Wall Switch 1",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 1
                    }
                }
            },
            {
                name = "In Wall Switch 2",
                category = "switch",
                subcategory = "in_wall",
                items = {
                    {
                        name = "switch",
                        cc_id = class_ids.switch_binary,
                        channel_id = 2
                    }
                }
            },
        }
    }
}

local function get_request_delay(zwave_node)
    local dev_handler_id = loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(zwave_node.manufacturer_id, zwave_node.product_type, zwave_node.product_id)

    local device_info = loadfile("HUB:zwave/scripts/helpers/get_device_info")(dev_handler_id)

    return device_info.request_value_delay
end

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )

tests.assert_called( zwave.request_node_values, "check  requesting of current values ", { { "9000", get_request_delay(node), 5, true } } )
