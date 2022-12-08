local water_valve = {
    valve_value = false,
    sensors_count = 0,
    is_control_inverted = false
}

function water_valve.subscribe(script)
    print("Plugin subscribed on water valve events")
end

function water_valve.unsubscribe(script)
    print("Plugin unsubscribed on water valve events")
end

function water_valve.request_all_values()
    print("All water valve values requested")
end

function water_valve.set_water_switch_value(value)
    water_valve.valve_value = value
end

function water_valve.get_water_leak_sensors_count()
    return water_valve.sensors_count
end

function water_valve.should_invert_control()
    return water_valve.is_control_inverted
end

return water_valve
