local params = ...

local icon_type = params.icon_type

-- main
print("Detect device category with icon: " .. icon_type)

return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_light_bulb")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_light_plugged")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_light_in_wall")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_switch_interrior")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_switch_in_wall")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_switch_valve")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_switch_relay")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_door_lock")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_window_covering")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_remote_control")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_generic_io")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_generic_io_repeater")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_serial_port")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_av")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_siren")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_mouse_trap")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_door_bell")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_keypad")(icon_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_garage_door")(icon_type)
