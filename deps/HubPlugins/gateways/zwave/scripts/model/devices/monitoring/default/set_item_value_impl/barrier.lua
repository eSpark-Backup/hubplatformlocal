local self, item_descriptor, value = ...

local zwave = require("zwave")

zwave.set_token_value(item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, value)

hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(item_descriptor)
