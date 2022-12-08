local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local set_setting_value = loadfile( "HUB:zwave/scripts/set_setting_value" )
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )
local core = require("core")

zwave.request_node_values = tests.create_mock()
zwave.set_door_lock_configuration = tests.create_mock()
zwave.request_door_lock_configuration = tests.create_mock()
zwave.set_door_lock_mode = tests.create_mock()
zwave.request_door_lock_state = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { 
        {
            id = class_ids.door_lock,
            version = 4,
            capabilities = {
                timed_operations_supported = false,
                hold_and_release_supported = false,
                twist_assist_supported = false,
                block_to_block_supported = false,
                auto_relock_supported = true,

                inside_handle_ids = { },
                outside_handle_ids = { 1, 2 },

                door_lock_modes = {
                    "unsecured",
                    "unsecured_with_timeout",
                    "unsecured_for_inside",
                    "unsecured_for_inside_with_timeout",
                    "secured",
                    "unknown"
                },

                door_supported = true,
                bolt_supported = false,
                latch_supported = true
            }
        }
    }
}

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

    channels = { channel_0 }
}

zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )


local function find_setting_id(tag)
    return assert(storage:call("find_setting_id", 9000, class_ids.door_lock, tag), "setting id with tag '" .. tag .. "' must be present")
end

local function get_fail_timer_arguments( id )
    return hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", id ):call("get_arguments")
end

local auto_relock_timeout_id = find_setting_id("auto_relock_timeout")
local enabled_outside_handle_ids_id = find_setting_id("enabled_outside_handle_ids")



do -- set enabled outside handle ids
    set_setting_value({ 
        setting_id = enabled_outside_handle_ids_id,
        value = { "1", "2" }
    })

    local configuration = {
        enabled_inside_handle_ids = { },
        enabled_outside_handle_ids = { 1, 2 },
        auto_relock_timeout = 0,
        hold_and_release_timeout = 0,
        twist_assist_enabled = false,
        block_to_block_enabled = false
    }

    tests.assert_called(
        zwave.set_door_lock_configuration, 
        "check set_door_lock_configuration was called",  
        { { "9000", 0, configuration } })


    tests.assert_called(zwave.request_door_lock_configuration, "check request_door_lock_configuration was called", { { "9000", 0 } })

    events_handler( {
        event = "door_lock_configuration_updated",
        node_id = 9000,
        channel_id = 0,
        value = configuration
    } )
end


do -- set auto relock timeout
    set_setting_value({ 
        setting_id = auto_relock_timeout_id,
        value = 225
    })

    local configuration = {
        enabled_inside_handle_ids = { },
        enabled_outside_handle_ids = { 1, 2 },
        auto_relock_timeout = 225,
        hold_and_release_timeout = 0,
        twist_assist_enabled = false,
        block_to_block_enabled = false
    }

    tests.assert_called(
        zwave.set_door_lock_configuration, 
        "check set_door_lock_configuration was called",  
        { { "9000", 0, configuration } })


    tests.assert_called(zwave.request_door_lock_configuration, "check request_door_lock_configuration was called", { { "9000", 0 } })

    events_handler( {
        event = "door_lock_configuration_updated",
        node_id = 9000,
        channel_id = 0,
        value = configuration
    } )
end

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )


tests.assert_eq(require("storage").empty(), true, "check all cache was removed from storage")



