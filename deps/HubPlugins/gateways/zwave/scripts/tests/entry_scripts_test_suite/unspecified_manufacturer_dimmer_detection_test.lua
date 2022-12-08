local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local node = {
    basic_type = 4,
    channels = {
        {
            classes = {
                {
                    id = 32,
                    is_supported = true,
                    version = 2
                },
                {
                    id = 112,
                    parameters = {},
                    version = 0
                },
                {
                    id = 94,
                    version = 2
                },
                {
                    application_build_number = 255,
                    application_framework_api_build_number = 255,
                    firmware_versions = {
                        "1.3"
                    },
                    hardware_version = 0,
                    host_interface_build_number = 0,
                    id = 134,
                    library_type = "enhanced_slave",
                    protocol_build_number = 255,
                    protocol_version = "6.81.0",
                    sdk_version = "7.0.0",
                    version = 3,
                    zwave_protocol_version = "6.81"
                },
                {
                    id = 114,
                    version = 2
                },
                {
                    association_groups = {
                        {
                            group_id = 1,
                            group_nodes = {
                                1
                            }
                        }
                    },
                    association_groups_count = 1,
                    id = 133,
                    version = 2
                },
                {
                    id = 89,
                    version = 3
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_UNKNOWN",
    icon_type = "ICON_TYPE_GENERIC_ON_OFF_POWER_SWITCH",
    id = "9000",
    is_battery_powered = false,
    is_reachable = false,
    is_smart_include = false,
    manufacturer_id = 65535,
    product_id = 1,
    product_type = 170,
    security = {
        "unauthenticated"
    },
    security_class_started = {
        "unauthenticated"
    },
    specific_type = "SPECIFIC_TYPE_UNKNOWN"
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000 
} )



local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )

--  COMMENTED CODE is for addon for previous certification

--local dimmer_item_id = storage:call("find_item_id", "dimmer", 9000, class_ids.basic, 0 )
--tests.assert_ne(dimmer_item_id, nil)

local switch_item_id = storage:call("find_item_id", "switch", 9000, class_ids.basic, 0 )
tests.assert_ne(switch_item_id, nil)

zwave.set_value = tests.create_mock()
zwave.request_value = tests.create_mock()


--set_item_value({ item_id = dimmer_item_id, value = 0x0A })
set_item_value({ item_id = switch_item_id, value = true })

events_handler( {
    event = "value_updated",
    node_id = "9000",     
    channel_id = 0,
 --   class_id = class_ids.basic,
    class_id = class_ids.switch_binary,
--    value = 0x0F
    value = 0xFF
} )

--tests.assert_called(zwave.set_value, "", { { "9000", class_ids.basic, 0, 0x0A } } )
tests.assert_called(zwave.set_value, "", { { "9000", class_ids.basic, 0, 0xFF } } )

--tests.assert_eq(core.get_item(dimmer_item_id).value, 0x0A)
tests.assert_eq(core.get_item(switch_item_id).value, true)


