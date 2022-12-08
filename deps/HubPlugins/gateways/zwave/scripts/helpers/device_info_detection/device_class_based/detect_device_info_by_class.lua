local params = ...

local generic_type = params.generic_type
local specific_type = params.specific_type
local channels = params.channels


local function collect_ccs(channels)
    local result = {}
    for _, channel in ipairs(channels) do
        if channel.classes then
            for _, cc in ipairs(channel.classes) do
                result[cc.id] = true
            end
        end
    end

    return result
end

-- main
print("Detect device category with generic type: " .. generic_type .. ", specific type: " .. specific_type)

local ccs = collect_ccs(channels)

return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_light_rgb")(generic_type, specific_type, ccs)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_light_in_wall")(generic_type, specific_type, ccs)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_switch_in_wall")(generic_type, specific_type, ccs)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_switch_valve")(generic_type, specific_type, ccs)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_thermostat")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_thermostat_heating")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_door_lock")(generic_type, specific_type, ccs)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_window_covering")(generic_type, specific_type, ccs)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_remote_control")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_controller")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_generic_io")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_generic_io_repeater")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_serial_port")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_av")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_siren")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_appliance")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_door_bell")(generic_type, specific_type)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_garage_door")(generic_type, specific_type, ccs)
    or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_keypad")(generic_type, specific_type, ccs)
