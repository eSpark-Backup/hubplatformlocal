local tests = require( "tests" )
local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")

zwave.request_node_values = tests.create_mock()

local node = {
    basic_type = 0,
    channels = {
        {
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_UNKNOWN",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_rediscover_required = true,
    is_smart_include = false,
    manufacturer_id = 0,
    product_id = 0,
    product_type = 0,
    security = {
        "none"
    },
    security_class_started = {
        "none"
    },
    specific_type = "SPECIFIC_TYPE_UNKNOWN"
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

-- check adding result

local expected_settings = {
    {
        value_type = "action",
        value = {
            text = "Rediscover",
            lang_tag = "rediscover_tag"
        },
        tag = "rediscover"
    }
}

local expected_devices = {
    {
        name = "Unknown",
        category = "unknown",
        node_id = "9000",
        battery_powered = false,
        security = "low",
        settings = expected_settings,
    }
}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"
loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices )
