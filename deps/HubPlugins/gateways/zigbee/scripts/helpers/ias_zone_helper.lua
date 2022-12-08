local helper = {}

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local smoke_alarm_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/smoke_alarm_event")()
local water_leak_alarm_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/water_leak_alarm_event")()
local co_alarm_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/co_alarm_event")()
local dw_state_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/dw_state_event")()
local emergency_state = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()

local item_values_mappings = {
    [item_names.motion] = { on = true, off = false },
    [item_names.smoke_alarm] = { on = smoke_alarm_enum.smoke_detected, off = smoke_alarm_enum.no_smoke },
    [item_names.water_leak_alarm] = { on = water_leak_alarm_enum.water_leak_detected, off = water_leak_alarm_enum.no_water_leak },
    [item_names.co_alarm] = { on = co_alarm_enum.co_detected, off = co_alarm_enum.no_co },
    [item_names.dw_state] = { on = dw_state_enum.dw_is_opened, off = dw_state_enum.dw_is_closed }
}

local zone_type_to_item_name = {
    motion_sensor = item_names.motion,
    contact_switch = item_names.dw_state,
    remote_control = item_names.emergency_state,
    key_fob = item_names.emergency_state,
    keypad = item_names.emergency_state
}

local alarm_type_to_item_name = {
    fire_sensor = { alarm_1 = item_names.smoke_alarm },
    water_sensor = { alarm_1 = item_names.water_leak_alarm },
    gas_sensor = { alarm_1 = item_names.co_alarm },
}

function helper.zone_type_to_item_name(zone_type, alarm_type)
    if not alarm_type then
        return zone_type_to_item_name[zone_type]
    elseif alarm_type_to_item_name[zone_type] then
        return alarm_type_to_item_name[zone_type][alarm_type]
    else
        return nil
    end
end

function helper.zone_status_to_item_value(item_name, zone_status)
    if item_values_mappings[item_name] then
        return item_values_mappings[item_name][zone_status]
    end
end

function helper.alarm_statuses_to_item_value(item_name, alarm1_status, alarm2_status)
    if item_name == item_names.emergency_state then
        if alarm1_status == "on" and alarm2_status == "on" then
            return emergency_state.emergency_and_panic
        elseif alarm1_status == "on" then
            return emergency_state.panic
        elseif alarm2_status == "on" then
            return emergency_state.emergency
        else
            return emergency_state.idle
        end
    else
        local value = (alarm1_status == "on" or alarm2_status == "on") and "on" or "off"
        return helper.zone_status_to_item_value(item_name, value)

    end
end

function helper.get_additional_states(zone_type)
    if zone_type == "remote_control" or
       zone_type == "key_fob" or
       zone_type == "keypad" then
        return {emergency_state.emergency, emergency_state.panic, emergency_state.emergency_and_panic}
   end
end

function helper.add_to_child_device(item_name)
    return true
end

return helper
