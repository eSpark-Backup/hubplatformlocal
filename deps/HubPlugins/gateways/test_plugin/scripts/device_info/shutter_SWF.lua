local info = loadfile( "HUB:test_plugin/scripts/device_info/default" )()

info.master_device = {
    name = "TestMotor",
    device_type_id = "622_21075_23089",
    category = "window_cov",
    subcategory = "window_cov",
    type = "shutter.roller",
    battery_powered = true,
    reachable = true,
    persistent = false,
    room_id = "",
    security = "no",
    ready = true,
    status = "idle",
    info = {
         manufacturer = "Springs Window Fashions",
         model = "RSZ1"
    },
    items = { "dimmer", "dimmer_up", "dimmer_down", "dimmer_stop", "switch", "battery", "goto_favorite", "shutter_state", "shutter_command" }
}

return info
