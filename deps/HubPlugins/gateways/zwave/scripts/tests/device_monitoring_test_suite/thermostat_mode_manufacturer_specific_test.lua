-- Test case for Thermostat Mode CC in MANUFACTURER_SPECIFIC (0x1F) mode:
--   #1 Verify that the item was created and its value is updated
--   #2 Verify that the setting was created
--   #3 Simulate set_setting_value when mode IS NOT "manufacturer_specific" to verify that there're no zwave requests
--   #4 Simulate set_item_value to verify zwave requests
--   #5 Simulate thermostat_mode_updated event with manufacturer specific data to verify item/setting value update
--   #6 Simulate set_setting_value when mode is "manufacturer_specific" to verify zwave requests & timers

local tests = require("tests")

local timer = require( "timer" )

local core = require("core")
core.update_item_value = tests.track_calls(core.update_item_value)
core.set_setting_value = tests.track_calls(core.set_setting_value)

local zwave = require("zwave")
zwave.set_thermostat_mode = tests.create_mock()
zwave.request_node_values = tests.create_mock()
zwave.request_value = tests.create_mock()

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local set_item_value = hubplugins.weak_loadfile( "HUB:zwave/scripts/set_item_value" )
local set_setting_value = hubplugins.weak_loadfile( "HUB:zwave/scripts/set_setting_value" )

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")

local node = {
    id = 9000,
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,
    command_span = 0,
    specific_type = "",
    generic_type = "",

    security = { "none" },
    security_class_started = { "none" },

    channels = { { id = 0,
                   classes = {
                       {
                           id = class_ids.thermostat_mode,
                           modes = {
                               mode_enum.token_to_value["off"],
                               mode_enum.token_to_value["manufacturer_specific"]
                           }
                       } } } }
}

-- prepare zwave
zwave.add_node(node)

-- add node
events_handler({
    event = "node_added",
    node_id = node.id
})

events_handler({
    event = "thermostat_mode_updated",
    node_id = node.id,
    channel_id = 0,
    class_id = class_ids.thermostat_mode,
    mode = mode_enum.token_to_value["off"]
})
timer.cancel("zwave_thermostats_energy_saving_mode_changed_timer")

-- #1 Verify that the item was created and its value is updated
local item_id = storage:call("find_item_id", item_names.thermostat_mode, node.id, class_ids.thermostat_mode, 0)
tests.assert_ne(item_id, nil, item_names.thermostat_mode .. " item not found")
tests.assert_called(core.update_item_value, "check call to core.update_item_value after thermostat_mode_updated",
        { { item_id, "off" } })

local setting_name = "thermostat_mode_manufacturer_specific_data"

-- #2 Verify that the setting was created
local setting_id = storage:call("find_setting_id", node.id, class_ids.thermostat_mode, setting_name)
tests.assert_ne(setting_id, nil, setting_name .. " setting not found")
local settings_fail_timer_id = hubplugins.object("settings_fail_timer", "HUB:zwave/scripts/timer_handlers", setting_id)._timer_id

local manufacturer_specific_data1 = "010203"
local manufacturer_specific_data2 = "040506"

-- #3 Simulate set_setting_value when mode IS NOT "manufacturer_specific" to verify that there're no zwave requests
set_setting_value({setting_id = setting_id, value = manufacturer_specific_data1})

tests.assert_eq( timer.get_time_to_execution(settings_fail_timer_id), nil, "check settings_fail_timer disabled after set_setting_value" )
tests.assert_called(core.set_setting_value, "check call to core.set_setting_value after set_setting_value",
        { { setting_id, manufacturer_specific_data1, "synced" } })
tests.assert_called(core.update_item_value, "ensure core.update_item_value IS NOT called after set_setting_value", nil, 0)

-- #4 Simulate set_item_value to verify zwave requests
set_item_value({ item_id = item_id, value = "manufacturer_specific" })

tests.assert_called(zwave.set_thermostat_mode, "check call to zwave.set_thermostat_mode after set_item_value",
        { { node.id, 0, mode_enum.token_to_value["manufacturer_specific"], manufacturer_specific_data1 } })

timer.add_time(2000)
tests.assert_called( zwave.request_value, "check zwave.request_value called after set_item_value and timeout",
        { { node.id, class_ids.thermostat_mode, 0 } })

-- #5 Simulate thermostat_mode_updated event with manufacturer specific data to verify item/setting value update
timer.cancel = tests.create_mock()
events_handler({
    event = "thermostat_mode_updated",
    node_id = node.id,
    channel_id = 0,
    class_id = class_ids.thermostat_mode,
    mode = mode_enum.token_to_value["manufacturer_specific"],
    manufacturer_specific_data = manufacturer_specific_data1
})

local thermostat_mode_update_timer_id = hubplugins.object("thermostat_mode_update_timer", "HUB:zwave/scripts/timer_handlers", node.id, 0)._timer_id
tests.assert_called_unordered(
        timer.cancel,
        "check timer.cancel called after thermostat_mode_updated",
        { { settings_fail_timer_id }, { thermostat_mode_update_timer_id } } )
tests.assert_called(core.update_item_value, "check call to core.update_item_value after thermostat_mode_updated",
        { { item_id, "manufacturer_specific" } })
tests.assert_called(core.set_setting_value, "check call to core.set_setting_value after thermostat_mode_updated",
        { { setting_id, manufacturer_specific_data1, "synced" } })

-- #6 Simulate set_setting_value when mode is "manufacturer_specific" to verify zwave requests
set_setting_value({setting_id = setting_id, value = manufacturer_specific_data2})

tests.assert_eq( timer.get_time_to_execution(settings_fail_timer_id), 30000, "check settings_fail_timer enabled after set_setting_value" )

tests.assert_called(core.set_setting_value, "check call to core.set_setting_value after set_setting_value",
        { { setting_id, manufacturer_specific_data2, "pending" } })
tests.assert_called(zwave.set_thermostat_mode, "check call to zwave.set_thermostat_mode after set_setting_value",
        { { node.id, 0, mode_enum.token_to_value["manufacturer_specific"], manufacturer_specific_data2 } })
tests.assert_called( zwave.request_value, "check zwave.request_value called after set_setting_value",
        { { node.id, class_ids.thermostat_mode, 0 } })
