local self, item_descriptor, value = ...

local zwave = require("zwave")

local fan_mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_fan_mode")
value = fan_mode_enum.token_to_value[value]

zwave.set_value(item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, value)

hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(item_descriptor)
