local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require( "core" )
local timer = require( "timer" )

zwave.request_node_values = tests.create_mock()
zwave.request_setpoint_value = tests.create_mock()

local channel_0 = { 
    id = 0,
    classes = { 
        { id = class_ids.thermostat_mode,
            modes = { 0,1,2,7,13 }
        },
        { id = class_ids.thermostat_setpoint,
            setpoints = {
                {type = 1},
                {type = 2},
                {type = 9},
                {type = 15}
            }
        }
    }
}

local node = {
    id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { channel_0 }
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )

local set_setting_value = loadfile( "HUB:zwave/scripts/set_setting_value" )

local setting_id = storage:call("find_setting_id", 9000, class_ids.thermostat_setpoint, "controlled_setpoint_type")
local thermostat_setpoint_item_id = storage:call("find_item_id", item_names.thermostat_setpoint, 9000, class_ids.thermostat_setpoint, 0)
local thermostat_setpoint_heating_item_id = storage:call("find_item_id", item_names.thermostat_setpoint_heating, 9000, class_ids.thermostat_setpoint, 0)
local thermostat_setpoint_cooling_item_id = storage:call("find_item_id", item_names.thermostat_setpoint_cooling, 9000, class_ids.thermostat_setpoint, 0)
local thermostat_mode_item_id = storage:call("find_item_id", item_names.thermostat_mode, 9000, class_ids.thermostat_mode, 0)

assert(setting_id)

-- set cooling mode to setting
do
    set_setting_value({
        setting_id = setting_id,
        value = "cooling"
    })

    tests.assert_eq(core.get_setting(setting_id).value, "cooling", "check value set immediatelly")
    tests.assert_eq(core.get_setting(setting_id).status, "synced", "check status synced immediatelly")
    tests.assert_eq(core.get_item(thermostat_setpoint_item_id).show, false)
    tests.assert_eq(core.get_item(thermostat_setpoint_heating_item_id).show, false)
    tests.assert_eq(core.get_item(thermostat_setpoint_cooling_item_id).show, true)

    timer.add_time(10000)

    tests.assert_called(zwave.request_setpoint_value, "check request setpoint value after update", { { "9000", 0, 2 } })
end

-- send a few setpoint events 
do
    events_handler( {
                event = "thermostat_setpoint_value_updated", 
                node_id = "9000", 
                channel_id = 0, 
                class_id = class_ids.thermostat_setpoint, 
                type = 1, 
                value = 40, 
                scale = 1
            } )

    events_handler( {
            event = "thermostat_setpoint_value_updated", 
            node_id = "9000", 
            channel_id = 0, 
            class_id = class_ids.thermostat_setpoint, 
            type = 15, 
            value = 40, 
            scale = 1
        } )

    tests.assert_eq(core.get_item(thermostat_setpoint_item_id).value, { value = 0, scale = "celsius" })
    tests.assert_eq(core.get_item(thermostat_setpoint_heating_item_id).value, { value = 0, scale = "celsius" })
    tests.assert_eq(core.get_item(thermostat_setpoint_cooling_item_id).value, { value = 0, scale = "celsius" })

    events_handler( {
            event = "thermostat_setpoint_value_updated", 
            node_id = "9000", 
            channel_id = 0, 
            class_id = class_ids.thermostat_setpoint, 
            type = 2, 
            value = 40, 
            scale = 1
        } )

    tests.assert_eq(core.get_item(thermostat_setpoint_item_id).value, { value = 0, scale = "celsius" })
    tests.assert_eq(core.get_item(thermostat_setpoint_heating_item_id).value, { value = 0, scale = "celsius" })
    tests.assert_eq(core.get_item(thermostat_setpoint_cooling_item_id).value, { value = 40, scale = "fahrenheit" })
end

-- send thermosstat mode event, check required mode controlled
do
    events_handler( {
            event = "thermostat_mode_updated",
            node_id = "9000", 
            channel_id = 0,
            class_id = class_ids.thermostat_mode,
            mode = 1
        } )

    tests.assert_eq(core.get_item(thermostat_mode_item_id).value, "heat")

    tests.assert_eq(core.get_item(thermostat_setpoint_item_id).show, false)
    tests.assert_eq(core.get_item(thermostat_setpoint_heating_item_id).show, false)
    tests.assert_eq(core.get_item(thermostat_setpoint_cooling_item_id).show, true)

    timer.add_time(10000)

    tests.assert_called(zwave.request_setpoint_value, "check request setpoint value after update", { { "9000", 0, 2 } })
end

-- set default mode to setting, check controlled setpoint type is taken from mode (heat)
do
    set_setting_value({
        setting_id = setting_id,
        value = "default"
    })

    tests.assert_eq(core.get_setting(setting_id).value, "default", "check value set immediatelly")
    tests.assert_eq(core.get_setting(setting_id).status, "synced", "check status synced immediatelly")
    tests.assert_eq(core.get_item(thermostat_setpoint_item_id).show, false)
    tests.assert_eq(core.get_item(thermostat_setpoint_heating_item_id).show, true)
    tests.assert_eq(core.get_item(thermostat_setpoint_cooling_item_id).show, false)

    timer.add_time(10000)

    tests.assert_called(zwave.request_setpoint_value, "check request setpoint value after update", { { "9000", 0, 1 } })
end
