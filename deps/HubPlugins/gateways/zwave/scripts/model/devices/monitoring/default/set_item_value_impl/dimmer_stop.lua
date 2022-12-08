local self, item_descriptor, value = ...

local zwave = require("zwave")

zwave.switch_multilevel_level_change(item_descriptor.node_id, item_descriptor.channel_id, "stop")

hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(item_descriptor)
