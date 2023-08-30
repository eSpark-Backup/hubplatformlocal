return {
    gateway_id = "gateway_id",
    name = "Unknown",
    type = "device",
    device_type_id = "",
    category = "unknown",
    battery_powered = false,
    user_code_restriction = "^\\d{4,8}$",
    attr_configurations = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/default_attribute_configurations")(),
    mfg_specific_attr_configurations = {},
    has_ias_zone_tamper = true,
    default_min_max_values = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/default_min_max_values")()
}
