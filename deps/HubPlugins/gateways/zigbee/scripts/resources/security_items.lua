-- copied from zwave resources

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    [item_names.co_alarm] = {
        co_detected = true,
    },
    [item_names.co2_alarm] = {
        co2_detected = true,
    },
    [item_names.smoke_alarm] = {
        smoke_detected = true,
    },
    [item_names.intrusion_alarm] = {
        intrusion = true,
    },
    [item_names.dw_state] = {
        dw_is_opened = true
    },
    [item_names.heat_alarm] = {
        overheat_detected = true,
        under_heat_detected = true,
    },
    [item_names.gas_alarm] = {
        toxic_gas_detected = true,
        combustible_gas_detected = true,
    },
    [item_names.motion] = {
        [true] = true
    },
    [item_names.glass_breakage_alarm] = {
        glass_breakage = true
    },
    [item_names.water_leak_alarm] = {
        water_leak_detected = true
    },
    [item_names.emergency_state] = {
        panic = true,
        emergency = true,
        emergency_and_panic = true
    }
}
