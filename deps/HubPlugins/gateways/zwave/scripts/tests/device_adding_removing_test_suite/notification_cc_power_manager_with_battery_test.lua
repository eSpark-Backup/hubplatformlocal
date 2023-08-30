local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert(loadfile("HUB:zwave/scripts/events/events_handling"))

zwave.request_node_values = tests.create_mock()

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

    channels = {
        {
            id = 0,
            classes = {
                {
                    id = 0x71,
                    notifications = {
                        {
                            type = "power_management",
                            event_types = { "charge_battery_soon" }
                        }
                    }
                },
                {
                    id = 0x80
                }
            }
        }
    }
}

zwave.add_node(node)

events_handler({
    event = "node_added",
    node_id = 9000
})

local expected_devices = {
    {
        name = "Unknown",
        category = "unknown",
        items = {
            {
                name = "battery",
                tag = "power_management",
                value = 100,
                channel_id = 0,
                cc_id = 0x71
            },
            {
                name = "battery",
                value = 100,
                channel_id = 0,
                cc_id = 0x80,
                is_merged = true
            }
        },
        node_id = "9000",
        battery_powered = true,
        security = "no",

        subdevices = {}
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile(test_suite_dir .. "check_zwave_node_adding_result")(expected_devices)
