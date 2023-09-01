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
                timed_operations_supported = true,
                hold_and_release_supported = true,
                twist_assist_supported = true,
                block_to_block_supported = true,
                auto_relock_supported = true,

                inside_handle_ids = { 1, 2, 3, 4 },
                outside_handle_ids = { 1, 2, 3, 4 },

                door_lock_modes = {
                    "unsecured",
                    "unsecured_with_timeout",
                    "unsecured_for_inside",
                    "unsecured_for_inside_with_timeout",
                    "secured",
                    "unknown"
                },

                door_supported = true,
                bolt_supported = true,
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

local lock_timeout_id = find_setting_id("lock_timeout")
local timed_operations_enabled_id = find_setting_id("timed_operations_enabled")
local hold_and_release_timeout_id = find_setting_id("hold_and_release_timeout")
local auto_relock_timeout_id = find_setting_id("auto_relock_timeout")
local twist_assist_enabled_id = find_setting_id("twist_assist_enabled")
local block_to_block_enabled_id = find_setting_id("block_to_block_enabled")
local enabled_inside_handle_ids_id = find_setting_id("enabled_inside_handle_ids")
local enabled_outside_handle_ids_id = find_setting_id("enabled_outside_handle_ids")

do -- set lock timeout when timed operations are disabled
    set_setting_value({ 
        setting_id = lock_timeout_id,
        value = 50
    })

    tests.assert_eq( core.get_setting(lock_timeout_id).status, "synced", "check status is synced" )

    tests.assert_eq( core.get_setting(lock_timeout_id).value, 50, "check lock timeout value changed immediatelly" )
    tests.assert_eq( core.get_setting(timed_operations_enabled_id).value, false, "check timed operations are disabled" )

    tests.assert_called(zwave.set_door_lock_configuration, "check set_door_lock_configuration was not called", { })
    tests.assert_called(zwave.request_door_lock_configuration, "check request_door_lock_configuration was not called", { })
end

do -- set timed operations enabled
    set_setting_value({ 
        setting_id = timed_operations_enabled_id,
        value = true
    })

    tests.assert_eq( core.get_setting(timed_operations_enabled_id).value, true, "check timed operations enabled" )
    tests.assert_eq( core.get_setting(timed_operations_enabled_id).status, "pending", "check status became pending" )

    tests.assert_eq( get_fail_timer_arguments(timed_operations_enabled_id), { setting_id = timed_operations_enabled_id, value = true } )

    local configuration = {
        enabled_inside_handle_ids = { },
        enabled_outside_handle_ids = { },
        lock_timeout = 50,
        auto_relock_timeout = 0,
        hold_and_release_timeout = 0,
        twist_assist_enabled = false,
        block_to_block_enabled = false
    }

    tests.assert_called(zwave.set_door_lock_configuration, "check set_door_lock_configuration was called",  { { "9000", 0, configuration } })
    tests.assert_called(zwave.request_door_lock_configuration, "check request_door_lock_configuration was called", { { "9000", 0 } })

    events_handler( {
        event = "door_lock_configuration_updated",
        node_id = 9000,
        channel_id = 0,
        value = configuration
    } )

    tests.assert_eq( core.get_setting(timed_operations_enabled_id).status, "synced", "check status became " )
    tests.assert_eq( get_fail_timer_arguments(timed_operations_enabled_id), nil )
end

do -- set lock timeout when timed operations are enabled
    set_setting_value({ 
        setting_id = lock_timeout_id,
        value = 60
    })

    local configuration = {
        enabled_inside_handle_ids = { },
        enabled_outside_handle_ids = { },
        lock_timeout = 60,
        auto_relock_timeout = 0,
        hold_and_release_timeout = 0,
        twist_assist_enabled = false,
        block_to_block_enabled = false
    }

    tests.assert_called(zwave.set_door_lock_configuration, "check set_door_lock_configuration was called",  { { "9000", 0, configuration } })
    tests.assert_called(zwave.request_door_lock_configuration, "check request_door_lock_configuration was called", { { "9000", 0 } })

    events_handler( {
        event = "door_lock_configuration_updated",
        node_id = 9000,
        channel_id = 0,
        value = configuration
    } )
end

do -- disabe timed operations and set new lock timeout
    set_setting_value({ 
        setting_id = timed_operations_enabled_id,
        value = false
    })

    tests.assert_called(
        zwave.set_door_lock_configuration, 
        "check set_door_lock_configuration was called",  
        {
            {
                "9000",
                0,
                {
                    enabled_inside_handle_ids = { },
                    enabled_outside_handle_ids = { },
                    auto_relock_timeout = 0,
                    hold_and_release_timeout = 0,
                    twist_assist_enabled = false,
                    block_to_block_enabled = false
                }
            }
        })
    tests.assert_called(zwave.request_door_lock_configuration, "check request_door_lock_configuration was called", { { "9000", 0 } })

    set_setting_value({ 
        setting_id = lock_timeout_id,
        value = 30
    })

    tests.assert_eq( core.get_setting(lock_timeout_id).value, 30, "check lock timeout value changed immediatelly" )
    tests.assert_eq( core.get_setting(timed_operations_enabled_id).value, false, "check timed operations disabled" )

    tests.assert_called(zwave.set_door_lock_configuration, "check set_door_lock_configuration was not called", { })
    tests.assert_called(zwave.request_door_lock_configuration, "check request_door_lock_configuration was not called", { })
end

do -- set enabled inside handle ids
    set_setting_value({ 
        setting_id = enabled_inside_handle_ids_id,
        value = { "1", "4" }
    })

    local configuration = {
        enabled_inside_handle_ids = { 1, 4 },
        enabled_outside_handle_ids = { },
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

do -- set enabled outside handle ids
    set_setting_value({ 
        setting_id = enabled_outside_handle_ids_id,
        value = { "1", "2" }
    })

    local configuration = {
        enabled_inside_handle_ids = { 1, 4 },
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

do -- change scope of other configs
    set_setting_value({ 
        setting_id = auto_relock_timeout_id,
        value = 10
    })

    set_setting_value({ 
        setting_id = hold_and_release_timeout_id,
        value = 20
    })

    set_setting_value({ 
        setting_id = block_to_block_enabled_id,
        value = true
    })

    set_setting_value({ 
        setting_id = twist_assist_enabled_id,
        value = true
    })

    tests.assert_eq( core.get_setting(auto_relock_timeout_id).status, "pending", "check status became pending" )
    tests.assert_eq( core.get_setting(hold_and_release_timeout_id).status, "pending", "check status became pending" )
    tests.assert_eq( core.get_setting(block_to_block_enabled_id).status, "pending", "check status became pending" )
    tests.assert_eq( core.get_setting(twist_assist_enabled_id).status, "pending", "check status became pending" )
    tests.assert_eq( core.get_setting(lock_timeout_id).status, "synced", "check lock timeout status is not changed" )


    -- May be use timer to avoid multiple set+request
    tests.assert_called(
        zwave.set_door_lock_configuration, 
        "check set_door_lock_configuration was called",  
        {
            {
                "9000",
                0,
                {
                    enabled_inside_handle_ids = { 1, 4 },
                    enabled_outside_handle_ids = { 1, 2 },
                    auto_relock_timeout = 10,
                    hold_and_release_timeout = 0,
                    twist_assist_enabled = false,
                    block_to_block_enabled = false
                }
            },
            {
                "9000",
                0,
                {
                    enabled_inside_handle_ids = { 1, 4 },
                    enabled_outside_handle_ids = { 1, 2 },
                    auto_relock_timeout = 10,
                    hold_and_release_timeout = 20,
                    twist_assist_enabled = false,
                    block_to_block_enabled = false
                }
            },
            {
                "9000",
                0,
                {
                    enabled_inside_handle_ids = { 1, 4 },
                    enabled_outside_handle_ids = { 1, 2 },
                    auto_relock_timeout = 10,
                    hold_and_release_timeout = 20,
                    twist_assist_enabled = false,
                    block_to_block_enabled = true
                }
            },
            {
                "9000",
                0,
                {
                    enabled_inside_handle_ids = { 1, 4 },
                    enabled_outside_handle_ids = { 1, 2 },
                    auto_relock_timeout = 10,
                    hold_and_release_timeout = 20,
                    twist_assist_enabled = true,
                    block_to_block_enabled = true
                }
            }
        })

    tests.assert_called( zwave.request_door_lock_configuration, "check request_door_lock_configuration was called", nil, 4 )
end

do
    local item_id = storage:call("find_item_id", item_names.door_lock, 9000, class_ids.door_lock, 0)
    assert(item_id)
    set_item_value({
        item_id = item_id,
        value = "secured"
    })

    tests.assert_called(zwave.set_door_lock_mode, "check setting zwave door lock mode", { { "9000", 0, "secured" } })
    tests.assert_called(zwave.request_door_lock_state, "check door lock state not requested", {})
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



