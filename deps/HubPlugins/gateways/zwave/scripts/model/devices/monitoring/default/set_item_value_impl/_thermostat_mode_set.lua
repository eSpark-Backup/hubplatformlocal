local self, item_descriptor, value = ...

local manufacturer_data

if value == "manufacturer_specific" then
    local setting_info = self:call("find_setting", item_descriptor.node_id, item_descriptor.class_id, "thermostat_mode_manufacturer_specific_data")
    manufacturer_data = setting_info and setting_info.value
end

local zwave = require("zwave")

local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_mode")
value = mode_enum.token_to_value[value]

zwave.set_thermostat_mode(item_descriptor.node_id, item_descriptor.channel_id, value, manufacturer_data)
