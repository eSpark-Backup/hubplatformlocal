local zigbee_node = ...

local ias_zone_type_attr = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/find_attribute")(
    zigbee_node,
    1,
    "ias_zone",
    "ZoneType"
)

if not ias_zone_type_attr then return end

local zone_type_to_device_info = {
    remote_control = {
        name = "Remote Control",
        category = "remote_control"
    },
    keypad = {
        name = "Keypad",
        category = "keypad"
    },
    key_fob = {
        name = "Remote Control",
        category = "remote_control"
    }
}

return zone_type_to_device_info[ias_zone_type_attr.formatted_value.zone_type]
