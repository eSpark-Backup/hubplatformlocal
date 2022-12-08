local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestDoorLock",
    category = "door_lock",
    type = "doorlock",
    device_type_id = "test",
    battery_powered = false,
    items = { "door_lock", "user_lock_operation", "user_codes" }
}

return info
