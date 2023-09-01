local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local timer = require("timer")

local set_setting_value = loadfile( "HUB:zwave/scripts/set_setting_value" )
local timer_callback = loadfile("HUB:zwave/scripts/timer_handlers/timeout/set_thermostat_setback")


zwave.request_node_values = tests.create_mock()
zwave.request_value = tests.create_mock()
zwave.set_thermostat_setback_value = tests.create_mock()

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

timer.set_timeout_with_id = tests.create_mock()
timer.cancel = tests.create_mock()

local function get_setting_id(tag)
    return assert(storage:call("find_setting_id", 9000, class_ids.thermostat_setback, tag), "setting id with tag '" .. tag .. "' must be present")
end

local function get_setting(tag)
    return assert(core.get_setting(get_setting_id(tag)))
end

local function get_fail_timer_id(tag)
    return hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", get_setting_id( tag ) )._timer_id
end

local setback_timer_id = hubplugins.object("thermostat_setback_timer", "HUB:zwave/scripts/timer_handlers", 9000, 0 )._timer_id

local function assert_set_timeout_with_id_called(arguments, tag)
    local expectation = {}

    if arguments then
        table.insert(
                expectation,
                { 
                    3000, 
                    setback_timer_id, 
                    "HUB:zwave/scripts/timer_handlers/timeout/set_thermostat_setback", 
                    arguments
                })
    end

    if tag then
        table.insert(
                expectation,
                {
                    30000,
                    get_fail_timer_id(tag), 
                    "HUB:zwave/scripts/timer_handlers/timeout/set_failed_setting_status",
                    { setting_id = get_setting_id(tag) }
                })
    end

    tests.assert_called(timer.set_timeout_with_id, "check timer set", expectation)
end



-- set value twice, mode, override type
do
    local arguments 

    -- set value
    timer.get_arguments = tests.create_mock(arguments)

    set_setting_value({
            setting_id = get_setting_id( "thermostat_setback_value" ),
            value = {
                value = -10.1,
                scale = "celsius"
            }
        })

    arguments = {  
        node_id = "9000",
        channel_id = 0,
        thermostat_setback_value = {
            value = -10.1,
            scale = "celsius",
            relative = true
        }
    }

    tests.assert_called( timer.cancel, "check timer.cancel for fail called", { { get_fail_timer_id("thermostat_setback_value") } })
    assert_set_timeout_with_id_called(arguments, "thermostat_setback_value")
    
    tests.assert_eq(get_setting("thermostat_setback_value").value, { value = -10.1, scale = "celsius", relative = true })

    tests.assert_eq( get_setting("thermostat_setback_value").status, "pending")


    -- set mode
    timer.get_arguments = tests.create_mock(arguments)

    set_setting_value({
        setting_id = get_setting_id( "thermostat_setback_mode" ),
        value = "custom_value"
    })

    tests.assert_called_unordered( timer.cancel, "check timer.cancel called", { { get_fail_timer_id("thermostat_setback_mode") }, { setback_timer_id } })
    assert_set_timeout_with_id_called(arguments, "thermostat_setback_mode")

    arguments.thermostat_setback_mode = "custom_value"

    tests.assert_eq(get_setting("thermostat_setback_mode").value, "custom_value")
    tests.assert_eq(get_setting("thermostat_setback_value").status, "pending")


    -- set override type

    timer.get_arguments = tests.create_mock(arguments)

    set_setting_value({
        setting_id = get_setting_id( "thermostat_setback_override_type" ),
        value = "permanent_override"
    })


    tests.assert_called_unordered( timer.cancel, "check timer.cancel called", { { get_fail_timer_id("thermostat_setback_override_type") }, { setback_timer_id } })
    assert_set_timeout_with_id_called(arguments, "thermostat_setback_override_type")

    arguments.thermostat_setback_override_type = "permanent_override"

    tests.assert_eq(get_setting("thermostat_setback_override_type").value, "permanent_override")
    tests.assert_eq(get_setting("thermostat_setback_override_type").status, "pending")

    -- set value again
    timer.get_arguments = tests.create_mock(arguments)

    set_setting_value({
        setting_id = get_setting_id( "thermostat_setback_value" ),
        value = {
            value = 29,
            scale = "celsius"
        }
    })

    tests.assert_called_unordered( timer.cancel, "check timer.cancel called", { { get_fail_timer_id("thermostat_setback_value") }, { setback_timer_id } })
    assert_set_timeout_with_id_called(arguments, "thermostat_setback_value")

    arguments.thermostat_setback_value.value = 12

    tests.assert_eq(get_setting("thermostat_setback_value").value, { value = 12, scale = "celsius", relative = true })
    tests.assert_eq(get_setting("thermostat_setback_value").status, "pending")

    -- call timer callback
    timer_callback( arguments )

    tests.assert_called( zwave.set_thermostat_setback_value, "check set_thermostat_setback_value called", { { "9000", 0, "permanent_override", "custom_value", 12 } })
    tests.assert_called( zwave.request_value, "check request_value called", { { "9000", class_ids.thermostat_setback, 0 } })
end


-- set mode only
do
    core.set_setting_value( get_setting_id("thermostat_setback_override_type"), "temporary_override", "synced" )
    core.set_setting_value( get_setting_id("thermostat_setback_value"), { value = 1.1, scale = "celsius", relative = true }, "synced" )

    -- set mode
    timer.get_arguments = tests.create_mock()


    local arguments = {
        node_id = "9000",
        channel_id = 0,
        thermostat_setback_mode = "frost_protection"
    }

    set_setting_value({
        setting_id = get_setting_id( "thermostat_setback_mode" ),
        value = "frost_protection"
    })


    tests.assert_called( timer.cancel, "check timer.cancel for fail timer called", { { get_fail_timer_id("thermostat_setback_mode") } })
    assert_set_timeout_with_id_called(arguments, "thermostat_setback_mode")

    tests.assert_eq(get_setting("thermostat_setback_mode").value, "frost_protection")
    tests.assert_eq(get_setting("thermostat_setback_mode").status, "pending")


    -- call timer callback
    timer_callback( arguments )

    tests.assert_called( zwave.set_thermostat_setback_value, "check set_thermostat_setback_value called", { { "9000", 0, "temporary_override", "frost_protection", nil } })
    tests.assert_called( zwave.request_value, "check request_value called", { { "9000", class_ids.thermostat_setback, 0 } })
end



-- set value only for custom_value mode
do
    core.set_setting_value( get_setting_id("thermostat_setback_override_type"), "permanent_override" )
    core.set_setting_value( get_setting_id("thermostat_setback_mode"), "custom_value" )

    -- set mode
    timer.get_arguments = tests.create_mock()


    local arguments = {
        node_id = "9000",
        channel_id = 0,
        thermostat_setback_value = { value = -10, scale = "celsius", relative = true }
    }

    set_setting_value({
        setting_id = get_setting_id( "thermostat_setback_value" ),
        value = { value = -18, scale = "fahrenheit" }
    })


    tests.assert_called( timer.cancel, "check timer.cancel for fail timer called", { { get_fail_timer_id("thermostat_setback_value") } })
    assert_set_timeout_with_id_called(arguments, "thermostat_setback_value")

    tests.assert_eq(get_setting("thermostat_setback_value").value, { value = -10, scale = "celsius", relative = true })
    tests.assert_eq(get_setting("thermostat_setback_value").status, "pending")


    -- call timer callback
    timer_callback( arguments )

    tests.assert_called( zwave.set_thermostat_setback_value, "check set_thermostat_setback_value called", { { "9000", 0, "permanent_override", "custom_value", -10 } })
    tests.assert_called( zwave.request_value, "check request_value called", { { "9000", class_ids.thermostat_setback, 0 } })
end


-- set value only for not custom_value mode
for mode in pairs({ "frost_protection", "energy_saving" }) do
    core.set_setting_value( get_setting_id("thermostat_setback_override_type"), "permanent_override", "synced" )
    core.set_setting_value( get_setting_id("thermostat_setback_mode"), mode, "synced" )

    -- set mode
    timer.get_arguments = tests.create_mock()


    local arguments = {
        node_id = "9000",
        channel_id = 0,
        thermostat_setback_value = { value = -10, scale = "celsius", relative = true }
    }

    set_setting_value({
        setting_id = get_setting_id( "thermostat_setback_value" ),
        value = { value = -18, scale = "fahrenheit" }
    })


    tests.assert_called( timer.cancel, "check timer.cancel for fail timer called", { { get_fail_timer_id("thermostat_setback_value") } })
    assert_set_timeout_with_id_called(arguments, "thermostat_setback_value")


    tests.assert_eq(get_setting("thermostat_setback_value").value, { value = -10, scale = "celsius", relative = true })
    tests.assert_eq(get_setting("thermostat_setback_value").status, "pending")


    -- call timer callback
    timer_callback( arguments )

    tests.assert_called( zwave.set_thermostat_setback_value, "check set_thermostat_setback_value not called", { })
    tests.assert_called( zwave.request_value, "check request_value not called", { })

    tests.assert_eq(get_setting("thermostat_setback_value").value, { value = -10, scale = "celsius", relative = true })
    tests.assert_eq(get_setting("thermostat_setback_value").status, "synced")
end


-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
} )


