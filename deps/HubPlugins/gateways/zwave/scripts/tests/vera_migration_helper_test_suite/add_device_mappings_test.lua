local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function add_simple_node(node_id)
    local node = {
        basic_type = 0,
        channels = {
            {
                id = 0
            }
        },
        generic_type = "GENERIC_TYPE_UNKNOWN",
        id = tostring(node_id),
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
        node_id = node_id
    } )
end

add_simple_node(2)
add_simple_node(3)
add_simple_node(4)

-- init vera migartion
core.add_vera_device_mapping = tests.create_mock()

events_handler( {
    event = "vera_migration_finished",
    vera_device_model = loadfile("HUB:zwave/scripts/tests/vera_migration_helper_test_suite/simple_vera_device_model")()
} )

-- check result
local device_id_2 = assert(storage:call("find_device_id", 2), "device not found")
local device_id_3 = assert(storage:call("find_device_id", 3), "device not found")
local device_id_4 = assert(storage:call("find_device_id", 4), "device not found")

tests.assert_called(
    core.add_vera_device_mapping,
    "check, if vera device mappings added",
    {
        { 57, { device_id = device_id_2 } },
        { 58, { device_id = device_id_3 } },
        { 59, { device_id = device_id_4 } }
    }
)
