local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")
local timer = require("timer")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()
timer.cancel = tests.create_mock()


local channel_0 = {
    id = 0,
    classes = { 
        {
            id = class_ids.thermostat_setback,
            version = 1
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

local function get_setting_id(tag)
    return assert(storage:call("find_setting_id", 9000, class_ids.thermostat_setback, tag), "setting id with tag '" .. tag .. "' must be present")
end

local function get_setting(tag)
    return assert(core.get_setting(get_setting_id(tag)))
end

local function get_fail_timer_id( tag )
    return hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", get_setting_id( tag ) )._timer_id
end

local function assert_timer_cancelled()
    tests.assert_called_unordered( 
        timer.cancel, 
        "check fail timers cancelled", 
        { 
            { get_fail_timer_id("thermostat_setback_mode") }, 
            { get_fail_timer_id("thermostat_setback_value") },
            { get_fail_timer_id("thermostat_setback_override_type") }
        })
end

-- custom value, peramnent override
do
    events_handler( {
        event = "thermostat_setback_value_updated",
        node_id = "9000",
        channel_id = 0,
        override_type = "permanent_override",
        mode = "custom_value",
        value = 11.6
    } )

    tests.assert_eq(get_setting("thermostat_setback_value").value, { value = 11.6, scale = "celsius", relative = true }, "check value" )
    tests.assert_eq( get_setting("thermostat_setback_value").status, "synced", "check value status" )

    tests.assert_eq( get_setting("thermostat_setback_mode").value, "custom_value", "check mode" )
    tests.assert_eq( get_setting("thermostat_setback_mode").status, "synced", "check mode status" )

    tests.assert_eq( get_setting("thermostat_setback_override_type").value, "permanent_override", "check override type" )
    tests.assert_eq( get_setting("thermostat_setback_override_type").status, "synced", "check override type status" )

    assert_timer_cancelled()
end

-- frost_protection, temporary override
do
    core.update_setting_status( get_setting_id("thermostat_setback_value"), "pending" )
    core.update_setting_status( get_setting_id("thermostat_setback_mode"), "pending" )
    core.update_setting_status( get_setting_id("thermostat_setback_override_type"), "pending" )

    events_handler( {
        event = "thermostat_setback_value_updated",
        node_id = "9000",
        channel_id = 0,
        override_type = "temporary_override",
        mode = "frost_protection"
    } )

    tests.assert_eq(get_setting("thermostat_setback_value").value, { value = 11.6, scale = "celsius", relative = true }, "check value" )
    tests.assert_eq( get_setting("thermostat_setback_value").status, "synced", "check value status" )

    tests.assert_eq( get_setting("thermostat_setback_mode").value, "frost_protection", "check mode" )
    tests.assert_eq( get_setting("thermostat_setback_mode").status, "synced", "check mode status" )

    tests.assert_eq( get_setting("thermostat_setback_override_type").value, "temporary_override", "check override type" )
    tests.assert_eq( get_setting("thermostat_setback_override_type").status, "synced", "check override type status" )

    assert_timer_cancelled()
end

-- unknown mode
do
    events_handler( {
        event = "thermostat_setback_value_updated",
        node_id = "9000",
        channel_id = 0,
        override_type = "no_override",
        mode = "unknown",
        value = 125
    } )

    tests.assert_eq(get_setting("thermostat_setback_value").value, { value = 0, scale = "celsius", relative = true }, "check value" )
    tests.assert_eq( get_setting("thermostat_setback_value").status, "synced", "check value status" )

    tests.assert_eq( get_setting("thermostat_setback_mode").value, "unknown", "check mode" )
    tests.assert_eq( get_setting("thermostat_setback_mode").status, "synced", "check mode status" )

    tests.assert_eq( get_setting("thermostat_setback_override_type").value, "no_override", "check override type" )
    tests.assert_eq( get_setting("thermostat_setback_override_type").status, "synced", "check override type status" )

    assert_timer_cancelled()
end

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )
