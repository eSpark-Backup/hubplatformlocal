local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require "tests"
local core = require( "core" )


zwave.request_node_values = tests.create_mock()

local function test_impl(region)
    zwave.get_firmware_region = function() return region end

    local cc_switch = {
        id = class_ids.switch_binary
    }

    local cc_firmware_update = {
        id = class_ids.firmware_update,
        firmware_ids = { 1, 5, 0 }
    }

    local cc_version = {
        id = class_ids.version,
        zwave_protocol_version = "1.1",
        hardware_version = 3,
        firmware_versions = { "1.0", "7.1", "2.3" }
    }

    local node = {
        id = "9000",
        manufacturer_id = 999,
        product_type = 998,
        product_id = 997,
        is_battery_powered = false,

        specific_type = "SPECIFIC_TYPE_POWER_SWITCH_BINARY",
        generic_type = "GENERIC_TYPE_SWITCH_BINARY",

        security = { "s0" },
        security_class_started = { "s0" },

        channels = { 
            {
                id = 0,
                classes = { cc_firmware_update, cc_switch, cc_version }
            }
        }
    }

    local expected_devices = {
        {
            name = "Smoke Sensor",
            category = "security_sensor",
            subcategory = "smoke",
            items = expected_smoke_items,
            node_id = "9000",
            battery_powered = false,

            subdevices = {
                {
                    name = "Heat Sensor",
                    category = "security_sensor",
                    subcategory = "heat",
                    items = expected_heat_items
                }
            }
        }
    }

    zwave.add_node( node )

    events_handler( {
        event = "node_added",
        node_id = 9000 
    } )

    local devices = core.get_devices()
    tests.assert_eq(#devices, 1, "check devices count")
    local device = devices[1]


    tests.assert_eq(
        device.info,
        {
            manufacturer = "manufacturer_999",
            model = "Unknown",
            protocol = "zwave",
            hardware = "3",
            ["firmware.stack"] = "1.1",
            ["zwave.smartstart"] = "no",
            ["zwave.node"] = "9000",
            ["channels.count"] = "1"
        },
        "Compare device info field"
    )

    tests.assert_eq(
        device.firmware,
        {
            { id = region .. ".999.1.0", version = "1.0" },
            { id = region .. ".999.5.1", version = "7.1" },
            { id = region .. ".999.998_997.2", version = "2.3" }
        },
        "Compare device firmware field")

    events_handler( {
        event = "node_removed",
        node_id = 9000,
        manufacturer_id = 999,
        product_type = 998,
        product_id = 997
    } )
end

tests.run_with_params(
test_impl,
{
    {
        value = "eu",
        description = "EU region"
    },
    {
        value = "us",
        description = "US region"
    }
})