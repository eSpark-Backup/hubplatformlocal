local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestGasMeter",
    category = "flow_meter",
    type = "meter.flow",
    device_type_id = "test",
    battery_powered = true,
    items = { "gas_meter_pulse", "gas_meter_volume", "meter_reset" }
}

return info
