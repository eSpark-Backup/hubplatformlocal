local tests = require( "tests" )

local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/energy_saving_mode_fixture")()
fixture:init()

core.get_local_time = function(...) return "2021-01-01T06:01:01" end
core.get_next_week_event = function(...) return "2022-01-01T01:01:01" end
core.update_item_value(fixture.energy_saving_mode_item_id, "moderate")

-- test heat not shown
do
    core.update_item_value(fixture.setpoint_heating_item_id, { value = 0, scale = "fahrenheit" })
    core.update_item_value(fixture.setpoint_cooling_item_id, { value = 0, scale = "fahrenheit" })  
    fixture:show_item(fixture.setpoint_heating_item_id, false)
      
    zwave.set_setpoint_value = tests.create_mock()
    
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/process_thermostats_energy_saving_mode")()
    
    tests.assert_called(zwave.set_setpoint_value, "check set setpoint value", {
        { "9000", class_ids.thermostat_setpoint, 0, 2, { value = 75, scale = "fahrenheit" } }
    })
end

-- test cool not shown
do
    core.update_item_value(fixture.setpoint_heating_item_id, { value = 0, scale = "fahrenheit" })
    core.update_item_value(fixture.setpoint_cooling_item_id, { value = 0, scale = "fahrenheit" })  
    fixture:show_item(fixture.setpoint_heating_item_id, true)
    fixture:show_item(fixture.setpoint_cooling_item_id, false)
      
    zwave.set_setpoint_value = tests.create_mock()
    
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/process_thermostats_energy_saving_mode")()
    
    tests.assert_called(zwave.set_setpoint_value, "check set setpoint value", {
        { "9000", class_ids.thermostat_setpoint, 0, 1, { value = 70, scale = "fahrenheit" } }
    })
end

-- test both not shown
do
    core.update_item_value(fixture.setpoint_heating_item_id, { value = 0, scale = "fahrenheit" })
    core.update_item_value(fixture.setpoint_cooling_item_id, { value = 0, scale = "fahrenheit" })  
    fixture:show_item(fixture.setpoint_heating_item_id, false)
    fixture:show_item(fixture.setpoint_cooling_item_id, false)
      
    zwave.set_setpoint_value = tests.create_mock()
    
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/process_thermostats_energy_saving_mode")()
    
    tests.assert_called(zwave.set_setpoint_value, "check no set setpoint value", {})
end
